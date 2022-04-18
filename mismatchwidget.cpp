#include "mismatchwidget.h"
#include "ui_mismatchwidget.h"
#include <QMessageBox>
#include <QTime>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QBuffer>
#include <QListView>

#include "privacydlg.h"


MismatchWidget::MismatchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MismatchWidget)
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


    on_btnVisitor_clicked();

    m_nVisitorType = 0;
    m_curPerson = NULL;
}

MismatchWidget::~MismatchWidget()
{
    delete ui;
}


void MismatchWidget::on_btnVisitor_clicked()
{
    ui->btnVisitor->setStyleSheet("QPushButton {background-color: rgb(18, 67, 125); color: white; border-radius: 0px; }");
    ui->btnContractor->setStyleSheet("background:transparent; border-style: solid; border-color: rgb(18, 67, 125); border-radius: 0px; border-width: 1px;}");
    m_nVisitorType = 0;
}

void MismatchWidget::on_btnContractor_clicked()
{
    ui->btnContractor->setStyleSheet("QPushButton {background-color: rgb(18, 67, 125); color: white; border-radius: 0px; }");
    ui->btnVisitor->setStyleSheet("background:transparent; border-style: solid; border-color: rgb(18, 67, 125); border-radius: 0px; border-width: 1px;}");
    m_nVisitorType = 1;
}

void MismatchWidget::SetFaceImage(PersonResult persondata)
{
    memcpy(m_feature, persondata.m_pFeature, FEATURE_LEN * sizeof(float));
	m_faceImage = persondata.m_faceImg.copy();
	QImage scaledImg = persondata.m_faceImg.scaled(ui->lblFace->size(), Qt::KeepAspectRatio);
    ui->lblFace->setPixmap(QPixmap::fromImage(scaledImg));

    ui->cbxStaff->clear();

    int nCnt = g_staffList.count();
    for(int i = 0; i < nCnt; i++)
    {
        ui->cbxStaff->addItem(g_staffList.at(i).m_strName, Qt::UserRole | Qt::AlignCenter);
        ui->cbxStaff->setItemData(i, QVariant(g_staffList.at(i).m_nStaffId));
    }
    ui->cbxStaff->model()->sort(0);
    ui->cbxStaff->setCurrentIndex(-1);

}

void MismatchWidget::on_btnContinue_clicked()
{
    GlobalData* pGlobal = GlobalData::getInstance();

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
/*
    if(strEmail.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your email address!");
        return;
    }
*/
    QString strPhone = ui->lnPhone->text().trimmed();
    if(strPhone.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your phone number!");
        return;
    }

    if(ui->cbxStaff->currentIndex() < 0)
    {
        QMessageBox::warning(this, "Warning", "Please select the visiting!");
        return;
    }
    int nStaffId = (int)ui->cbxStaff->currentData().toInt();
    g_nStaffId = nStaffId;


    QString filePath = tr("%1_%2_1.jpg").arg(strFirst).arg(strLast);
    QString strType = GlobalData::getInstance()->getVisitorType(m_nVisitorType);

    EncryptImage(m_faceImage, g_imagepath + "/" + filePath);
    //if(!m_faceImage.save(g_imagepath + "/" + filePath))
        //return;

    if(m_curPerson!=NULL)
        delete m_curPerson;

    m_curPerson = new Person(0, 0, strFirst, strLast, strCompany, strEmail, strPhone, m_feature, FEATURE_LEN, filePath,strType, "","",NULL,NULL);
    pGlobal->saveData(m_faceImage, m_curPerson);

    //update db.
    DBManagement* db = DBManagement::getInstance();

    db->lockUserTB();
    db->InsertPerson(*m_curPerson);
    db->unlockUserTB();

    QTime time = QTime::currentTime();
    int ntime = time.hour() * 60 + time.minute();

    db->lockHistoryTB();
    db->InsertCheck(m_curPerson->m_nId, ntime, nStaffId, 0);    
    db->UpdateCheck(m_curPerson->m_nId, ntime, nStaffId);
    db->unlockHistoryTB();

    pGlobal->m_transactionMgr.scheduleSigninFirstUser(m_curPerson, m_faceImage, nStaffId);


    emit clickedContinuefMismatch(m_nVisitorType, m_faceImage, m_curPerson->m_strFirst, m_curPerson->m_strLast);
}

void MismatchWidget::on_TRY_BTN_clicked()
{
    emit clickedTryagain();
}

void MismatchWidget::on_lnFirst_textEdited(const QString &arg1)
{
    ui->lnFirst->setText(arg1.toUpper());
}

void MismatchWidget::on_lnLast_textEdited(const QString &arg1)
{
    ui->lnLast->setText(arg1.toUpper());
}

void MismatchWidget::init()
{
    if(m_curPerson)
    {
        delete m_curPerson;
        m_curPerson = NULL;
    }
    ui->lnFirst->setText("");
    ui->lnLast->setText("");
    ui->lnCompany->setText("");
    ui->lnEmail->setText("");
    ui->lnPhone->setText("");
    ui->cbxStaff->setCurrentIndex(-1);
}
