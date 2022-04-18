#include "splashform.h"
#include "ui_splashform.h"
#include <QDesktopWidget>
#include "global.h"
#include "MainWindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SplashForm::SplashForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SplashForm)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint);
    window()->showMaximized();
    window()->setGeometry(QApplication::desktop()->availableGeometry());

    //back ground
    QPixmap pixmap = QPixmap(":/images/login").scaled(size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, pixmap);
    this->setPalette(palette);

    //logo image
    if(GlobalData::getInstance()->m_logoImage.isNull()==false)
    {
        QString strlogo = GlobalData::getInstance()->m_strCurrentDir + "appLogo.png";
        QString strStyleSheet = "border-image: url(\"" + strlogo+"\"); background:transparent;";
        strStyleSheet = strStyleSheet.replace("\\", "/");
        ui->label_logo->setStyleSheet(strStyleSheet);
    }

/*
    //add background image
    QPixmap pixmap = QPixmap(":/images/splash").scaled(size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, pixmap);
    this->setPalette(palette);
*/
    installEventFilter(this);
}

SplashForm::~SplashForm()
{
    delete ui;    
}

void SplashForm::on_SplashForm_accepted()
{

}

void SplashForm::on_SplashForm_rejected()
{

}



bool SplashForm::eventFilter(QObject *obj, QEvent *event)
{
/*
    if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::KeyPress)
    {
        MainWindow* pMain = (MainWindow*)GlobalData::getInstance()->m_pMain;
        pMain->wakeFromIdle();
        pMain->show();

        hide();
        return true;
    }
*/
    GlobalData* pGlobal = GlobalData::getInstance();
    MainWindow* pMain = (MainWindow*)pGlobal->m_pMain;

    if (event->type() == QEvent::MouseButtonPress)
    {
        if (ui->widget_first_visitors->underMouse())
        {
            pGlobal->m_appMode = appmode_first_visitors;
            pMain->updatedAppMode();
            pMain->wakeFromIdle();
            pMain->show();
            pMain->activateWindow();
            hide();
        }
        else if(ui->widget_returning_visitors->underMouse())
        {
            pGlobal->m_appMode = appmode_returning_visitors;
            pMain->updatedAppMode();
            pMain->wakeFromIdle();
            pMain->show();
            pMain->activateWindow();
            hide();
        }
        else if(ui->widget_signout->underMouse())
        {
            pGlobal->m_appMode = appmode_signout;
            pMain->updatedAppMode();
            pMain->wakeFromIdle();
            pMain->show();
            pMain->activateWindow();
            hide();
        }
        return true;
    }

    return false;
}
