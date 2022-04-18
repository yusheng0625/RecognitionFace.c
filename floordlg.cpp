#include "floordlg.h"
#include "ui_floordlg.h"
#include <QApplication>
#include <QDesktopWidget>
#include "global.h"

FloorDlg::FloorDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FloorDlg)
{
    ui->setupUi(this);
//    setWindowFlags( Qt::FramelessWindowHint );

    //set full screen
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint);
    window()->showMaximized();
    window()->setGeometry(QApplication::desktop()->availableGeometry());

#ifdef DEV_VERSION
    //evac image
    if(GlobalData::getInstance()->m_evacImage.isNull()==false)
    {
        QString strlogo = GlobalData::getInstance()->m_strCurrentDir + "evacImage.png";
        QString strStyleSheet = "border-image: url(\"" + strlogo+"\");";
        strStyleSheet = strStyleSheet.replace("\\", "/");
        ui->label_3->setStyleSheet(strStyleSheet);
    }
#endif

    m_nTimerCount = 0;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_Time()));
    m_timer->start(1000);


}

FloorDlg::~FloorDlg()
{
    m_timer->stop();
    delete m_timer;
    delete ui;
}

void FloorDlg::on_btnNext_clicked()
{
    emit(clickedNext());
}

void FloorDlg::on_Time()
{
    if(isVisible()==false)
    {
        m_nTimerCount = 0;
        return;
    }

    m_nTimerCount++;
    if(m_nTimerCount >=120)
    {
        m_nTimerCount = 0;
        emit(timedOut());
    }
}
