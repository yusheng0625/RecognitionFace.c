#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QSettings>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <QThread>
#include "global.h"
#include "splashform.h"



Loginwindow::Loginwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Loginwindow)
{
    m_bLoging = false;
    ui->setupUi(this);
    //set full screen
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint);
    window()->showMaximized();
    window()->setGeometry(QApplication::desktop()->availableGeometry());

    //add background image
    QPixmap pixmap = QPixmap(":/images/login").scaled(size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, pixmap);
    this->setPalette(palette);

    m_netManager = new QNetworkAccessManager(this);
    m_reply = NULL;

}

Loginwindow::~Loginwindow()
{
    delete ui;
    delete m_netManager;
}

void Loginwindow::locInfoRequestFinished()
{    

    GlobalData* pGlobal = GlobalData::getInstance();
    QByteArray byteArray = m_reply->readAll();

    byteArray = byteArray.replace("\\", "");

//    QMessageBox::about(NULL, "aaa", byteArray.toStdString().c_str());


    QJsonDocument doc = QJsonDocument::fromJson(byteArray);
    QJsonObject obj = doc.object();

    bool bSuccess = false;

    if(obj.value("status").toInt() == 200)
    {
        if(obj.value("result").toBool())
        {
            QJsonObject subObj = obj.value("data").toObject();
            g_nlocId = subObj.value("location_id").toInt();
            g_locName = subObj.value("location_name").toString();

            QSettings setting("setting.ini", QSettings::IniFormat);
            setting.setValue("location_id", g_nlocId);
            setting.setValue("location_name", g_locName);
            g_strLocationKey = ui->lnLocIdKey->text().trimmed();
            setting.setValue("location_key", g_strLocationKey);

            pGlobal->m_client_id = subObj.value("client_id").toInt();
            pGlobal->log("clinet_id=" + QString::number(pGlobal->m_client_id));

            bSuccess = true;
            //create splash and show it.
        }else{
            QMessageBox::warning(this, "Warning!", "The identification key is not correct!");
        }
    }
    else
    {
        QMessageBox::warning(this, "Warning!", "Unable to connect to server!");
    }


    m_reply->close();

    if(bSuccess==false)
    {
        pGlobal->closeall();
    }
    else
        staffListRequestSend();

}


void Loginwindow::on_btn_check_clicked()
{
    if(m_bLoging)return;

    QString locKey = ui->lnLocIdKey->text().trimmed();
    if(locKey.isEmpty())
    {
        QMessageBox::warning(this, "Warning!", "Please enter location identification key!");
        return;
    }

    QUrlQuery params;
    params.addQueryItem("location_identification_key", locKey);
    QString strUrl = GlobalData::getInstance()->getLocationInfoUrl();
    QUrl url(strUrl);
    url.setQuery(params.query());

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    m_bLoging = true;
    m_reply = m_netManager->get(request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(locInfoRequestFinished()));

}

void Loginwindow::setLocationKey(QString strLocationKey)
{
    ui->lnLocIdKey->setText(strLocationKey);
}


void Loginwindow::staffListRequestSend()
{
    GlobalData::getInstance()->log("getting staff list");

    g_staffList.clear();
    QUrlQuery params;
    params.addQueryItem("location_id", QString::number(g_nlocId));
    params.addQueryItem("client_id", QString::number(GlobalData::getInstance()->m_client_id));


    QUrl url(QString(GlobalData::getInstance()->getStaffListUrl()));
    url.setQuery(params.query());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    m_reply = m_netManager->get(request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(staffListRequestFinished()));
}

void Loginwindow::staffListRequestFinished()
{
    QByteArray byteArray = m_reply->readAll();
    byteArray = byteArray.replace("\\", "");

    QJsonDocument doc = QJsonDocument::fromJson(byteArray);
    QJsonObject obj = doc.object();
    DBManagement* db = DBManagement::getInstance();

    bool bSuccess = false;
    if(obj.value("status").toInt() == 200)
    {
        if(obj.value("result").toBool())
        {
            QJsonArray ary = obj.value("data").toArray();
            for(int i = 0; i < ary.count(); i++)
            {
                QJsonObject staffObj = ary.at(i).toObject();
                int staffId = staffObj.value("staff_id").toInt();
                QString staffName = staffObj.value("staff_name").toString();
                QString phoneNumber = staffObj.value("phone_number").toString();
                QString strEmail = staffObj.value("email").toString();
                QString lastUpdated = staffObj.value("last_updated").toString();
                db->InsertStaff(StaffInfo(0, staffId, staffName, phoneNumber, strEmail, lastUpdated));
            }
            bSuccess = true;

        }else{
            QMessageBox::warning(this, "Warning!", "Unable to load staff list!");
        }
    }
    else
    {
        QMessageBox::warning(this, "Staff list getting is failed!", "Unable to connect to server!");
    }
    db->GetStaffInfo();

    GlobalData* pGlobal = GlobalData::getInstance();
    if(bSuccess)
    {
        pGlobal->m_pSplash->show();
        pGlobal->m_pSplash->activateWindow();
        pGlobal->m_transactionMgr.start();
        close();
    }
    else
    {
        pGlobal->closeall();
    }
    m_reply->close();
    m_reply = NULL;
}
