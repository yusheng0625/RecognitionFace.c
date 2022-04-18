#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>



namespace Ui {
class Loginwindow;
}

class Loginwindow : public QDialog
{
    Q_OBJECT

public:
    explicit Loginwindow(QWidget *parent = 0);
    ~Loginwindow();

private slots:
    void locInfoRequestFinished();
    void staffListRequestFinished();
    void on_btn_check_clicked();
private:
    Ui::Loginwindow *ui;
    bool m_bLoging;
public:
    QNetworkAccessManager *m_netManager;
    QNetworkReply* m_reply;
    void staffListRequestSend();
public:
    void setLocationKey(QString strLocationKey);

};

#endif // LOGINWINDOW_H
