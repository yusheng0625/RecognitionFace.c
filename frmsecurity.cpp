#include "frmsecurity.h"
#include "ui_frmsecurity.h"
#include "global.h"
#include <QMessageBox>


frmSecurity::frmSecurity(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmSecurity)
{
    ui->setupUi(this);
}

frmSecurity::~frmSecurity()
{
    delete ui;
}


void frmSecurity::on_pushButton_clicked()
{
    QString strCode = ui->lineEdit->text();

    //QMessageBox::about(NULL,strCode + ", " + g_strLocationKey,"");

    if(strCode.length()<5)
    {
        ui->lineEdit->setText("");
        ui->lineEdit->setPlaceholderText("Invalid security code, retry again!");
        return;
    }

    if(g_strLocationKey.indexOf(strCode) >=0)
        this->accept();
    else
    {
        ui->lineEdit->setText("");
        ui->lineEdit->setPlaceholderText("Invalid security code, retry again!");
    }
}

void frmSecurity::on_pushButton_2_clicked()
{
    this->reject();
}
