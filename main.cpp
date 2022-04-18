#include "mainwindow.h"
#include <QApplication>
#include "loginwindow.h"
#include "faceDetect.h"
#include "FaceApi.h"
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QFontDatabase>
#include <QSslSocket>

#include "global.h"
#include "splashform.h"

#include "floordlg.h"
#include "privacydlg.h"


QString loadTermConditionString(QString strPath)
{
    QFile file(strPath);

    QString line = "";
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){

            line.append(stream.readLine()+"\n");
        }
        file.close();
    }
    return line;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.addLibraryPath("./plugins");

/*
    FloorDlg dlg12;
    dlg12.exec();

    PrivacyDlg dlg32;
    dlg32.exec();
    return 1;
*/

    QFontDatabase::removeAllApplicationFonts();
    QFontDatabase::addApplicationFont(":/fonts/AcerFoco_Rg.ttf");

    QFont font("AcerFoco Rg", 8);
    //font.setStyleHint(QFont::Monospace);
    QApplication::setFont(font);

    QSslSocket::supportsSsl();


    GlobalData* pGlobal = GlobalData::getInstance();
    pGlobal->m_pApp = &a;
    pGlobal->m_strCurrentDir = qApp->applicationDirPath() + "/";


    //create log file
    QFile* pLogFile = new QFile(pGlobal->m_strCurrentDir + "log.txt");
    if (pLogFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        pLogFile->seek(pLogFile->size());
        pGlobal->m_logStream = new QTextStream(pLogFile);
    }
    pGlobal->log("Current dir: '" + pGlobal->m_strCurrentDir + "'");

    //create DBs.
    DBManagement* dbManager = DBManagement::getInstance();
    dbManager->CreateDatabase();


    //create image folder;
    g_imagepath = qApp->applicationDirPath() + "/image";
    pGlobal->log("Image path: '" + g_imagepath + "'");


    QDir qDir;
    if (! qDir.exists(g_imagepath))
        qDir.mkpath(g_imagepath);

    QSettings setting(pGlobal->m_strCurrentDir + "setting.ini", QSettings::IniFormat);
    int nLocId = setting.value("location_id", "-1").toInt();
    QString strLocKey = setting.value("location_key").toString();
    QString strLocName = setting.value("location_name").toString();

    pGlobal->m_strBaseUrl = setting.value("base_url").toString();
    pGlobal->log("Base Url: " + pGlobal->m_strBaseUrl);

    pGlobal->m_strPrinter = setting.value("printer_name").toString();
    pGlobal->log("Printer name: " + pGlobal->m_strPrinter);


    //write current version
    setting.setValue("app_ver", QString(APP_VER));
    setting.sync();


    //load app logo image.
    QString logoFileName(pGlobal->m_strCurrentDir+ "appLogo.png");
    if(QFileInfo::exists(logoFileName))
        pGlobal->m_logoImage.load(logoFileName);

    QString evacImgFileName(pGlobal->m_strCurrentDir+ "evacImage.png");
    if(QFileInfo::exists(evacImgFileName))
        pGlobal->m_evacImage.load(evacImgFileName);

    pGlobal->m_strTerms = loadTermConditionString(pGlobal->m_strCurrentDir+ "terms.txt");

    g_strLocationKey = strLocKey;
    Loginwindow* logDlg = new Loginwindow();
    pGlobal->m_pLoginFrm = logDlg;
    if(nLocId > 0)
    {
       logDlg->setLocationKey(strLocKey);       
    }

    pGlobal->m_pLoginFrm->show();
    //pGlobal->m_pLoginFrm->hide();

    pGlobal->m_pEvacuationFrm = new FloorDlg();
    pGlobal->m_pEvacuationFrm->hide();

    pGlobal->m_pLicenseFrm = new PrivacyDlg();
    pGlobal->m_pLicenseFrm->hide();
    //pGlobal->m_pLicenseFrm->show();

    pGlobal->m_pSplash = new SplashForm();
    pGlobal->m_pSplash->hide();
    //pGlobal->m_pSplash->show();

    pGlobal->m_pMain = new MainWindow();
    pGlobal->m_pMain->hide();
    //pGlobal->m_pMain->show();



    QString modelPath = qApp->applicationDirPath() + "/models/face.prototxt";
    QString weightPath = qApp->applicationDirPath() + "/models/face.dat";

    pGlobal->log("Initalize opencv modules");
    Detect_Init();
    F_Init((char*)(modelPath.toStdString().c_str()), (char*)(weightPath.toStdString().c_str()));

    //pGlobal->m_pSplash = new SplashForm();
    //pGlobal->m_pSplash->exec();
    //SplashForm* pSplash = (SplashForm*)pGlobal->m_pSplash;
    //pSplash->doWorkIdle();

    return a.exec();
}
