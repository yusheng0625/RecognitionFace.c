#ifndef USERMANAGEDIALOG_H
#define USERMANAGEDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QTableWidgetItem>
#include "dbmanagement.h"

namespace Ui {
class UserManageDialog;
}

class UserManageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserManageDialog(QWidget *parent = 0);
    ~UserManageDialog();
private slots:
    void on_SaveButton_clicked();

    void on_RemoveButton_clicked();

    void on_CloseButton_clicked();

    void on_tableWidget_itemClicked(QTableWidgetItem *item);

private:
    Ui::UserManageDialog *ui;
    bool m_bSelectedItem;
public:
    void LoadUserTable();
    DBManagement m_DBMangement;
};

#endif // USERMANAGEDIALOG_H
