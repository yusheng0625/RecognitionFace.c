#include "matchwidget.h"
#include "ui_matchwidget.h"
#include "dbmanagement.h"
#include <QTime>
#include <QMessageBox>
#include "global.h"

MatchWidget::MatchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MatchWidget)
{
    ui->setupUi(this);
    m_bPrimaryThresoldMatched = true;
}

MatchWidget::~MatchWidget()
{
    delete ui;
}

void MatchWidget::on_btnContinue_clicked()
{
    emit clickedContinue(m_nPersonId, m_faceImage,m_nStaffId);
}

void MatchWidget::SetData(int nPersonId, int nHistId, QImage faceImg, int nSessionId, float score, int staffId)
{
    GlobalData* pGlobal = GlobalData::getInstance();
    m_nPersonId = nPersonId;
    //m_nHistId = nHistId;
    m_faceImage = faceImg.copy();
    m_nStaffId = staffId;
    //m_nSessionId = nSessionId;
    DBManagement* db = DBManagement::getInstance();

    db->lockUserTB();
    Person* person = db->GetPerson(nPersonId);
    db->unlockUserTB();

    if(person == NULL)
    {
        pGlobal->log("MatchWidget => Can't find user from db id=" + QString::number(nPersonId));
        return;
    }


    m_strFirst = person->m_strFirst;
    m_strLast = person->m_strLast;

    if(score >= g_threshold)
    {
        m_bPrimaryThresoldMatched = true;
        ui->lblWelcome->setVisible(true);
        ui->btn_notYou->setVisible(true);

        ui->lblName->setText(m_strFirst + " " + m_strLast);
        ui->lblWelcome->setText("Welcome Back " + person->m_strFirst);

        ui->btnGoBack->setText("GO BACK");
        ui->btnContinue->setText("CONTINUE");
    }
    else
    {
        m_bPrimaryThresoldMatched = false;
        ui->lblWelcome->setVisible(false);
        ui->btn_notYou->setVisible(false);

        //ui->lblName->setText("We think you might be\n\n" + m_strFirst + " " + m_strLast + "?");
        ui->lblName->setText("We think you might be\n\n" + m_strFirst + "?");
        ui->btnGoBack->setText("NO");
        ui->btnContinue->setText("YES");
    }


    QImage scaledImg = faceImg.scaled(ui->lblFace->size(), Qt::KeepAspectRatio);
    ui->lblFace->setPixmap(QPixmap::fromImage(scaledImg));

    pGlobal->saveData(m_faceImage, person);
    delete person;

}


void MatchWidget::timerEvent(QTimerEvent * event)
{

    {
        QImage       img;
        int nID, nHistoryID, nSessionID, nstaffId;
        PersonResult result;
        float score;
        GlobalData::getInstance()->m_camMgr.Capture(ui->lblCamera, &nID,
                                                                 &nHistoryID, &nSessionID, &nstaffId, &img, &result, true);
    }
}

void MatchWidget::on_btnGoBack_clicked()
{
    if(m_bPrimaryThresoldMatched)
        emit clickedGoBack();
    else
        emit clickedContinueSecondaryScore(m_nPersonId, m_faceImage, m_nStaffId);
}

void MatchWidget::on_btn_notYou_clicked()
{
    GlobalData::getInstance()->log("false positive");
    emit clickedGoBack();
}
