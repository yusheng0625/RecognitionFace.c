#include "userinfo.h"
#include "ui_userinfo.h"
#include "dbmanagement.h"
#include <QMessageBox>
#include <QTime>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QBuffer>
#include <QListView>
#include <QTimer>
#include <QProxyStyle>
#include <QRect>
#include <QPainter>





UserInfo::UserInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserInfo)
{
    ui->setupUi(this);

    QListView *listVw = (QListView*)ui->cbxStaff->view();
    if (listVw) {
        listVw->setSpacing(10);
    }
    QLineEdit* pLine = ui->cbxStaff->lineEdit();

    if(pLine)
    {
        pLine->setPlaceholderText("Visiting");
        QFont font("AcerFoco Rg", 14);
        pLine->setFont(font);
    }
    m_person = NULL;
}

UserInfo::~UserInfo()
{
    delete ui;
}


void UserInfo::on_btnContinue_clicked()
{
    GlobalData* pGlobal = GlobalData::getInstance();

    if(m_person == NULL)
        return;
    //update image
    m_person->updateNewImage(m_faceImg, pGlobal->m_lastFeature);

    DBManagement* db = DBManagement::getInstance();

    db->lockUserTB();
    db->UpdatePersonImages(*m_person);
    db->unlockUserTB();


    QString strFirst = ui->lnFirst->text().trimmed();
    if(strFirst.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your first name!");
        return;
    }
    QString strLast = ui->lnLast->text().trimmed();
    if(strLast.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your last name!");
        return;
    }
    QString strCompany = ui->lnCompany->text().trimmed();
    if(strCompany.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your company name!");
        return;
    }
    QString strEmail = ui->lnEmail->text().trimmed();
    QString strPhone = ui->lnPhone->text().trimmed();
    if(strPhone.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your phone number!");
        return;
    }

    if(ui->cbxStaff->currentIndex() < 0)
    {
        QMessageBox::warning(this,"Warning",
                             "Please select which staff member you are visiting or select Reception");
        return;
    }

    int nStaffId = (int)ui->cbxStaff->currentData().toInt();
    g_nStaffId = nStaffId;

    //QMessageBox::about(NULL, QString::number(g_nStaffId), "slected staff");

    //fill person
    m_person->m_strFirst = strFirst;
    m_person->m_strLast = strLast;
    m_person->m_strCompany = strCompany;
    m_person->m_strEmail = strEmail;
    m_person->m_strPhone = strPhone;

    if(m_nVisitorType ==0)
        m_person->m_strType = "VISITOR";
    else
        m_person->m_strType = "CONTRACTOR";



    QTime time = QTime::currentTime();
    int ntime = time.hour() * 60 + time.minute();

    db->lockHistoryTB();
    db->InsertCheck(m_person->m_nId, ntime, nStaffId, 0);
    db->UpdateCheck(m_person->m_nId, ntime, nStaffId);
    db->unlockHistoryTB();

    pGlobal->m_transactionMgr.scheduleSigninReturningUser(m_person, m_faceImg, nStaffId);
    emit clickedContinuefUserInfo(m_nVisitorType, m_faceImg, m_person->m_strFirst, m_person->m_strLast);
}


void UserInfo::SetData(int nId, QImage faceImg, int staffId)
{
    m_nPersonId = nId;
    m_faceImg = faceImg.copy();

#ifdef LOG_FILE_DEBUG
    GlobalData::getInstance()->log("UserInfo setData_ 0");
#endif

    if(m_person)
    {
        delete m_person;
        m_person = NULL;
    }

#ifdef LOG_FILE_DEBUG
    GlobalData::getInstance()->log("UserInfo setData_ 1");
#endif

    DBManagement* db = DBManagement::getInstance();
    db->lockUserTB();
    m_person = db->GetPerson(nId);
    db->unlockUserTB();

    if(m_person ==NULL)
        return;

#ifdef LOG_FILE_DEBUG
    GlobalData::getInstance()->log("UserInfo setData_ 2");
#endif


    ui->lblName->setText("Hi " + m_person->m_strFirst);
    QImage scaledImg = faceImg.scaled(ui->lblFace->size(), Qt::KeepAspectRatio);
    ui->lblFace->setPixmap(QPixmap::fromImage(scaledImg));

    ui->lnFirst->setText(m_person->m_strFirst);
    ui->lnLast->setText(m_person->m_strLast);
    ui->lnCompany->setText(m_person->m_strCompany);
    ui->lnEmail->setText(m_person->m_strEmail);
    ui->lnPhone->setText(m_person->m_strPhone);
    ui->cbxStaff->clear();

    int nCnt = g_staffList.count();
    int iSelect = -1;
    for(int i = 0; i < nCnt; i++)
    {
        ui->cbxStaff->addItem(g_staffList.at(i).m_strName, Qt::UserRole | Qt::AlignCenter);
        ui->cbxStaff->setItemData(i, QVariant(g_staffList.at(i).m_nStaffId));
        if(staffId == g_staffList.at(i).m_nStaffId)
            iSelect = i;
    }
    //ui->cbxStaff->model()->sort(0);
    ui->cbxStaff->setCurrentIndex(iSelect);

    if(m_person->m_strType == "VISITOR")
        on_btnVisitor_clicked();
    else
        on_btnContractor_clicked();

#ifdef LOG_FILE_DEBUG
    GlobalData::getInstance()->log("UserInfo setData_ 3");
#endif

}

void UserInfo::on_btnVisitor_clicked()
{
    ui->btnVisitor->setStyleSheet("QPushButton {background-color: rgb(18, 67, 125); color: white; border-radius: 0px; }");
    ui->btnContractor->setStyleSheet("background: transparent; color: white; border-style: solid; border-color: rgb(18, 67, 125); border-radius: 0px; border-width: 1px;}");
    m_nVisitorType = 0;
}

void UserInfo::on_btnContractor_clicked()
{
    ui->btnContractor->setStyleSheet("QPushButton {background-color: rgb(18, 67, 125); color: white; border-radius: 0px; }");
    ui->btnVisitor->setStyleSheet("background: transparent; color: white; border-style: solid; border-color: rgb(18, 67, 125); border-radius: 0px; border-width: 1px;}");
    m_nVisitorType = 1;

}

void UserInfo::on_btn_GoBack_clicked()
{
    emit clickedBackfUserInfo();
}
