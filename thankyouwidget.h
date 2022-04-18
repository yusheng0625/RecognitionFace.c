#ifndef THANKYOUWIDGET_H
#define THANKYOUWIDGET_H

#include <QWidget>
#include <QTimer>
#include "global.h"

namespace Ui {
class ThankyouWidget;
}

class ThankyouWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ThankyouWidget(QWidget *parent = 0);
    ~ThankyouWidget();
    void ResetTimer();
    bool m_bLiveTimer;

    void setVisitorInfo(QImage img, Person* person, QString strVisitorType);
    void print();
private slots:
    void on_btnCheckOut_clicked();
    void on_Time();
signals:
    void callGotoHome();

private:
    Ui::ThankyouWidget *ui;
    QTimer *m_timer;
    int     m_nTimerCount;
};

#endif // THANKYOUWIDGET_H
