#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QPixmap>
#include <QPalette>
#include <QApplication>
#include <QDesktopWidget>
#include <QFontDatabase>
#include "dbmanagement.h"
#include <QDate>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include "global.h"
#include "floordlg.h"
#include "privacydlg.h"
#include "splashform.h"
#include "frmsecurity.h"

QString month[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GlobalData* pGlobal = GlobalData::getInstance();


    //settting logo image
    /*
    if(pGlobal->m_logoImage.isNull()==false)
    {
        QString strlogo = GlobalData::getInstance()->m_strCurrentDir + "appLogo.png";
        QString strStyleSheet = "border-image: url(\"" + strlogo+"\"); background:transparent;";
        strStyleSheet = strStyleSheet.replace("\\", "/");
        ui->label_3->setStyleSheet(strStyleSheet);
    }
     */

    //version
    ui->label_version->setText("v" + GlobalData::getInstance()->m_strAppVersion);


    installEventFilter(this);

    m_nIdleCount = 0;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_Time()));
    timer->start(1000);



    //set full screen
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint);
    window()->showMaximized();
    window()->setGeometry(QApplication::desktop()->availableGeometry());

    //add style for detection&settings
    ui->btnDetect->setStyleSheet(QString("\
    QPushButton {\
        color: rgb(255, 255, 255);\
        border: 0px solid black;\
    } \
    QPushButton:checked {\
        color: rgb(255, 255, 255);\
        border-bottom: 3px solid rgb(255, 255, 255);\
    }"));
    ui->btnSetting->setStyleSheet(QString("\
    QPushButton {\
     color: rgb(255, 255, 255);\
     border: 0px solid black;\
    } \
    QPushButton:checked {\
     color: rgb(255, 255, 255);\
     border-bottom: 3px solid rgb(255, 255, 255);\
    }"));

    m_pThankWidget = new ThankuDialog(ui->mainStack);
    m_pSettingWidget = new settingsdialog(ui->mainStack);
    m_pSettingWidget->ReadSettings();
    ui->label_3->setText(g_strClientName);

    m_pCheckOut = new checkoutWidget(ui->mainStack);


    m_pSignIn = new SignInCamera(ui->mainStack);
    m_pMatchWidget = new MatchWidget(ui->mainStack);
    m_pMismatchWidget = new MismatchWidget(ui->mainStack);
    m_pMatchWidgetTop3 = new matchTop3Widget(ui->mainStack);
    m_pUserInfo = new UserInfo(ui->mainStack);
    m_pThankWidget1 = new ThankyouWidget(ui->mainStack);



    ui->mainStack->addWidget(m_pThankWidget);
    ui->mainStack->addWidget(m_pThankWidget1);

    ui->mainStack->addWidget(m_pSettingWidget);
    ui->mainStack->addWidget(m_pSignIn);
    ui->mainStack->addWidget(m_pMatchWidget);
    ui->mainStack->addWidget(m_pMatchWidgetTop3);
    ui->mainStack->addWidget(m_pMismatchWidget);
    ui->mainStack->addWidget(m_pUserInfo);
    ui->mainStack->addWidget(m_pCheckOut);


    connect(m_pSignIn, SIGNAL(captureProcessed(int, QImage*,int, int, int, int, PersonResult*, float)), this, SLOT(on_signInProcessed(int, QImage*, int, int, int, int, PersonResult*, float)));
    connect(m_pMatchWidget, SIGNAL(clickedContinue(int,QImage,int)), this, SLOT(on_clickedContinuefMatch(int,QImage, int)));
    connect(m_pMatchWidget, SIGNAL(clickedContinueSecondaryScore(int,QImage,int)), this, SLOT(on_clickedContinueToTop3fMatch(int,QImage,int)));
    connect(m_pMatchWidget, SIGNAL(clickedGoBack()), this, SLOT(on_switchSplash()));

    connect(m_pMatchWidgetTop3, SIGNAL(clickedContinue(int,QImage, int)), this, SLOT(on_clickedContinuefMatchTop3(int,QImage, int)));
    connect(m_pMatchWidgetTop3, SIGNAL(clickedGoBack()), this, SLOT(on_switchSplash()));


    connect(m_pUserInfo, SIGNAL(clickedContinuefUserInfo(int, QImage,QString,QString)), this, SLOT(on_clickedContinuefUserInfo(int,QImage,QString,QString)));
    connect(m_pUserInfo, SIGNAL(clickedBackfUserInfo()), this, SLOT(on_clickedBackfUserInfo()));


    connect(pGlobal->m_pEvacuationFrm , SIGNAL(clickedNext()), this, SLOT(on_clickedNextEvacuationForm()));
    connect(pGlobal->m_pEvacuationFrm , SIGNAL(timedOut()), this, SLOT(on_timedOutEvacuationForm()));

    connect(pGlobal->m_pLicenseFrm , SIGNAL(clickedNext()), this, SLOT(on_clickedNextPrivacyForm()));
    connect(pGlobal->m_pLicenseFrm , SIGNAL(timedOut()), this, SLOT(on_timedOutPrivacyForm()));


    connect(m_pThankWidget, SIGNAL(callGotoHome()), this, SLOT(on_switchSplash()));
    connect(m_pThankWidget1, SIGNAL(callGotoHome()), this, SLOT(on_switchSplash()));

//    connect(m_pThankWidget1, SIGNAL(callGotoHome()), this, SLOT(on_callGotoHomefThank()));
    connect(m_pMismatchWidget, SIGNAL(clickedContinuefMismatch(int, QImage,QString,QString)), this, SLOT(on_clickedContinuefMismatch(int, QImage,QString,QString)));
    connect(m_pMismatchWidget, SIGNAL(clickedTryagain()), this, SLOT(on_callGotoHomefThank()));

    connect(m_pCheckOut, SIGNAL(clickedNotYet()), this, SLOT(on_switchSplash()));
    connect(m_pCheckOut, SIGNAL(clickedYesfCheckout(QString ,QImage,QString,QString)), this, SLOT(on_clickedYesfCheckout(QString, QImage,QString,QString)));



    //centerForm bkg
    QPixmap pixmap = QPixmap(":/images/login").scaled(size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, pixmap);
    this->setPalette(palette);

    ui->btnDetect->setChecked(true);
//    ui->btnSetting->setChecked(true);

    QDate date = QDate::currentDate();
    QString strDate = tr("%1th %2 %3").arg(date.day()).arg(month[date.month()-1]).arg(date.year());
    if(date.day() == 1 || date.day() == 21)
        strDate = tr("%1st %2 %3").arg(date.day()).arg(month[date.month()-1]).arg(date.year());
    else if(date.day() == 2 || date.day() == 22)
        strDate = tr("%1nd %2 %3").arg(date.day()).arg(month[date.month()-1]).arg(date.year());
    else if(date.day() == 3 || date.day() == 23)
        strDate = tr("%1rd %2 %3").arg(date.day()).arg(month[date.month()-1]).arg(date.year());

    ui->lblDate->setText(strDate);
    ui->lblLocName->setText(g_locName);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_clickedNextEvacuationForm()
{
    m_nIdleCount = 0;

    GlobalData* pGlobal = GlobalData::getInstance();
    pGlobal->m_pLicenseFrm->show();
    pGlobal->m_pLicenseFrm->activateWindow();
    pGlobal->m_pEvacuationFrm->hide();
}
void MainWindow::on_timedOutEvacuationForm()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    on_switchSplash();
    pGlobal->m_pEvacuationFrm->hide();
}

void MainWindow::on_clickedNextPrivacyForm()
{
    m_nIdleCount = 0;
    show();
    activateWindow();

    GlobalData* pGlobal = GlobalData::getInstance();
    m_pThankWidget->setVisitorInfo(m_signedUser.m_faceImg,
        &pGlobal->m_person, pGlobal->getVisitorType(m_signedUser.m_nId));

    ui->mainStack->setCurrentWidget(m_pThankWidget);
    m_pThankWidget->ResetTimer();
    m_pThankWidget->print();
    pGlobal->m_pLicenseFrm->hide();
}

void MainWindow::on_timedOutPrivacyForm()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    on_switchSplash();
    pGlobal->m_pLicenseFrm->hide();
}



void MainWindow::on_btnDetect_toggled(bool checked)
{
    checkTabBtn(checked);
}

void MainWindow::on_btnSetting_toggled(bool checked)
{
    if(checked ==false) return;
    frmSecurity dlg;
    dlg.setModal(true);
    dlg.setWindowFlags(Qt::FramelessWindowHint);

    if ( dlg.exec() == QDialog::Accepted )
    {
        checkTabBtn(!checked);
    }
    else
    {
        checkTabBtn(true);
    }
}
void MainWindow::on_btnHome_clicked()
{
    on_switchSplash();
}

void MainWindow::checkTabBtn(bool bDetection)
{
    m_nIdleCount = 0;
    if(bDetection)
    {
        ui->btnDetect->setEnabled(false);
        ui->btnSetting->setEnabled(true);
        ui->btnSetting->setChecked(false);

        if(GlobalData::getInstance()->m_camMgr.IsOpenCamera())
            GlobalData::getInstance()->m_camMgr.StopCamera(NULL);
        ui->mainStack->setCurrentWidget(m_pSignIn);
        GlobalData::getInstance()->m_camMgr.StartCamera(m_pSignIn);
        //ui->mainStack->setCurrentWidget(m_pUserInfo);
    }else{
        ui->btnDetect->setEnabled(true);
        ui->btnDetect->setChecked(false);
        ui->btnSetting->setEnabled(false);

        if(GlobalData::getInstance()->m_camMgr.IsOpenCamera())
            GlobalData::getInstance()->m_camMgr.StopCamera(NULL);
        ui->mainStack->setCurrentWidget(m_pSettingWidget);
        m_pSettingWidget->ReadSettings();
    }
}

void MainWindow::on_clickedContinueToTop3fMatch(int nPersonId, QImage faceImg, int nStaffId)
{
    //QMessageBox::about(NULL, QString::number(nStaffId), "on_clickedContinueToTop3fMatch");

    m_nIdleCount = 0;
    GlobalData::getInstance()->m_camMgr.StopCamera(m_pMatchWidget);
    ui->mainStack->setCurrentWidget(m_pMatchWidgetTop3);
    m_pMatchWidgetTop3->SetData(nPersonId, faceImg, nStaffId);
    GlobalData::getInstance()->m_camMgr.StartCamera(m_pMatchWidgetTop3);
}

void MainWindow::on_clickedContinuefMatchTop3(int nPersonId, QImage faceImg, int staffId)
{
    //QMessageBox::about(NULL, QString::number(staffId), "on_clickedContinuefMatchTop3");

    m_nIdleCount = 0;
    GlobalData::getInstance()->m_camMgr.StopCamera(m_pMatchWidgetTop3);
    m_pUserInfo->SetData(nPersonId, faceImg, staffId);
    ui->mainStack->setCurrentWidget(m_pUserInfo);
}

void MainWindow::on_clickedContinuefMatch(int nPersonId, QImage faceImg, int staffId)
{
    //QMessageBox::about(NULL, QString::number(staffId), "on_clickedContinuefMatch");

    m_nIdleCount = 0;
    GlobalData::getInstance()->m_camMgr.StopCamera(m_pMatchWidget);
    m_pUserInfo->SetData(nPersonId, faceImg, staffId);
    ui->mainStack->setCurrentWidget(m_pUserInfo);
}


void MainWindow::on_clickedContinuefMismatch(int iVisitor,QImage faceImg, QString firstName, QString lastName)
{
    m_nIdleCount = 0;

    GlobalData* pGlobal = GlobalData::getInstance();
    m_signedUser.m_nId = iVisitor;
    m_signedUser.m_faceImg = faceImg;
    m_signedUser.m_strFirstName = firstName;
    m_signedUser.m_strLastName = lastName;
    pGlobal->m_pEvacuationFrm->show();
    pGlobal->m_pEvacuationFrm->activateWindow();
    hide();
}

void MainWindow::on_clickedContinuefUserInfo(int iVisitor, QImage faceImg, QString firstName, QString lastName)
{
    m_nIdleCount = 0;

    GlobalData* pGlobal = GlobalData::getInstance();
    m_signedUser.m_nId = iVisitor;
    m_signedUser.m_faceImg = faceImg;
    m_signedUser.m_strFirstName = firstName;
    m_signedUser.m_strLastName = lastName;
    pGlobal->m_pEvacuationFrm->show();
    pGlobal->m_pEvacuationFrm->activateWindow();
    hide();
}

void MainWindow::on_clickedBackfUserInfo()
{
    m_nIdleCount = 0;
    ui->mainStack->setCurrentWidget(m_pMatchWidget);
    GlobalData::getInstance()->m_camMgr.StartCamera(m_pMatchWidget);
}


void MainWindow::on_clickedYesfCheckout(QString visitorType,QImage faceImg, QString strFirst, QString strLast)
{
    m_nIdleCount = 0;

    GlobalData* pGlobal = GlobalData::getInstance();
    pGlobal->m_camMgr.StopCamera(m_pCheckOut);


    m_pThankWidget1->setVisitorInfo(faceImg, &pGlobal->m_person, visitorType);
    ui->mainStack->setCurrentWidget(m_pThankWidget1);
    m_pThankWidget1->ResetTimer();
}


void MainWindow::on_callGotoHomefThank()
{
    m_nIdleCount = 0;

    if(GlobalData::getInstance()->m_camMgr.IsOpenCamera())
        GlobalData::getInstance()->m_camMgr.StopCamera(NULL);

    ui->mainStack->setCurrentWidget(m_pSignIn);
    GlobalData::getInstance()->m_camMgr.StartCamera(m_pSignIn);
}



void MainWindow::on_signInProcessed(int status, QImage* img, int nID, int sessionId, int historyId, int staffId, PersonResult* personResult,float score)
{
    //stop camera
    GlobalData* pGlobal = GlobalData::getInstance();
    if(status == CarmeraManager::capture_unknown)
        return;
    m_nIdleCount = 0;

    if(pGlobal->m_appMode == appmode_first_visitors)
    {
        if(status == CarmeraManager::capture_misMatched)
        {
            //keep last feature.
            memcpy(pGlobal->m_lastFeature, personResult->m_pFeature, FEATURE_LEN * sizeof(float));

            pGlobal->m_camMgr.StopCamera(m_pSignIn);
            m_pMismatchWidget->init();
            m_pMismatchWidget->SetFaceImage(*personResult);
            ui->mainStack->setCurrentWidget(m_pMismatchWidget);
        }
        else if(score >= g_threshold)
        {
            pGlobal->m_camMgr.StopCamera(m_pSignIn);
            QMessageBox::about(this,  APP_TITLE, "You matched an existing user.\nPlease use Returning Visitors screen");
            on_switchSplash();
        }
    }
    else if(pGlobal->m_appMode == appmode_returning_visitors)
    {
        if(status == CarmeraManager::capture_matched || status == CarmeraManager::capture_checked_out)
        {
            //keep last feature.
            memcpy(pGlobal->m_lastFeature, personResult->m_pFeature, FEATURE_LEN * sizeof(float));

            pGlobal->m_camMgr.StopCamera(m_pSignIn);
            m_pMatchWidget->SetData(nID, historyId, *img, sessionId, score, staffId);
            ui->mainStack->setCurrentWidget(m_pMatchWidget);
            GlobalData::getInstance()->m_camMgr.StartCamera(m_pMatchWidget);
        }
        else if(status == CarmeraManager::capture_misMatched)
        {
            QMessageBox::about(this,  APP_TITLE, "Sorry your score is too low,\n please try again or login as new visitor");
            on_switchSplash();
        }
    }
    else if(pGlobal->m_appMode == appmode_signout)
    {
        if(status == CarmeraManager::capture_checked_out)
        {
            //keep last feature.
            memcpy(pGlobal->m_lastFeature, personResult->m_pFeature, FEATURE_LEN * sizeof(float));

            pGlobal->m_camMgr.StopCamera(m_pSignIn);
            m_pCheckOut->SetData(nID, historyId, *img, sessionId);
            ui->mainStack->setCurrentWidget(m_pCheckOut);
            GlobalData::getInstance()->m_camMgr.StartCamera(m_pCheckOut);
        }
        else
        {
            GlobalData::getInstance()->m_camMgr.StopCamera(m_pSignIn);
            QMessageBox::about(this,  APP_TITLE, "Please sign in before signing out");
            on_switchSplash();
        }
    }


/*
    switch(status)
    {
        case CarmeraManager::capture_checked_out:
            m_nIdleCount = 0;
            m_pCheckOut->SetData(nID, historyId, *img, sessionId);
            ui->mainStack->setCurrentWidget(m_pCheckOut);
            GlobalData::getInstance()->m_camMgr.StartCamera(m_pCheckOut);
        break;
        case CarmeraManager::capture_matched:
            m_nIdleCount = 0;
            m_pMatchWidget->SetData(nID, historyId, *img, sessionId);
            ui->mainStack->setCurrentWidget(m_pMatchWidget);
            GlobalData::getInstance()->m_camMgr.StartCamera(m_pMatchWidget);
        break;
        case CarmeraManager::capture_misMatched:
            m_nIdleCount = 0;
            m_pMismatchWidget->init();
            m_pMismatchWidget->SetFaceImage(*personResult);
            ui->mainStack->setCurrentWidget(m_pMismatchWidget);
        break;
    }
*/
}


void MainWindow::on_centerForm_destroyed()
{
    if(GlobalData::getInstance()->m_camMgr.IsOpenCamera())
        GlobalData::getInstance()->m_camMgr.StopCamera(NULL);
}


void MainWindow::on_switchSplash()
{
    m_nIdleCount = 0;
    if(GlobalData::getInstance()->m_camMgr.IsOpenCamera())
        GlobalData::getInstance()->m_camMgr.StopCamera(NULL);

    GlobalData::getInstance()->m_pSplash->show();
    GlobalData::getInstance()->m_pSplash->activateWindow();
    SplashForm* pSplash = (SplashForm*)GlobalData::getInstance()->m_pSplash;

    hide();
}

void MainWindow::on_Time()
{
    if(this->isVisible()==false)
        return;

    m_nIdleCount++;
    if(m_nIdleCount >=120)
    {
        on_switchSplash();
    }
}


void  MainWindow::wakeFromIdle()
{
    bool bDetectState = ui->btnDetect->isChecked();
    checkTabBtn(bDetectState);
}

void MainWindow::on_btnClose_clicked()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    pGlobal->m_camMgr.StopCamera(NULL);

    pGlobal->m_pSplash->close();
    this->close();
    pGlobal->m_pApp->exit(0);
}

void MainWindow::updatedAppMode()
{
    m_pSignIn->updatedAppMode();
}
