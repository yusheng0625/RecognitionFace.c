#ifndef FLOORDLG_H
#define FLOORDLG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class FloorDlg;
}

class FloorDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FloorDlg(QWidget *parent = 0);
    ~FloorDlg();

private slots:
    void on_btnNext_clicked();
    void on_Time();
signals:
    void clickedNext();
    void timedOut();
private:
    Ui::FloorDlg *ui;
    QTimer *m_timer;
    int     m_nTimerCount;

};

#endif // FLOORDLG_H
