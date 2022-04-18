#ifndef USERINFO_H
#define USERINFO_H

#include <QWidget>
#include <QImage>
#include <QNetworkAccessManager>
#include <QByteArray>
#include <QComboBox>
#include <QPainter>
#include "global.h"

namespace Ui {
class UserInfo;
}


class UserInfo : public QWidget
{
    Q_OBJECT

public:
    explicit UserInfo(QWidget *parent = 0);
    ~UserInfo();
    void SetData(int nId, QImage faceImg, int staffId);
private slots:
    void on_btnContinue_clicked();
    void on_btnVisitor_clicked();
    void on_btnContractor_clicked();
    void on_btn_GoBack_clicked();
signals:
    void clickedContinuefUserInfo(int visitorType,QImage faceImg, QString strFirst, QString strLast);
    void clickedBackfUserInfo();


private:
    Ui::UserInfo *ui;
    Person* m_person;
    QImage m_faceImg;
    int m_nPersonId;
    int m_nVisitorType; //0 - visitor, 1 - contractor
};

#endif // USERINFO_H
