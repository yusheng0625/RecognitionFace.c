#include "transactionmgr.h"
#include <QDateTime>
#include "global.h"
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <QJsonArray>
#include <QTime>
TransactionMgr::TransactionMgr(QObject *parent) : QThread(parent)
{
    m_netMgr = new QNetworkAccessManager(this);
    m_replySignIn = NULL;
    m_replySignOut = NULL;
    m_replyChekedOutList = NULL;
    m_bValidCurrentReq = false;
    m_bBusy = false;
    connect(this, SIGNAL(sendRequest()), this, SLOT(onSendRequest()));
    connect(this, SIGNAL(sendRequestCheckedOutList()), this, SLOT(onSendCheckedOutListReq()));
}

void TransactionMgr::_closeReplySignIn()
{
    if(m_replySignIn != NULL)
    {
        disconnect(m_replySignIn, SIGNAL(finished()),this,SLOT(receiveSignInReply()));
        if(m_replySignIn->isFinished()==false)
            m_replySignIn->abort();
        m_replySignIn->close();
        m_replySignIn = NULL;
    }
}

void TransactionMgr::_closeReplySignOut()
{
    if(m_replySignOut != NULL)
    {
        disconnect(m_replySignOut, SIGNAL(finished()),this,SLOT(receiveSignOutReply()));
        if(m_replySignOut->isFinished()==false)
            m_replySignOut->abort();
        m_replySignOut->close();
        m_replySignOut = NULL;
    }
}
void TransactionMgr::_closeReplyCheckedOutList()
{
    if(m_replyChekedOutList != NULL)
    {
        disconnect(m_replyChekedOutList, SIGNAL(finished()),this,SLOT(receiveCheckedOutListReply()));
        if(m_replyChekedOutList->isFinished()==false)
            m_replyChekedOutList->abort();
        m_replyChekedOutList->close();
        m_replyChekedOutList = NULL;
    }
}


void TransactionMgr::onSendRequest()
{
    switch(m_currentReq._type)
    {
        case req_sign_in_frist:
        case req_sign_in_returning:
            sendSignInRequeest();
            break;
        case req_sign_out:
            sendSignOutRequeest();
            break;
    }
}

void TransactionMgr::scheduleSigninFirstUser(Person* pPerson, QImage& img, int nStaffId)
{
#ifdef LOG_FILE_DEBUG
    GlobalData::getInstance()->log("TransactionMgr::scheduleSigninFirstUser");
#endif
    QDateTime tm = QDateTime::currentDateTime();
    QString strTm = tm.toString("yyyy-MM-dd HH:mm");

    TRANSACTTION_REQUEST request;
    RequestSignIn* reqSignIn = new RequestSignIn();

    reqSignIn->m_image = img.copy();
    reqSignIn->m_person.Copy(*pPerson);
    reqSignIn->m_nStaffId = nStaffId;
    reqSignIn->m_strTime = strTm;

    request._type = req_sign_in_frist;
    request._data = reqSignIn;
    _pushRequest(&request);
}

void TransactionMgr::scheduleSigninReturningUser(Person* pPerson, QImage& img, int nStaffId)
{
#ifdef LOG_FILE_DEBUG
    GlobalData::getInstance()->log("TransactionMgr::scheduleSigninReturningUser");
#endif
    QDateTime tm = QDateTime::currentDateTime();
    QString strTm = tm.toString("yyyy-MM-dd HH:mm");

    if(pPerson->m_nVisitorId ==0)
    {
        GlobalData::getInstance()->log("Requesting returning user sign-in for Incompleted sign-in " +
                                       pPerson->m_strFirst + " " + pPerson->m_strLast);
    }

    TRANSACTTION_REQUEST request;
    RequestSignIn* reqSignIn = new RequestSignIn();

    reqSignIn->m_image = img.copy();
    reqSignIn->m_person.Copy(*pPerson);
    reqSignIn->m_nStaffId = nStaffId;
    reqSignIn->m_strTime = strTm;

    request._type = req_sign_in_returning;
    request._data = reqSignIn;

    _pushRequest(&request);
}


void TransactionMgr::scheduleSignOut(int nVisitorId, int nPersonId, int sessionId)
{
#ifdef LOG_FILE_DEBUG
    GlobalData::getInstance()->log("TransactionMgr::scheduleSignOut");
#endif

    QDateTime tm = QDateTime::currentDateTime();
    QString strTm = tm.toString("yyyy-MM-dd HH:mm");

    if(nVisitorId ==0)
    {
        GlobalData::getInstance()->log("Requesting sign-out for Incompleted sign-out person_id=" +
                                       QString::number(nPersonId));
    }


    TRANSACTTION_REQUEST request;
    RequestSignOut* reqSignOut = new RequestSignOut();
    reqSignOut->m_nVisitorId = nVisitorId;
    reqSignOut->m_nPersonId = nPersonId;
    reqSignOut->m_nSessionId = sessionId;
    reqSignOut->m_strTime = strTm;

    request._type = req_sign_out;
    request._data = reqSignOut;

    _pushRequest(&request);

}
void TransactionMgr::_checkQueueForFirstUser(int nPersonId, int nVisitorId, int sessionId)
{
    m_requestsLock.lockForWrite();

    for(int i=0; i<m_requests.size(); i++)
    {
        if(m_requests[i]._type == req_sign_in_returning)
        {
            RequestSignIn* pRegSign = (RequestSignIn*)m_requests[i]._data;
            if(pRegSign->m_person.m_nId == nPersonId)
            {
                pRegSign->m_person.m_nVisitorId = nVisitorId;
            }
        }
        else if(m_requests[i]._type == req_sign_out)
        {
            RequestSignOut* pReqSignOut = (RequestSignOut*)m_requests[i]._data;
            if(pReqSignOut->m_nPersonId == nPersonId)
            {
                GlobalData::getInstance()->log("Update for signout visitor_id=" + QString::number(nVisitorId));
                pReqSignOut->m_nVisitorId = nVisitorId;
                pReqSignOut->m_nSessionId = sessionId;
            }
        }
    }
    m_requestsLock.unlock();
}

void TransactionMgr::_pushRequest(TRANSACTTION_REQUEST* newReq)
{
    m_requestsLock.lockForWrite();
    m_requests.push_back(*newReq);
    m_requestsLock.unlock();
}

bool TransactionMgr::_getRequest(TRANSACTTION_REQUEST* out)
{
//    GlobalData* pGlobal = GlobalData::getInstance();
    bool bRet = false;
    m_requestsLock.lockForRead();
    if(m_requests.size() > 0)
    {
        *out = m_requests[0];
        bRet = true;
        m_requests.erase(m_requests.begin());
    }
    m_requestsLock.unlock();
    return bRet;
}


void TransactionMgr::sendSignOutRequeest()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    RequestSignOut* reqSignOut = (RequestSignOut*)m_currentReq._data;

    QUrlQuery params;
    params.addQueryItem("client_id", QString::number(pGlobal->m_client_id));
    params.addQueryItem("session_id", QString::number(reqSignOut->m_nSessionId));
    params.addQueryItem("visitor_id", QString::number(reqSignOut->m_nVisitorId));
    params.addQueryItem("checkout_datetime", reqSignOut->m_strTime);

    pGlobal->log("Check-out: sending request");
    QUrl url(QString(pGlobal->getCheckOutUrl()));
    url.setQuery(params.query());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    _closeReplySignOut();

    pGlobal->log("checkout url = " + url.toString());

    m_replySignOut = m_netMgr->get(request);
    connect(m_replySignOut,SIGNAL(finished()),this,SLOT(receiveSignOutReply()));
}


void TransactionMgr::sendSignInRequeest()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    RequestSignIn* reqSignIn = (RequestSignIn*)m_currentReq._data;


    QString bound = "margin"; //name of the boundary
    //according to rfc 1867 we need to put this string here:
    QByteArray data(QString("--" + bound + "\r\n").toLatin1());
    data.append("Content-Disposition: form-data; name=\"client_id\"\r\n\r\n");
    data.append(QString::number(pGlobal->m_client_id) + "\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    data.append("--" + bound + "\r\n"); //according to rfc 1867


    data.append("Content-Disposition: form-data; name=\"visitor_id\"\r\n\r\n");
    if(reqSignIn->m_person.m_nVisitorId > 0)
        data.append(QString::number(reqSignIn->m_person.m_nVisitorId) + "\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    else
        data.append("\r\n");
    data.append("--" + bound + "\r\n"); //according to rfc 1867

    int iVisitorType = pGlobal->getVisitorTypeId(reqSignIn->m_person.m_strType);

    data.append("Content-Disposition: form-data; name=\"visitor_type\"\r\n\r\n");
    data.append(QString::number(iVisitorType) + "\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    data.append("--" + bound + "\r\n"); //according to rfc 1867

    data.append("Content-Disposition: form-data; name=\"last_name\"\r\n\r\n");
    data.append(reqSignIn->m_person.m_strLast + "\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    data.append("--" + bound + "\r\n"); //according to rfc 1867

    data.append("Content-Disposition: form-data; name=\"first_name\"\r\n\r\n");
    data.append(reqSignIn->m_person.m_strFirst + "\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    data.append("--" + bound + "\r\n"); //according to rfc 1867

    data.append("Content-Disposition: form-data; name=\"company\"\r\n\r\n");
    data.append(reqSignIn->m_person.m_strCompany + "\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    data.append("--" + bound + "\r\n"); //according to rfc 1867

    data.append("Content-Disposition: form-data; name=\"email\"\r\n\r\n");
    data.append(reqSignIn->m_person.m_strEmail + "\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    data.append("--" + bound + "\r\n"); //according to rfc 1867

    data.append("Content-Disposition: form-data; name=\"checkin_datetime\"\r\n\r\n");
    data.append(reqSignIn->m_strTime + "\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    data.append("--" + bound + "\r\n"); //according to rfc 1867

    data.append("Content-Disposition: form-data; name=\"staff_id\"\r\n\r\n");
    data.append(QString::number(reqSignIn->m_nStaffId) + "\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    data.append("--" + bound + "\r\n"); //according to rfc 1867

    data.append("Content-Disposition: form-data; name=\"location_id\"\r\n\r\n");
    data.append(QString::number(g_nlocId) + "\r\n"); //our script's name, as I understood. Please, correct me if I'm wrong
    data.append("--" + bound + "\r\n"); //according to rfc 1867

    //qimage to bytearray
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    reqSignIn->m_image.save(&buffer, "PNG");

    QString a = QString("Content-Disposition: form-data; name=\"visitor_picture\"; filename=\"%1\"\r\n").arg("photo.png");
    data.append(a.toLatin1()); //name of the input is "uploaded" in my form, next one is a file name.
    data.append("Content-Type: image/jpeg\r\n\r\n"); //data type
    data.append(ba); //let's read the file
    data.append("\r\n");
    data.append("--" + bound + "--\r\n"); //closing boundary according to rfc 1867

    GlobalData::getInstance()->log("Check in: Send request!");

    QUrl url(QString(pGlobal->getCheckInUrl()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data; boundary=" + bound).toLatin1());
    request.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(data.length()).toLatin1());

    _closeReplySignIn();
    m_replySignIn = m_netMgr->post(request, data);
    connect(m_replySignIn, SIGNAL(finished()),this,SLOT(receiveSignInReply()));
}

void TransactionMgr::receiveSignInReply()
{
    QByteArray byteArray = m_replySignIn->readAll();
    _closeReplySignIn();

    byteArray = byteArray.replace("\\", "");
    QJsonDocument doc = QJsonDocument::fromJson(byteArray);
    QJsonObject obj = doc.object();    

    GlobalData* pGlobal = GlobalData::getInstance();
    if(m_bValidCurrentReq ==false)
    {
#ifdef LOG_FILE_DEBUG
        pGlobal->log("receiveSignInReply not valid current Reg");
#endif
        m_bBusy = false;
        return;
    }
    if(m_currentReq._type != req_sign_in_frist && m_currentReq._type != req_sign_in_returning)
    {
#ifdef LOG_FILE_DEBUG
        pGlobal->log("receiveSignInReply invalid current Reg");
#endif
        m_bBusy = false;
        return;
    }

    RequestSignIn* reqSignIn = (RequestSignIn*)m_currentReq._data;
    if(obj.value("status").toInt() == 200)
    {
        if(obj.value("result").toBool())
        {
            QJsonObject subObj = obj.value("data").toObject();
            int sessionId = subObj.value("session_id").toInt();
            int visitorId = subObj.value("visitor_id").toInt();
            reqSignIn->m_person.m_nVisitorId = visitorId;
            DBManagement* db = DBManagement::getInstance();

            db->lockUserTB();
            db->UpdatePerson(reqSignIn->m_person);
            db->unlockUserTB();

            db->lockHistoryTB();
            db->UpdateCheck(reqSignIn->m_person.m_nId, sessionId, reqSignIn->m_nStaffId);
            db->unlockHistoryTB();

            //if(m_currentReq._type == req_sign_in_frist)
            {
                //should check queue if there is signout or orher request for same user.
                _checkQueueForFirstUser(reqSignIn->m_person.m_nId, reqSignIn->m_person.m_nVisitorId, sessionId);
            }
/*
            QTime time = QTime::currentTime();
            int ntime = time.hour() * 60 + time.minute();
            db->InsertCheck(reqSignIn->m_person.m_nId, ntime, reqSignIn->m_nStaffId, g_sessionId);
*/
            pGlobal->log("Check in: Succed to check in! " +  reqSignIn->m_person.m_strFirst + " " + reqSignIn->m_person.m_strLast);
        }
        else
        {
            pGlobal->log("Check in: Unable to check in! " + reqSignIn->m_person.m_strFirst + " " + reqSignIn->m_person.m_strLast);
        }

        delete reqSignIn;
        m_bValidCurrentReq = false;
    }
    else
    {
        GlobalData::getInstance()->log("Check in: Unable to connect to server!");
    }

    m_bBusy = false;
}


void TransactionMgr::receiveSignOutReply()
{
    QByteArray byteArray = m_replySignOut->readAll();
    _closeReplySignOut();

    byteArray = byteArray.replace("\\", "");
    QJsonDocument doc = QJsonDocument::fromJson(byteArray);
    QJsonObject obj = doc.object();

    GlobalData* pGlobal = GlobalData::getInstance();
    if(m_bValidCurrentReq ==false)
    {
#ifdef LOG_FILE_DEBUG
        pGlobal->log("receiveSignOutReply not valid current Reg");
#endif
        m_bBusy = false;
        return;
    }
    if(m_currentReq._type != req_sign_out)
    {
#ifdef LOG_FILE_DEBUG
        pGlobal->log("receiveSignOutReply invalid current Reg");
#endif
        m_bValidCurrentReq =false;
        m_bBusy = false;
        return;
    }

    RequestSignOut* reqSignOut = (RequestSignOut*)m_currentReq._data;

    if(obj.value("status").toInt() == 200)
    {
        if(obj.value("result").toBool())
        {
            pGlobal->log("Check-out: Succed to check-out!");

        }else{
            pGlobal->log("Check-out: Failed to check-out!");
        }

        delete reqSignOut;
        m_bValidCurrentReq = false;
    }
    else
    {
        pGlobal->log("Check-out: Unable to connect to server!");
    }    
    m_bBusy = false;
}

void TransactionMgr::onSendCheckedOutListReq()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    QUrlQuery params;

    params.addQueryItem("client_id", QString::number(pGlobal->m_client_id));
    params.addQueryItem("location_id", QString::number(g_nlocId));
    params.addQueryItem("period_day", "1");

    QString strUrl = pGlobal->getChekedOutInfoUrl();
    QUrl url(strUrl);

    url.setQuery(params.query());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    _closeReplyCheckedOutList();

    m_replyChekedOutList = m_netMgr->get(request);

    connect(m_replyChekedOutList, SIGNAL(finished()), this, SLOT(receiveCheckedOutListReply()));

    pGlobal->log("CheckedOutList-send");
}

void TransactionMgr::receiveCheckedOutListReply()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    QByteArray byteArray = m_replyChekedOutList->readAll();
    _closeReplyCheckedOutList();

    byteArray = byteArray.replace("\\", "");
    QJsonDocument doc = QJsonDocument::fromJson(byteArray);
    QJsonObject obj = doc.object();

    if(obj.value("status").toInt() == 200)
    {
        if(obj.value("result").toBool())
        {
            pGlobal->log("(splash form): checkedOutList-succed");
            QJsonArray entryArry = obj.value("data").toArray();

            DBManagement* db = DBManagement::getInstance();

            db->lockHistoryTB();
            for(int i=0; i<entryArry.size(); i++)
            {
                QJsonObject entry = entryArry[i].toObject();
                int seesionId = entry.value("session_id").toInt();
                //pGlobal->log("(splash form): removing check status from db sessionId=" + QString::number(seesionId));
                db->DeleteCheckBySessionID(seesionId);
            }
            db->unlockHistoryTB();

        }
        else
        {
            pGlobal->log("(splash form): checkedOutList-failed");
        }
    }
    else
    {
        pGlobal->log("(splash form): checkedOutList-unable connect to server");
    }

    m_bBusy = false;
}


void TransactionMgr::run()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    pGlobal->log("TransactionMgr::run");
    m_bBusy = false;
    m_bValidCurrentReq = false;

    QTime idleTimer, busyTimer;

    //first time call checkedoutlist
    m_bBusy = true;
    emit sendRequestCheckedOutList();
    idleTimer.start();
    busyTimer.start();

    while(true)
    {
        if(m_bBusy)
        {
            if(busyTimer.elapsed() > 30 * 1000)
            {
                m_bBusy = false;
            }
            this->msleep(100);
            continue;
        }

        if(m_bValidCurrentReq==true)
            this->msleep(3000);


        if(m_bValidCurrentReq ==false)
            m_bValidCurrentReq = _getRequest(&m_currentReq);

        if(m_bValidCurrentReq==false)
        {
            if(idleTimer.elapsed() >= 7200 * 1000) //per 2 hours.
            {
                m_bBusy = true;
                emit sendRequestCheckedOutList();
                idleTimer.restart();
                busyTimer.restart();
            }
            this->msleep(500);
        }
        else
        {
            m_bBusy = true;
            busyTimer.restart();
            emit sendRequest();
        }

    }
}
