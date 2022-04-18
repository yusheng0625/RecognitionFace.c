#ifndef PRIVACYDLG_H
#define PRIVACYDLG_H

#include <QDialog>
#include <QTimer>
#include <QString>

namespace Ui {
class PrivacyDlg;
}

class PrivacyDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PrivacyDlg(QWidget *parent = 0);
    ~PrivacyDlg();

private slots:
    void on_btnNext_clicked();
    void on_Time();
signals:
    void clickedNext();
    void timedOut();
private:
    Ui::PrivacyDlg *ui;
    QTimer *m_timer;
    int     m_nTimerCount;

};

#endif // PRIVACYDLG_H
