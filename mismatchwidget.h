#ifndef MISMATCHWIDGET_H
#define MISMATCHWIDGET_H

#include <QWidget>
#include <QTimer>
#include "global.h"
#include <dbmanagement.h>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QByteArray>

namespace Ui {
class MismatchWidget;
}

class MismatchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MismatchWidget(QWidget *parent = 0);
    ~MismatchWidget();

public:
    float m_feature[FEATURE_LEN];
    QImage m_faceImage;
    Person* m_curPerson;

	void SetFaceImage(PersonResult persondata);

    int m_nVisitorType; //0 - visitor, 1 - contractor

    void init();
private slots:
    void on_btnVisitor_clicked();
    void on_btnContractor_clicked();
    void on_btnContinue_clicked();

    void on_TRY_BTN_clicked();
    void on_lnFirst_textEdited(const QString &arg1);
    void on_lnLast_textEdited(const QString &arg1);

signals:
    void clickedTryagain();
    void clickedContinuefMismatch(int iVisitor, QImage faceImg, QString firstName, QString lastName);

private:
    Ui::MismatchWidget *ui;
};

#endif // MISMATCHWIDGET_H
