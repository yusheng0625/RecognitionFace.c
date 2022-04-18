#ifndef PERSONRESULT_HEADER
#define PERSONRESULT_HEADER

#include <QString>
#include <QImage>
#include <QNetworkAccessManager>
#include <QWidget>
#include <QDialog>
#include <QMainWindow>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QPixmap>
#include <QTextStream>
#include "dbmanagement.h"
#include <QApplication>
#include <QDateTime>
#include <QReadWriteLock>
#include "transactionmgr.h"

#define LOG_FILE
#define LOG_FILE_DEBUG

#define DEV_VERSION


#define APP_VER      "2.11"
#define APP_TITLE    "My Visitors"


extern QString g_imagepath;
extern float g_threshold;
extern float g_threshold2;
extern int g_camIdx;
extern int g_resIdx;
extern int g_printerDPI;
extern QString g_strLocationKey;
extern QString g_strClientName;

extern int g_nlocId;
extern QString g_locName;
extern QList<StaffInfo> g_staffList;
extern int g_nStaffId;


typedef enum app_mode{
    appmode_unknown,
    appmode_first_visitors,
    appmode_returning_visitors,
    appmode_signout,
}APP_MODE;


class PersonResult{
public:
	PersonResult();
    PersonResult(PersonResult* src);
    PersonResult(int nId, float flSim, QImage faceImg, float* pFeature, QString strFirstName, QString strLastName);

	~PersonResult();
    void copy(PersonResult* src);

public:
	int m_nId;
	float m_flSimilarity;
	QImage m_faceImg;
    QString m_strFirstName;
    QString m_strLastName;
    float m_pFeature[FEATURE_LEN];
};

class CarmeraManager
{
public:
    typedef enum _cptureStatus{
        capture_unknown =0,
        capture_checked_out,
        capture_matched,
        capture_misMatched,
    }cptureStatus;

public:
    CarmeraManager();
    ~CarmeraManager();

    cv::VideoCapture m_capture;
    bool m_bCameraOpen;
    QWidget* m_pCameraWidget;
    QLabel*  m_pCameraLabel;
    int  m_nTimer;
    int  m_nSkipped;
    bool m_bFeature;
    QList<PersonResult*> m_matchList;

    bool IsOpenCamera();
    void StartCamera(QWidget* pWidget);
    void StopCamera(QWidget* pWidget = NULL);
    cptureStatus Capture(QLabel* cameraWdiget, int* pID, int* pHistID, int* pSessionID, int* pStaffId, QImage* pImage,  PersonResult* pPerson,
                         bool bOnlyDrawing, float* pScore=NULL);

private:
    void drawGuidBox(QSize widgetSize,QSize imgSize, QPainter* painter);
    void classifyTop3Matches();
    void clearMachList();
};


class GlobalData
{
public:
    static GlobalData* g_instance;
    static GlobalData* getInstance();
    GlobalData();
    ~GlobalData();

    bool m_bValidData;
    void saveData(QImage faceImg, Person* person);
    QImage m_faceImg;
    Person m_person;
    void setValidData(bool bValid);
    QString m_strPrinter;

    float m_lastFeature[FEATURE_LEN];
public:
    CarmeraManager m_camMgr;
    TransactionMgr m_transactionMgr;

    QApplication* m_pApp;
    QDialog*     m_pSplash;
    QMainWindow* m_pMain;
    QDialog*     m_pLoginFrm;
    QDialog*     m_pEvacuationFrm;
    QDialog*     m_pLicenseFrm;

    QString      m_strAppVersion;
    QString      m_strBaseUrl;
    QString      m_strCurrentDir;
    QString      m_strAppName;

    int          m_client_id;
    APP_MODE     m_appMode;

    QList<PersonResult*> m_matchListTop3;
public:
    void closeall();
    bool isValidImageUserImage(QString strFile);

    QString      getVisitorType(int iVisitorType)
    {
        if(iVisitorType==0)
            return "VISITOR";
        return "CONTRACTOR";
    }
    int      getVisitorTypeId(QString     strVisitorType )
    {
        if(strVisitorType=="VISITOR")
            return 0;
        return 1;
    }


    QString      getStaffName(int staffId)
    {
        QString strName = "";
        int nCnt = g_staffList.count();
        for(int i = 0; i < nCnt; i++)
        {
            if(staffId == g_staffList.at(i).m_nStaffId)
            {
                strName =  g_staffList.at(i).m_strName;
                break;
            }
        }
        return strName;
    }

    QString      getAppInfoUrl()
    {
        return m_strBaseUrl + "/api/setting/app-info";
    }
    QString      getChekedOutInfoUrl()
    {
        return   m_strBaseUrl + "/api/visitor/manual-check-out-list" ;
    }

    QString      getLocationInfoUrl()
    {
        return   m_strBaseUrl + "/api/location-info" ;
    }

    QString      getStaffListUrl()
    {
        return   m_strBaseUrl + "/api/user/staff-list" ;
    }
    QString      getCheckInUrl()
    {
        return   m_strBaseUrl + "/api/visitor/check-in" ;
    }
    QString      getCheckOutUrl()
    {
        return   m_strBaseUrl + "/api/visitor/check-out" ;
    }

    QString      getSettingListUrl()
    {
        return   m_strBaseUrl + "/api/setting/list" ;
    }

public:

/*
    QFile file("result.txt");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    out << QString("victory");   // serialize a string
*/
    QReadWriteLock m_logLock;

    QTextStream* m_logStream;
    void log(QString strLog)
    {
        QDateTime curTime = QDateTime::currentDateTime();
        QString strTime = curTime.toString() + "=> ";
#ifdef LOG_FILE
        m_logLock.lockForWrite();
        if(m_logStream)
        {
            (*m_logStream) << strTime << strLog << "\n";
            m_logStream->flush();
        }
        m_logLock.unlock();
#endif
    }

    QPixmap m_logoImage;
    QPixmap m_evacImage;
    QString m_strTerms;
};

void EncryptImage(QImage img, QString path);
bool DecryptImage(QString path, QImage& outImg);

#endif
