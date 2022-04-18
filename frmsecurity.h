#ifndef FRMSECURITY_H
#define FRMSECURITY_H

#include <QDialog>

namespace Ui {
class frmSecurity;
}

class frmSecurity : public QDialog
{
    Q_OBJECT

public:
    explicit frmSecurity(QWidget *parent = 0);
    ~frmSecurity();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::frmSecurity *ui;
};

#endif // FRMSECURITY_H
