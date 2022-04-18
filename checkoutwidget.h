#ifndef CHECKOUTWIDGET_H
#define CHECKOUTWIDGET_H

#include <QWidget>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrlQuery>

namespace Ui {
class checkoutWidget;
}

class checkoutWidget : public QWidget
{
    Q_OBJECT

public:
    explicit checkoutWidget(QWidget *parent = 0);
    ~checkoutWidget();

private slots:
    void on_btnYes_clicked();
    void on_btnNotYet_clicked();
    void on_btnNotYou_clicked();

signals:
    void clickedYesfCheckout(QString visitorType,QImage faceImg, QString strFirst, QString strLast);
    void clickedNotYet();

private:
    Ui::checkoutWidget *ui;
    void timerEvent(QTimerEvent* event);
    QNetworkAccessManager *m_networkManager;
    QNetworkReply* m_reply;
public:
    int m_nPersonId;
    int m_nHistId;
    int m_nSessionId;
    QString m_strFirst;
    QString m_strLast;
    QImage m_faceImage;
    void SetData(int nPersonId, int nHistId, QImage faceImg, int nSessionId);

    QUrlQuery m_checkoutReqParams;


    int  m_nResendTimer;
    int  m_nResendTimerCount;
    bool m_bShouldResend;
};

#endif // CHECKOUTWIDGET_H
