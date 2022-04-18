#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "thankudialog.h"
#include "thankyouwidget.h"
#include "settingsdialog.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "visitorinfo.h"
#include "signincamera.h"
#include "matchwidget.h"
#include "matchtop3widget.h"
#include "mismatchwidget.h"
#include "userinfo.h"
#include "checkoutwidget.h"
#include "global.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void     wakeFromIdle();
private slots:
    void on_signInProcessed(int status, QImage* img, int nID, int sessionId, int historyId, int staffId, PersonResult* personResult, float);
    void on_clickedContinuefMatch(int nPersonId, QImage faceImg, int staffId);
    void on_clickedContinueToTop3fMatch(int nPersonId, QImage faceImg, int staffId);
    void on_clickedContinuefMatchTop3(int nPersonId, QImage faceImg, int staffId);


    void on_clickedContinuefUserInfo(int visitorType, QImage faceImg, QString strFirst, QString strLast);
    void on_clickedBackfUserInfo();
    void on_callGotoHomefThank();

    void on_clickedContinuefMismatch(int iVisitor, QImage faceImg, QString firstName, QString lastName);
    void on_clickedYesfCheckout(QString visitorType,QImage faceImg, QString strFirst, QString strLast);

    void on_centerForm_destroyed();
    void on_Time();
    void on_switchSplash();

    void on_clickedNextEvacuationForm();
    void on_timedOutEvacuationForm();
    void on_clickedNextPrivacyForm();
    void on_timedOutPrivacyForm();


    void on_btnDetect_toggled(bool checked);
    void on_btnSetting_toggled(bool checked);
    void on_btnHome_clicked();
    void on_btnClose_clicked();

private:
    Ui::MainWindow *ui;
    int  m_nIdleCount;
private:
    void checkTabBtn(bool bDetection);

private:
    SignInCamera* m_pSignIn;
    MatchWidget*  m_pMatchWidget;
    matchTop3Widget* m_pMatchWidgetTop3;
    MismatchWidget* m_pMismatchWidget;
    UserInfo*     m_pUserInfo;
    checkoutWidget* m_pCheckOut;

    ThankuDialog* m_pThankWidget;
    ThankyouWidget* m_pThankWidget1;
    settingsdialog* m_pSettingWidget;
protected:
    bool eventFilter(QObject *obj, QEvent *event)
    {
        if(event->type()==QEvent::MouseButtonPress ||event->type()==QEvent::KeyPress )
        {
            m_nIdleCount = 0;
        }
        return false;
    }

private:
    PersonResult m_signedUser;

public:
    void updatedAppMode();

};

#endif // MAINWINDOW_H
