#include "privacydlg.h"
#include "ui_privacydlg.h"
#include <QApplication>
#include <QDesktopWidget>
#include "global.h"

PrivacyDlg::PrivacyDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrivacyDlg)
{
    ui->setupUi(this);
//    setWindowFlags( Qt::FramelessWindowHint );
    //set full screen
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowSystemMenuHint);
    window()->showMaximized();
    window()->setGeometry(QApplication::desktop()->availableGeometry());

    QLabel* imgLabel = new QLabel();

    imgLabel->setBackgroundRole(QPalette::Base);
    imgLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imgLabel->setScaledContents(true);
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    ui->scrollArea->setWidget(imgLabel);
    ui->scrollArea->setWidgetResizable(false);


    QImage img;
    img.load(GlobalData::getInstance()->m_strCurrentDir + "terms.png");
    imgLabel->setPixmap(QPixmap::fromImage(img));
    imgLabel->adjustSize();


    //ui->scrollArea->setVisible(false);
#ifdef DEV_VERSION
/*
    QString strTerms = GlobalData::getInstance()->m_strTerms;
    if(strTerms.isEmpty() ==false)
    {
        ui->textEdit->setText(strTerms);
    }
*/
#endif

    m_nTimerCount = 0;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_Time()));
    m_timer->start(1000);    

}

PrivacyDlg::~PrivacyDlg()
{
    m_timer->stop();
    delete m_timer;
    delete ui;
}

void PrivacyDlg::on_btnNext_clicked()
{
    emit(clickedNext());
}
void PrivacyDlg::on_Time()
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
