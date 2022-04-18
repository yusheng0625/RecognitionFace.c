#include "global.h"
#include "faceDetect.h"
#include "FaceApi.h"
#include <QPainter>
#include <QMessageBox>

#define FACE_SIZE 0.15f
#define FACE_ANGLE 30
#define FACE_COUNT 5

using namespace cv;
using namespace std;



QString g_imagepath;
float g_threshold;
float g_threshold2;
int g_printerDPI = 300;

int g_camIdx = 0;
int g_resIdx = 0;

int g_nlocId = -1;
QString g_locName;
QString g_strLocationKey = "";
QString g_strClientName = "Acer";

QNetworkAccessManager* g_netManager = NULL;
QList<StaffInfo> g_staffList;
int g_nStaffId = 0;

PersonResult::PersonResult()
{
	m_nId = 0;
	m_flSimilarity = 0.0f;
	m_faceImg = QImage();
    memset(m_pFeature, 0, FEATURE_LEN*sizeof(float));
}

PersonResult::PersonResult(PersonResult* src)
{
    copy(src);
}

PersonResult::PersonResult(int nId, float flSim, QImage faceImg, float* pFeature, QString strFirstName, QString strLastName)
{
	m_nId = nId;
	m_flSimilarity = flSim;
	m_faceImg = faceImg.copy();
	memcpy(m_pFeature, pFeature, 256 * sizeof(float));
    m_strFirstName = strFirstName;
    m_strLastName = strLastName;
}

PersonResult::~PersonResult()
{

}
void PersonResult::copy(PersonResult* src)
{
    m_nId = src->m_nId;
    m_flSimilarity = src->m_flSimilarity;
    m_faceImg = src->m_faceImg.copy();
    memcpy(m_pFeature, src->m_pFeature, 256 * sizeof(float));
    m_strFirstName = src->m_strFirstName;
    m_strLastName = src->m_strLastName;
}


GlobalData* GlobalData::g_instance = NULL;
GlobalData* GlobalData::getInstance()
{
    if(g_instance ==NULL)
        g_instance = new GlobalData();
    return g_instance;
}

GlobalData::GlobalData()
{
    m_strAppVersion = APP_VER;
    m_bValidData = false;        
    m_logStream = NULL;
    m_client_id = 0;
    m_appMode = appmode_unknown;

    m_pApp =  NULL;
    m_pSplash = NULL;
    m_pMain = NULL;
    m_pLoginFrm = NULL;
    m_pEvacuationFrm = NULL;
    m_pLicenseFrm = NULL;

}

GlobalData::~GlobalData()
{

}
bool GlobalData::isValidImageUserImage(QString strFile)
{
    QString strAbsPath;
    strAbsPath = g_imagepath  + "/" + strFile;
    QFileInfo file(strAbsPath);
    if(file.exists() && file.isFile())
        return true;

    QFile::remove(strFile);
    return false;
}

void GlobalData::closeall()
{
    m_camMgr.StopCamera();

    if(m_pLoginFrm)
    {
        m_pLoginFrm->close();
        delete m_pLoginFrm;
    }


    if(m_pSplash)
    {
        m_pSplash->close();
        delete m_pSplash;
    }


    if(m_pEvacuationFrm)
    {
        m_pEvacuationFrm->close();
        delete m_pEvacuationFrm;
    }


    if(m_pLicenseFrm)
    {
        m_pLicenseFrm->close();
        delete m_pLicenseFrm;
    }

    if(m_pMain)
    {
        m_pMain->close();
        delete m_pMain;
    }
    m_transactionMgr.terminate();

    m_pApp->exit(0);
    return;
}

void GlobalData::saveData(QImage faceImg, Person* person)
{
    m_faceImg = faceImg.copy();
    m_person.Copy(*person);
    m_bValidData = true;
}

void GlobalData::setValidData(bool bValid)
{
    m_bValidData = bValid;
}


CarmeraManager::CarmeraManager()
{
    m_bCameraOpen = false;
    m_pCameraWidget = NULL;
    m_nTimer = -1;
    m_nSkipped = 0;
    m_bFeature = false;
    m_pCameraLabel = NULL;
}
CarmeraManager::~CarmeraManager()
{
}
bool CarmeraManager::IsOpenCamera()
{
    return m_bCameraOpen;
}
void CarmeraManager::StartCamera(QWidget* pWidget)
{
    if(m_capture.open(g_camIdx))
    {
        if(m_pCameraWidget != NULL)
            m_pCameraWidget->killTimer(m_nTimer);

        m_pCameraWidget = pWidget;
        m_nTimer = m_pCameraWidget->startTimer(30);
        m_bCameraOpen = true;
        m_bFeature = true;
        m_nSkipped = 0;
        clearMachList();
    }
}

void CarmeraManager::clearMachList()
{
    for(int i=0; i<m_matchList.size(); i++)
        delete m_matchList.at(i);
    m_matchList.clear();
}

void CarmeraManager::StopCamera(QWidget* pWidget)
{
    if(m_capture.isOpened())
    {
        if(pWidget != NULL)
            pWidget->killTimer(m_nTimer);
        else
        {
            if(m_pCameraWidget != NULL)
                m_pCameraWidget->killTimer(m_nTimer);
            m_pCameraWidget = NULL;
        }

        if(m_pCameraLabel !=NULL)
        {
            QPixmap pixmap = QPixmap(":/images/blank").scaled(m_pCameraLabel->size(), Qt::IgnoreAspectRatio);
            m_pCameraLabel->setPixmap(pixmap);
            m_pCameraLabel = NULL;
        }


        m_capture.release();
    }
    m_bCameraOpen = false;
}

void CarmeraManager::drawGuidBox(QSize widgetSize,QSize imgSize, QPainter* painter)
{

    int width = 320;
    int height = 300;
    QRect guidRect( (widgetSize.width() - width) / 2,
                    (widgetSize.height() - height) * 2/ 5,
                    width, height);

    int imWidth = widgetSize.width();
    int imHeight = widgetSize.height();

    QRect photoRect(guidRect.x(), guidRect.y(),width,height);

    QRect topRC(0, 0, imWidth , photoRect.top());
    QRect bottomRC(0, photoRect.bottom()+1 , imWidth , imHeight - photoRect.bottom()-1);


    QRect leftRC(0,                  photoRect.top(),  photoRect.left(), photoRect.height());
    QRect rightRC(photoRect.right(), photoRect.top() , photoRect.left(), photoRect.height());


    painter->fillRect(topRC, QBrush(QColor(30, 30, 30, 128)));
    painter->fillRect(bottomRC, QBrush(QColor(30, 30, 30, 128)));
    painter->fillRect(leftRC, QBrush(QColor(30, 30, 30, 128)));
    painter->fillRect(rightRC, QBrush(QColor(30, 30, 30, 128)));

    QPen qpen(Qt::white);
    qpen.setWidth(2);
    painter->setPen(qpen);
    painter->drawRect(photoRect);


/*
    QRect guidRect( (widgetSize.width() - width) / 2,
                    (widgetSize.height() - height) * 2/ 5,
                    width, height);
    float fRatio = widgetSize.width() * 1.0f / imgSize.width();

    int imWidth = widgetSize.width()  * fRatio;
    int imHeight = widgetSize.height() * fRatio;

    QRect photoRect(guidRect.x() * fRatio, guidRect.y() * fRatio,
                    width * fRatio,height * fRatio);

    QRect topRC(0, 0, imWidth , photoRect.top());
    QRect bottomRC(0, photoRect.bottom() , imWidth , imHeight - photoRect.bottom());


    QRect leftRC(0, photoRect.top(), photoRect.left(), photoRect.height());
    QRect rightRC(photoRect.right(), photoRect.top() , photoRect.left(), imHeight - photoRect.bottom());


    painter->fillRect(topRC, QBrush(QColor(30, 30, 30, 128)));
    painter->fillRect(bottomRC, QBrush(QColor(30, 30, 30, 128)));
    painter->fillRect(leftRC, QBrush(QColor(30, 30, 30, 128)));
    painter->fillRect(rightRC, QBrush(QColor(30, 30, 30, 128)));

    painter->drawRect(photoRect);
*/
}

void CarmeraManager::classifyTop3Matches()
{
    GlobalData* pGlobal = GlobalData::getInstance();

    for(int i=0; i<pGlobal->m_matchListTop3.size(); i++)
        delete pGlobal->m_matchListTop3.at(i);
    pGlobal->m_matchListTop3.clear();

    float maxScore = 0;
    int   maxIdx = 0;
    QList<PersonResult*> matchList;

    for(int i=0; i<m_matchList.size(); i++)
        matchList.push_back(m_matchList.at(i));

    while(matchList.size() > 0 && pGlobal->m_matchListTop3.size() < 3)
    {
        maxScore = 0;
        for(int i = 0; i < matchList.size(); i++)
        {
            if(maxScore < matchList.at(i)->m_flSimilarity)
            {
                maxIdx = i;
                maxScore = matchList.at(i)->m_flSimilarity;
            }
        }

        PersonResult* maxScoreResult = matchList.at(maxIdx);
        pGlobal->m_matchListTop3.push_back(new PersonResult(maxScoreResult));
        matchList.erase(matchList.begin() + maxIdx);
    }
    matchList.clear();
}


CarmeraManager::cptureStatus CarmeraManager::Capture(QLabel* cameraWdiget, int* pID, int* pHistID, int* pSessionID, int* pStaffId, QImage* pImage,  PersonResult* pPerson, bool bOnlyDrawing, float* pScore)
{
    m_pCameraLabel = cameraWdiget;

    QString strUserFirst, strUserLast;
    cptureStatus status = capture_unknown;
    Mat frame;

    if(!m_capture.isOpened())
        return status;
    if(!m_capture.read(frame))
        return status;
    if(frame.empty())
        return status;

    QImage image = QImage(frame.data, frame.cols, frame.rows, QImage::Format_RGB888).rgbSwapped();
    int faceCount = 0;
    SFaceRect* pFaceRect = NULL;
    Detect_Faces((unsigned char*)image.bits(), image.bytesPerLine(), image.width(), image.height(), &faceCount, &pFaceRect);

    QSize size = cameraWdiget->size();
    QImage drawImage = image.scaled(size, Qt::KeepAspectRatio);
    QPainter painter(&drawImage);
    QPen qpen(Qt::green);
    qpen.setWidth(2);
    painter.setPen(qpen);

    float pFeature[FEATURE_LEN];
    if(faceCount > 0)
    {
        int feature_len = GetFeaturelen();
        int nCenterIdx = 0;
        int nMinDist = 100000;
        for(int i = 0; i < faceCount; i++)
        {
            int nDistfromCenter = (image.width() / 2 - pFaceRect[i].X - pFaceRect[i].Width / 2) * (image.width() / 2 - pFaceRect[i].X - pFaceRect[i].Width / 2) + (image.height() / 2 - pFaceRect[i].Y - pFaceRect[i].Height / 2) * (image.height() / 2 - pFaceRect[i].Y - pFaceRect[i].Height / 2);
            if(nDistfromCenter < nMinDist)
            {
                nCenterIdx = i;
                nMinDist = nDistfromCenter;
            }
        }

        int faceRect[4];
        faceRect[0] = std::max(0, pFaceRect[nCenterIdx].X);
        faceRect[1] = std::min(pFaceRect[nCenterIdx].X + pFaceRect[nCenterIdx].Width, image.width());
        faceRect[2] = std::max(0, pFaceRect[nCenterIdx].Y);
        faceRect[3] = std::min(pFaceRect[nCenterIdx].Y + pFaceRect[nCenterIdx].Height, image.height());

        if(bOnlyDrawing==false)
        {
            m_nSkipped++;
            if(abs(pFaceRect[nCenterIdx].Yaw) < FACE_ANGLE && pFaceRect[nCenterIdx].Width > image.width() * FACE_SIZE)
            {
                if(m_bFeature && faceRect[0] > image.width() / 8 && faceRect[1] < image.width() * 7 / 8 && faceRect[2] > image.height() / 8 && faceRect[3] < image.height() * 7 / 8)
                {
                    QRect faceRegion = QRect(faceRect[0] - 20, faceRect[2] - 20, faceRect[1] - faceRect[0] + 40, faceRect[3] - faceRect[2] + 40);
                    QImage faceImg = image.copy(faceRegion);

                    GetFeatures((unsigned char*)image.bits(), image.width(), image.height(),faceRect, pFaceRect[nCenterIdx].landmarks, pFeature);

                    //get similarity from database
                    DBManagement* dbManager = DBManagement::getInstance();
                    float flSimilarity = 0.0f;

                    dbManager->lockUserTB();
                    int nId = dbManager->GetSimilarId(pFeature, feature_len, g_threshold, flSimilarity,strUserFirst, strUserLast);
                    dbManager->unlockUserTB();

                    if(nId > 0)
                    {
                        m_matchList.push_back(new PersonResult(nId, flSimilarity, faceImg, pFeature,strUserFirst, strUserLast));
                        m_nSkipped--;
                    }
                    else
                    {
                        m_matchList.push_back(new PersonResult(-1, flSimilarity, faceImg, pFeature,"", ""));
                    }
                }
            }
        }
        delete[] pFaceRect;
        float fRatio = size.width() * 1.0f / image.width();
        painter.drawRect(QRect(faceRect[0] * fRatio, faceRect[2] * fRatio, (faceRect[1] - faceRect[0]) * fRatio, (faceRect[3] - faceRect[2]) * fRatio));
    }

    if(bOnlyDrawing==false)
        drawGuidBox(size, image.size(), &painter);

    cameraWdiget->setPixmap(QPixmap::fromImage(drawImage));
    if(bOnlyDrawing==true)
        return status;


    if(m_matchList.size() < FACE_COUNT)
        return status;

    //choose Top3 Matches
    classifyTop3Matches();

    int maxIdx = FACE_COUNT - 1;
    float maxScore = 0;
    for(int i = 0; i < m_matchList.size(); i++)
    {
        if(maxScore < m_matchList.at(i)->m_flSimilarity)
        {
            maxIdx = i;
            maxScore = m_matchList.at(i)->m_flSimilarity;
        }
    }


    if(pScore != NULL)
        *pScore = maxScore;

    //set threshold follwing appmode.
    float threshold = g_threshold2;
    if(GlobalData::getInstance()->m_appMode == appmode_first_visitors)
        threshold = g_threshold;



    if(maxScore >= threshold)//success
    {
        QString strMatchedScore = "Recognize Matched score: " + QString::number(maxScore);
        strMatchedScore +=" id=" + QString::number(m_matchList.at(maxIdx)->m_nId) +
                ", name=" + m_matchList.at(maxIdx)->m_strFirstName + " " + m_matchList.at(maxIdx)->m_strLastName;
        GlobalData::getInstance()->log(strMatchedScore);

        DBManagement* dbManager = DBManagement::getInstance();
        int nHistId = 0; int nSessionId; int staffId = -1;

        dbManager->lockUserTB();
        bool bExist = dbManager->IsExist(m_matchList.at(maxIdx)->m_nId, nHistId, nSessionId, staffId);
        dbManager->unlockUserTB();

        if(bExist)//checkout
        {
            status = capture_checked_out;
            *pID =m_matchList.at(maxIdx)->m_nId;
            *pHistID = nHistId;
            *pSessionID = nSessionId;
            *pStaffId = staffId;
            *pImage =m_matchList.at(maxIdx)->m_faceImg.copy();

            //ui->stackedWidget->setCurrentWidget(m_checkoutWidget);
            //m_checkoutWidget->SetData(m_matchList.at(maxIdx).m_nId, nHistId, m_matchList.at(maxIdx).m_faceImg, nSessionId);
        }else{//checkin
            status = capture_matched;
            *pID =m_matchList.at(maxIdx)->m_nId;
            *pImage =m_matchList.at(maxIdx)->m_faceImg.copy();
            *pStaffId = -1;
            //ui->stackedWidget->setCurrentWidget(m_matchWidget);
            //m_matchWidget->SetData(m_matchList.at(maxIdx).m_nId, m_matchList.at(maxIdx).m_faceImg);
        }
        pPerson->copy((PersonResult*)m_matchList.at(maxIdx));

    }else{//failure

        QString strMatchedScore = "Recognize misMatched score: " + QString::number(maxScore);
        strMatchedScore +=" id=" + QString::number(m_matchList.at(maxIdx)->m_nId) +
                ", name=" + m_matchList.at(maxIdx)->m_strFirstName + " " + m_matchList.at(maxIdx)->m_strLastName;
        GlobalData::getInstance()->log(strMatchedScore);

        status = capture_misMatched;
        pPerson->copy((PersonResult*)m_matchList.at(maxIdx));

        //ui->stackedWidget->setCurrentWidget(m_mismatchWidget);
        //m_mismatchWidget->SetFaceImage(m_matchList.at(maxIdx));
    }

    clearMachList();
    m_bFeature = false;

    if(m_nSkipped > 10)
    {
        m_nSkipped = 0;
    }
   return status;
}



const int MAX=1e4+79;
void EncryptImage(QImage img, QString path)
{
    img = img.convertToFormat(QImage::Format_RGB888);
    Mat mat(img.width(), img.height(), CV_8UC3, img.bits());

    int i, l;
    double u = 3.94;
    vector<pair<double, int >> x;
    Vec3b pixel;

    Mat image = mat.clone();

    x.push_back(std::make_pair(0.4, 0));

    double temp;
    for (int i = 1; i < image.cols; ++i){
        temp = u*x[i - 1].first*(1 - x[i - 1].first);
        x.push_back(std::make_pair(temp, i));
    }
    std::sort(x.begin(), x.end());

    i = 0;
    for (int r = 0; r < image.rows; ++r) {
        for (int c = 0; c < image.cols; ++c) {
            if (i>image.cols - 1)
                i = 0;
            int temps = x[i].second;

            pixel = image.at<Vec3b>(r, temps);
            image.at<Vec3b>(r, temps) = image.at<Vec3b>(r, c);
            image.at<Vec3b>(r, c) = pixel;

            i++;
        }
    }

    for (int r = 0; r < image.rows; ++r) {
        for (int c = 0; c < image.cols; ++c) {
            if (i>100){
                i = 1;
            }
            l = x[i].first*MAX;
            l = l % 255;
            image.at<Vec3b>(r, c)[0] = image.at<Vec3b>(r, c)[0] ^ l;
            image.at<Vec3b>(r, c)[1] = image.at<Vec3b>(r, c)[1] ^ l;
            image.at<Vec3b>(r, c)[2] = image.at<Vec3b>(r, c)[2] ^ l;
            i++;
        }
    }

    cv::FileStorage fs(path.toLatin1().data(), cv::FileStorage::WRITE);
    fs << "mymat"<<image;
}

bool checkMatFile(QString path)
{
    QFile file(path);
    if(file.open(QIODevice::ReadOnly)==false)
        return false;
    if(file.size() < 0x10)
    {
        file.close();
        return false;
    }

    char buff[0x11];
    if(file.read(buff, 0x10) != 0x10)
    {
        file.close();
        return false;
    }

    file.close();

    buff[0x10] = 0;
    if(strstr(buff + 0x0B, "mymat"))
        return true;
    return false;
}


bool DecryptImage(QString path, QImage& outImg)
{

    if(checkMatFile(path)==false)
        return false;

    Mat img;
    cv::FileStorage fs(path.toLatin1().data(), cv::FileStorage::READ);

    if(fs.isOpened()==false)
        return false;

    fs["mymat"]>>img;
    qDebug()<<img.cols;

    fs.release();

    int i, l;
    double u = 3.94;
    vector<pair<double, int >> x;
    Vec3b pixel;

    Mat image = img.clone();

    x.push_back(std::make_pair(0.4, 0));

    double temp;
    for (int i = 1; i < image.cols; ++i){
        temp = u*x[i - 1].first*(1 - x[i - 1].first);
        x.push_back(std::make_pair(temp, i));
    }
    std::sort(x.begin(), x.end());

    i = 1;
    for (int r = 0; r < image.rows; ++r) {
        for (int c = 0; c < image.cols; ++c) {
            if (i>100){
                i = 1;
            }
            l = x[i].first*MAX;
            l = l % 255;
            image.at<Vec3b>(r, c)[0] = image.at<Vec3b>(r, c)[0] ^ l;
            image.at<Vec3b>(r, c)[1] = image.at<Vec3b>(r, c)[1] ^ l;
            image.at<Vec3b>(r, c)[2] = image.at<Vec3b>(r, c)[2] ^ l;
            i++;
        }
    }

    i = image.cols - 1;
    for (int r = image.rows - 1; r >= 0; --r) {
        for (int c = image.cols - 1; c >= 0; --c) {
            if (i < 0)
                i = image.cols - 1;
            int temps = x[i].second;

            pixel = image.at<Vec3b>(r, temps);
            image.at<Vec3b>(r, temps) = image.at<Vec3b>(r, c);
            image.at<Vec3b>(r, c) = pixel;

            i--;
        }
    }

    QImage qimg = QImage(image.data, image.cols, image.rows, QImage::Format_RGB888);
    outImg = qimg.copy();
    return true;
}
