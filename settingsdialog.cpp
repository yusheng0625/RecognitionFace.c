#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "global.h"
#include <QSettings>
#include "insertdialog.h"
#include "usermanagedialog.h"

settingsdialog::settingsdialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingsdialog)
{
    ui->setupUi(this);

    ui->lnThresh->setValidator(new QIntValidator(0, 100, this));
    ui->lnCheckinTime->setValidator(new QIntValidator(1, 100, this));


    on_VISITOR_BTN_clicked();

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowCameraMenu(QPoint)));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowResolutionMenu(QPoint)));
    m_nCameraCount = 0;
    foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras()) {
        m_nCameraCount++;
        m_CameraDescriptions.append(cameraInfo.description());
    }
}

void settingsdialog::ReadSettings()
{
    QSettings setting("setting.ini", QSettings::IniFormat);

    int threshold = setting.value("threshold", "70").toInt();
    g_threshold = threshold / 100.0f;
    ui->lnThresh->setText(QString::number(threshold));

    int threshold2 = setting.value("threshold2", "60").toInt();
    if(threshold2 < 60)
        threshold2 = 60;

    g_threshold2 = threshold2 / 100.0f;
    ui->lnThresh_2->setText(QString::number(threshold2));


    int checkIntime = setting.value("checkintime", "1").toInt();
    ui->lnCheckinTime->setText(QString::number(checkIntime));

    bool actionSwitch = setting.value("actionswitch", "false").toBool();
    ui->ACTION_BTN->SetSwitch(actionSwitch);

    bool saveImg = setting.value("saveimage", "false").toBool();
    ui->SAVE_BTN->SetSwitch(saveImg);

    bool threshslider = setting.value("threshslider", "false").toBool();
    ui->SLIDE_BTN->SetSwitch(threshslider);

    int dpi = setting.value("printer_dpi", "300").toInt();
    ui->printer_dpi->setText(QString::number(dpi));
    g_printerDPI = dpi;

    g_strClientName = setting.value("clientname", "Acer").toString();

    g_camIdx = setting.value("camidx", "0").toInt();
    g_resIdx = setting.value("residx", "0").toInt();
}

settingsdialog::~settingsdialog()
{
    delete ui;
}

void settingsdialog::on_VISITOR_BTN_clicked()
{
    ui->VISITOR_BTN->setStyleSheet("QPushButton {background-color: rgb(18, 67, 125); color: white; border-radius: 0px; }");
    ui->CONTRACTOR_BTN->setStyleSheet("background:transparent; color:rgb(255,255,255); border-style: solid; border-color: rgb(18, 67, 125); border-radius: 0px; border-width: 1px;}");
}

void settingsdialog::on_CONTRACTOR_BTN_clicked()
{
    ui->CONTRACTOR_BTN->setStyleSheet("QPushButton {background-color: rgb(18, 67, 125); color: white; border-radius: 0px; }");
    ui->VISITOR_BTN->setStyleSheet("background: transparent; color:rgb(255,255,255); border-style: solid; border-color: rgb(18, 67, 125); border-radius: 0px; border-width: 1px;}");
}

void settingsdialog::on_BTN_CAMERA_clicked()
{
    m_bMenuFlag = true;
    emit customContextMenuRequested(QPoint(ui->BTN_CAMERA->pos().x() + ui->widget->pos().x() + ui->widget_3->pos().x() + ui->BTN_CAMERA->width(), ui->BTN_CAMERA->pos().y() + ui->widget->pos().y() + ui->widget_3->pos().y()));
}

void settingsdialog::on_BTN_RES_clicked()
{
    m_bMenuFlag = false;
    emit customContextMenuRequested(QPoint(ui->BTN_RES->pos().x() + ui->widget->pos().x() + ui->widget_4->pos().x() + ui->BTN_RES->width(), ui->BTN_RES->pos().y() + ui->widget->pos().y() + ui->widget_4->pos().y()));
}

void settingsdialog::on_Reg_BTN_clicked()
{
    InsertDialog dlg;
    dlg.exec();
}

void settingsdialog::on_DB_BTN_clicked()
{
    UserManageDialog dlg;
    dlg.LoadUserTable();
    dlg.exec();
}

void settingsdialog::on_btnSave_clicked()
{
    QSettings setting("setting.ini", QSettings::IniFormat);
    setting.setValue("threshold", ui->lnThresh->text());
    g_threshold = ui->lnThresh->text().toInt() / 100.0f;
    setting.setValue("threshold2", ui->lnThresh_2->text());
    g_threshold2 = ui->lnThresh_2->text().toInt() / 100.0f;

    setting.setValue("checkintime", ui->lnCheckinTime->text());
    setting.setValue("actionswitch", ui->ACTION_BTN->GetSwitch());
    setting.setValue("saveimage", ui->SAVE_BTN->GetSwitch());
    setting.setValue("threshslider", ui->SLIDE_BTN->GetSwitch());
    setting.setValue("camidx", g_camIdx);
    setting.setValue("residx", g_resIdx);
    setting.setValue("printer_dpi", ui->printer_dpi->text());
}

void settingsdialog::ShowCameraMenu(QPoint pos)
{
    if (m_bMenuFlag)
    {
        QMenu* contextMenu = new QMenu(tr("camera menu"), this);
        QActionGroup* actions = new QActionGroup(contextMenu);
        actions->setExclusive(false);
        for (int i = 0; i < m_nCameraCount; i++)
        {
            QString actionName = m_CameraDescriptions.at(i);
            QAction* action = contextMenu->addAction(actionName);
            action->setCheckable(true);
            if (g_camIdx == i) action->setChecked(true);
            else action->setChecked(false);
            actions->addAction(action)->setData(i);
            connect(actions, SIGNAL(triggered(QAction*)), SLOT(PopMenuSlot(QAction*)));
        }
        contextMenu->setStyleSheet("background-color: rgb(128, 128, 128);");
        contextMenu->exec(mapToGlobal(pos));
    }
}

void settingsdialog::PopCameraMenuSlot(QAction* action)
{
    int value = action->data().toInt();
    g_camIdx = value;
}

void settingsdialog::ShowResolutionMenu(QPoint pos)
{
    if (!m_bMenuFlag)
    {
        QMenu* contextMenu = new QMenu(tr("camera menu"), this);
        QActionGroup* actions = new QActionGroup(contextMenu);
        actions->setExclusive(false);
        int w = 320;
        int h = 240;
        for (int i = 0; i < 3; i++)
        {
            QString actionName =  QString::number(w * (i + 2)) + " * " + QString::number(h * (i + 2));
            QAction* action = contextMenu->addAction(actionName);
            action->setCheckable(true);
            if (g_resIdx == i) action->setChecked(true);
            else action->setChecked(false);
            actions->addAction(action)->setData(i);
            connect(actions, SIGNAL(triggered(QAction*)), SLOT(PopResolutionMenuSlot(QAction*)));
        }
        contextMenu->setStyleSheet("background-color: rgb(128, 128, 128);");
        contextMenu->exec(mapToGlobal(pos));
    }
}

void settingsdialog::PopResolutionMenuSlot(QAction* action)
{
    int value = action->data().toInt();
    g_resIdx = value;
}
