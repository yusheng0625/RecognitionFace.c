#include "signincamera.h"
#include "ui_signincamera.h"
#include "global.h"
#include <QMessageBox>


SignInCamera::SignInCamera(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SignInCamera)
{
    ui->setupUi(this);
    //add background image
    QPixmap pixmap = QPixmap(":/images/login").scaled(size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, pixmap);
    this->setPalette(palette);
/*
    if(GlobalData::getInstance()->m_logoImage.isNull()==false)
    {
        QString strlogo = GlobalData::getInstance()->m_strCurrentDir + "appLogo.png";
        QString strStyleSheet = "border-image: url(\"" + strlogo+"\"); background:transparent;";
        strStyleSheet = strStyleSheet.replace("\\", "/");
        ui->label_8->setStyleSheet(strStyleSheet);
    }
*/
}

SignInCamera::~SignInCamera()
{
    delete ui;
}

void SignInCamera::updatedAppMode()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    switch(pGlobal->m_appMode)
    {
        case appmode_first_visitors:
            ui->label_9->setText("FIRST-TIME VISITOR SIGN IN");
            pGlobal->log("app mode => first_visitors");
            ui->label_11->setText("Please Align Your Face In The White Box To Sign In");
        break;
        case appmode_returning_visitors:
            ui->label_9->setText("RETURNING VISITOR SIGN IN");
            pGlobal->log("app mode => returning_visitors");
            ui->label_11->setText("Please Align Your Face In The White Box To Sign In");
        break;
        case appmode_signout:
            pGlobal->log("app mode => signout");
            ui->label_9->setText("VISITOR SIGN-OUT");
            ui->label_11->setText("Please Align Your Face In The White Box To Sign Out");
        break;
    }
}

void SignInCamera::timerEvent(QTimerEvent *)
{
    QImage       img;
    int nID, nHistoryID, nSessionID, nStaffId;
    float score = 0;
    PersonResult result;

    int status = GlobalData::getInstance()->m_camMgr.Capture(ui->lblCamera, &nID,
                                                             &nHistoryID, &nSessionID, &nStaffId, &img, &result, false, &score);
    if(status != CarmeraManager::capture_unknown)
    {
        emit captureProcessed(status, &img, nID, nSessionID, nHistoryID, nStaffId, &result, score);
    }
}

