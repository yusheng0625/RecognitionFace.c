#include "usermanagedialog.h"
#include "ui_usermanagedialog.h"
#include "global.h"

UserManageDialog::UserManageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserManageDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    m_bSelectedItem = false;
}

UserManageDialog::~UserManageDialog()
{
    delete ui;
}

void UserManageDialog::on_SaveButton_clicked()
{
    if (ui->tableWidget->currentRow() < 0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please select the person to update.\n"), QMessageBox::Close);
        return;
    }
    if (ui->FirstEdit->text().isEmpty() || ui->LastEdit->text().isEmpty() || ui->CompanyEdit->text().isEmpty() || ui->EmailEdit->text().isEmpty() || ui->PhoneEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please enter the person information.\n"), QMessageBox::Close);
        return;
    }
    Person person;
    person.m_nId = ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->data(Qt::UserRole).toInt();
    person.m_strFirst = ui->FirstEdit->text();
    person.m_strLast = ui->LastEdit->text();
    person.m_strCompany = ui->CompanyEdit->text();
    person.m_strEmail = ui->EmailEdit->text();
    person.m_strPhone = ui->PhoneEdit->text();
    person.m_strFilePath = person.m_strFirst + person.m_strLast + ".jpg";
    m_DBMangement.UpdatePerson(person);
    LoadUserTable();
    ui->FirstEdit->clear();
    ui->LastEdit->clear();
    ui->CompanyEdit->clear();
    ui->EmailEdit->clear();
    ui->PhoneEdit->clear();
}

void UserManageDialog::on_RemoveButton_clicked()
{
    if (ui->tableWidget->currentRow() < 0 || !m_bSelectedItem)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please select the person to remove.\n"), QMessageBox::Close);
        return;
    }
    int nId = ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->data(Qt::UserRole).toInt();
    m_DBMangement.DeletePerson(nId);
    LoadUserTable();
}

void UserManageDialog::on_CloseButton_clicked()
{
    close();
}

void UserManageDialog::LoadUserTable()
{
    //header
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("FisrtName"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("LastName"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("CompanyName"));
    ui->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem("EmailAdress"));
    ui->tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem("PhoneNumber"));
    ui->tableWidget->setHorizontalHeaderItem(5, new QTableWidgetItem("FilePath"));
    QString configDir = qApp->applicationDirPath();
    QSqlDatabase table = QSqlDatabase::addDatabase("QSQLITE");
    table.setDatabaseName(configDir + "/usertable.db");
    table.open();
    if (table.isOpen())
    {

        QSqlQuery query(table);
        query.prepare("select id, firstname, lastname, companyname, emailadress, phonenumber, filepath from usertable");
        query.exec();
        query.last();
        int rowCount = query.at() + 1;
        ui->tableWidget->setRowCount(rowCount);
        query.first();
        query.previous();
        int currntRow = 0;
        while(query.next())
        {
            ui->tableWidget->setItem(currntRow, 0, new QTableWidgetItem(query.value(1).toString()));
            ui->tableWidget->setItem(currntRow, 1, new QTableWidgetItem(query.value(2).toString()));
            ui->tableWidget->setItem(currntRow, 2, new QTableWidgetItem(query.value(3).toString()));
            ui->tableWidget->setItem(currntRow, 3, new QTableWidgetItem(query.value(4).toString()));
            ui->tableWidget->setItem(currntRow, 4, new QTableWidgetItem(query.value(5).toString()));
            ui->tableWidget->setItem(currntRow, 5, new QTableWidgetItem(query.value(6).toString()));
            ui->tableWidget->item(currntRow, 0)->setData(Qt::UserRole, query.value(0));
            currntRow++;
        }
        table.close();
     }
}

void UserManageDialog::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    m_bSelectedItem = true;
    int nId = ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->data(Qt::UserRole).toInt();
    Person* person = m_DBMangement.GetPerson(nId);
    ui->FirstEdit->setText(person->m_strFirst);
    ui->LastEdit->setText(person->m_strLast);
    ui->CompanyEdit->setText(person->m_strCompany);
    ui->EmailEdit->setText(person->m_strEmail);
    ui->PhoneEdit->setText(person->m_strPhone);
    //QPixmap image(person->m_strFilePath);

    QImage img;
    if(DecryptImage(person->m_strFilePath,img))
    {
        QPixmap image = QPixmap::fromImage(img);
        QPixmap scaled_image = image.scaled(ui->PhotoLabel->size(), Qt::KeepAspectRatio);
        ui->PhotoLabel->setPixmap(scaled_image);
    }

    delete person;
}
