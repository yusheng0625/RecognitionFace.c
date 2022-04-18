#ifndef THANKUDIALOG_H
#define THANKUDIALOG_H

#include <QWidget>
#include <QTimer>
#include "global.h"

namespace Ui {
class ThankuDialog;
}

class ThankuDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ThankuDialog(QWidget *parent = 0);
    ~ThankuDialog();

private:
    Ui::ThankuDialog *ui;

signals:
    void callGotoHome();
public:
    QTimer* timer;
    void ResetTimer();

public:
    QImage m_faceImage;
    QString m_strFirst;
    QString m_strLast;
public:
    void setVisitorInfo(QImage img, Person* person, QString strVisitorType);
    void print();
};

#endif // THANKUDIALOG_H
