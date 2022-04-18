#include "checkoutwidget.h"
#include "ui_checkoutwidget.h"
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include "global.h"
#include <QTimer>

checkoutWidget::checkoutWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::checkoutWidget)
{
    ui->setupUi(this);
    m_networkManager = new QNetworkAccessManager();
    m_reply = NULL;

    m_nResendTimer = this->startTimer(1000);
    m_nResendTimerCount = 0;
    m_bShouldResend = false;
}

checkoutWidget::~checkoutWidget()
{
    delete ui;
    delete m_networkManager;
}

void checkoutWidget::on_btnYes_clicked()
{
    GlobalData* pGlobal = GlobalData::getInstance();

    DBManagement* db = DBManagement::getInstance();

    db->lockUserTB();
    Person* person = db->GetPerson(m_nPersonId);
    db->unlockUserTB();

    if(person==NULL)
        return;

    //update image
    person->updateNewImage(m_faceImage,pGlobal->m_lastFeature);
    db->lockUserTB();
    db->UpdatePersonImages(*person);
    db->unlockUserTB();

    //delete history record.
    db->lockHistoryTB();
    db->DeleteCheck(person->m_nId);
    db->unlockHistoryTB();

    pGlobal->m_transactionMgr.scheduleSignOut(person->m_nVisitorId, person->m_nId, m_nSessionId);
    emit clickedYesfCheckout(person->m_strType, m_faceImage, m_strFirst, m_strLast);
    delete person;
}


void checkoutWidget::SetData(int nPersonId, int nHistId, QImage faceImg, int nSessionId)
{
    m_nPersonId = nPersonId;
    m_nHistId = nHistId;
    m_faceImage = faceImg.copy();
    m_nSessionId = nSessionId;

    DBManagement* db = DBManagement::getInstance();

    db->lockUserTB();
    Person* person = db->GetPerson(nPersonId);
    db->unlockUserTB();

    if(person == NULL)
    {
        GlobalData::getInstance()->log("checkoutWidget => find user failed id=" + QString::number(nPersonId));
    }

    m_strFirst = person->m_strFirst;
    m_strLast = person->m_strLast;

    ui->lblName->setText(m_strFirst + " " + m_strLast);
    ui->lblHi->setText("Hi " + m_strFirst+",");
    m_faceImage = faceImg.copy();
    QImage scaledImg = faceImg.scaled(ui->lblFace->size(), Qt::KeepAspectRatio);
    ui->lblFace->setPixmap(QPixmap::fromImage(scaledImg));
    GlobalData::getInstance()->saveData(m_faceImage, person);


    QString strlog = "checkout personId: " + QString::number(nPersonId) + " visitorId: " +
            QString::number(person->m_nVisitorId) + " name: " + person->m_strFirst + " " + person->m_strLast;
    GlobalData::getInstance()->log(strlog);

    delete person;
}

void checkoutWidget::on_btnNotYet_clicked()
{
    emit clickedNotYet();
}

void checkoutWidget::timerEvent(QTimerEvent * event)
{
    QImage       img;
    int nID, nHistoryID, nSessionID, nStaffId;
    PersonResult result;
    float  score;

    int status = GlobalData::getInstance()->m_camMgr.Capture(ui->lblCamera, &nID,
                                                             &nHistoryID, &nSessionID, &nStaffId, &img, &result,true, &score);
}

void checkoutWidget::on_btnNotYou_clicked()
{
    GlobalData::getInstance()->log("false positive");
    emit clickedNotYet();
}
