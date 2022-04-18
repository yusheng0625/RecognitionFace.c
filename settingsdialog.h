#ifndef settingsdialog_H
#define settingsdialog_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QTime>
#include <QString>
#include <QList>
#include <QCamera>
#include <QCameraInfo>
#include <QDebug>

namespace Ui {
class settingsdialog;
}

class settingsdialog : public QWidget
{
    Q_OBJECT

public:
    explicit settingsdialog(QWidget *parent = 0);

    ~settingsdialog();

private slots:

    void on_VISITOR_BTN_clicked();
    void on_CONTRACTOR_BTN_clicked();
    void on_BTN_CAMERA_clicked();
    void on_BTN_RES_clicked();

    void on_Reg_BTN_clicked();

    void on_DB_BTN_clicked();

    void on_btnSave_clicked();

    void ShowCameraMenu(QPoint pos);
    void PopCameraMenuSlot(QAction* action);

    void ShowResolutionMenu(QPoint pos);
    void PopResolutionMenuSlot(QAction* action);

private:
    Ui::settingsdialog *ui;

    QList<QString> m_CameraDescriptions;
    int m_nCameraCount;
    bool m_bMenuFlag;

public:
    void ReadSettings();
};

#endif // settingsdialog_H
