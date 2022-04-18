#include "matchtop3widget.h"
#include "ui_matchtop3widget.h"
#include "global.h"
#include <QMessageBox>

matchTop3Widget::matchTop3Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::matchTop3Widget)
{
    ui->setupUi(this);
}

matchTop3Widget::~matchTop3Widget()
{
    delete ui;
}


void matchTop3Widget::SetData(int nPersonId, QImage faceImg, int staffId)
{
    GlobalData* pGlobal = GlobalData::getInstance();

    m_nPersonId = nPersonId;
    //m_nHistId = nHistId;
    m_faceImage = faceImg.copy();    
    //m_nSessionId = nSessionId;
    m_nStaffId = staffId;

    DBManagement* db = DBManagement::getInstance();

    db->lockUserTB();
    Person* person = db->GetPerson(nPersonId);
    db->unlockUserTB();

    if(person == NULL)
    {
        pGlobal->log("matchTop3Widget => Can't find user from db id=" + QString::number(nPersonId));
        return;
    }

    m_strFirst = person->m_strFirst;
    m_strLast = person->m_strLast;

    ui->lblName->setText(m_strFirst + " " + m_strLast + "?");

    QImage scaledImg = faceImg.scaled(ui->lblFace->size(), Qt::KeepAspectRatio);
    ui->lblFace->setPixmap(QPixmap::fromImage(scaledImg));


    pGlobal->saveData(m_faceImage, person);
    delete person;


    //top3 matches
    PersonResult* personRes;
    if(pGlobal->m_matchListTop3.size()>=2)
    {
        personRes = pGlobal->m_matchListTop3.at(1);
        ui->btn_top_match2->setText(personRes->m_strFirstName + " " + personRes->m_strLastName);
    }

    if(pGlobal->m_matchListTop3.size()>=3)
    {
        personRes = pGlobal->m_matchListTop3.at(2);
        ui->btn_top_match3->setText(personRes->m_strFirstName + " " + personRes->m_strLastName);
    }

}


void matchTop3Widget::timerEvent(QTimerEvent * event)
{
    QImage       img;
    int nID, nHistoryID, nSessionID, nStaffId;
    PersonResult result;

    GlobalData::getInstance()->m_camMgr.Capture(ui->lblCamera, &nID,
                                                             &nHistoryID, &nSessionID, &nStaffId, &img, &result, true);
}


void matchTop3Widget::on_btn_top_match2_clicked()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    DBManagement* db = DBManagement::getInstance();

    if(pGlobal->m_matchListTop3.size()>=2)
    {
        PersonResult* personRes = pGlobal->m_matchListTop3.at(1);

        db->lockUserTB();
        Person* person = db->GetPerson(personRes->m_nId);
        db->unlockUserTB();

        if(person == NULL)
        {
            pGlobal->log("MatchWidget=> Top2 User no exist in DB, personId=" + QString::number(personRes->m_nId));
            return;
        }
        pGlobal->saveData(personRes->m_faceImg, person);
        delete person;

        emit clickedContinue(personRes->m_nId, personRes->m_faceImg, m_nStaffId);
    }
}

void matchTop3Widget::on_btn_top_match3_clicked()
{
    GlobalData* pGlobal = GlobalData::getInstance();
    DBManagement* db = DBManagement::getInstance();

    if(pGlobal->m_matchListTop3.size()>=3)
    {
        PersonResult* personRes = pGlobal->m_matchListTop3.at(2);


        db->lockUserTB();
        Person* person = db->GetPerson(personRes->m_nId);
        db->unlockUserTB();

        if(person == NULL)
        {
            pGlobal->log("MatchWidget=> Top3 User no exist in DB, personId=" + QString::number(personRes->m_nId));
            return;
        }
        pGlobal->saveData(personRes->m_faceImg, person);
        delete person;

        emit clickedContinue(personRes->m_nId, personRes->m_faceImg, m_nStaffId);
    }
}

void matchTop3Widget::on_btn_nether_clicked()
{
    QMessageBox::about(this, APP_TITLE, "Please try again or sign in as a 1st Time Visitor");
    emit clickedGoBack();
}
