#include "insertdialog.h"
#include "ui_insertdialog.h"
#include "faceDetect.h"
#include "FaceApi.h"
#include <QMessageBox>
#include "dbmanagement.h"
#include "global.h"

InsertDialog::InsertDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    ui->cmb_type->addItem("VISITOR");
    ui->cmb_type->addItem("CONTRACTOR");
    ui->cmb_type->setCurrentIndex(0);
}

InsertDialog::~InsertDialog()
{
    delete ui;
}

void InsertDialog::on_CloseButton_clicked()
{
    close();
}

void InsertDialog::on_BrowserButton_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "",
                                                     tr("Images (*.png *.xpm *.jpg)"));
    if (fileName.isEmpty()) return;
    qDebug() << fileName;
    QPixmap image(fileName);
    m_Image = image;
    QPixmap scaled_image = image.scaled(ui->PhotoLabel->size(), Qt::KeepAspectRatio);
    ui->PhotoLabel->setPixmap(scaled_image);
}

void InsertDialog::on_AddButton_clicked()
{
    if(m_Image.width() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please select picture correctly!");
        return;
    }
    QString strFirst = ui->FirstEdit->text().trimmed();
    if(strFirst.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your first name!");
        return;
    }
    QString strLast = ui->LastEdit->text().trimmed();
    if(strLast.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your last name!");
        return;
    }
    QString strCompany = ui->CompanyEdit->text().trimmed();
    if(strCompany.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your company name!");
        return;
    }
    QString strEmail = ui->EmailEdit->text().trimmed();
    if(strEmail.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your email address!");
        return;
    }
    QString strPhone = ui->PhoneEdit->text().trimmed();
    if(strPhone.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please enter your phone number!");
        return;
    }

    QImage image = m_Image.toImage().convertToFormat(QImage::Format_RGB888);

    int faceCount = 0;
    SFaceRect* pFaceRect = NULL;
    Detect_Faces((unsigned char*)image.bits(), image.bytesPerLine(), image.width(), image.height(), &faceCount, &pFaceRect);

    if(faceCount > 0)
    {
        int feature_len = GetFeaturelen();
        float* pFeature = new float[feature_len];

        int nCenterIdx = 0;
        int nMinDist = 100000;
        for(int i = 0; i < faceCount; i++)
        {
            int nDistfromCenter = (image.width() / 2 - pFaceRect[i].X - pFaceRect[i].Width / 2) * (image.width() / 2 - pFaceRect[i].X - pFaceRect[i].Width / 2) + (image.height() / 2 - pFaceRect[i].Y - pFaceRect[i].Height / 2) * (image.height() / 2 - pFaceRect[i].Y - pFaceRect[i].Height / 2);
            if(nDistfromCenter < nMinDist)
            {
                nCenterIdx = i;
                nMinDist = nDistfromCenter;
            }
        }

        int faceRect[4];
        faceRect[0] = std::max(0, pFaceRect[nCenterIdx].X);
        faceRect[1] = std::min(pFaceRect[nCenterIdx].X + pFaceRect[nCenterIdx].Width, image.width());
        faceRect[2] = std::max(0, pFaceRect[nCenterIdx].Y);
        faceRect[3] = std::min(pFaceRect[nCenterIdx].Y + pFaceRect[nCenterIdx].Height, image.height());
        QRect faceRegion = QRect(faceRect[0] - 20, faceRect[2] - 20, faceRect[1] - faceRect[0] + 40, faceRect[3] - faceRect[2] + 40);
        QImage faceImg = image.copy(faceRegion);

        GetFeatures((unsigned char*)image.bits(), image.width(), image.height(),faceRect, pFaceRect[nCenterIdx].landmarks, pFeature);

        QString filePath = tr("%1/%2_%3.jpg").arg(g_imagepath).arg(strFirst).arg(strLast);
        QString strType = ui->cmb_type->currentText();

        //if(faceImg.save(filePath))
        EncryptImage(faceImg,filePath);
        {
            Person person(0, 0, strFirst, strLast, strCompany, strEmail, strEmail, pFeature, feature_len, filePath,strType, "", "", NULL, NULL);
            DBManagement* db = DBManagement::getInstance();
            db->lockUserTB();
            bool bInsert = db->InsertPerson(person);
            db->unlockUserTB();

            if(bInsert)
            {
                ui->FirstEdit->setText("");
                ui->LastEdit->setText("");
                ui->CompanyEdit->setText("");
                ui->EmailEdit->setText("");
                ui->PhoneEdit->setText("");
                m_Image = QPixmap();
                ui->PhotoLabel->setPixmap(m_Image);
                QMessageBox::information(this, "Success", "Registered successfully!");
            }
        }

        delete[] pFaceRect;
        delete[] pFeature;
    }
    else
    {
        QMessageBox::warning(this, "Warning", "There is no face!");
    }
}
