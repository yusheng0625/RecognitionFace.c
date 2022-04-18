#include "thankyouwidget.h"
#include "ui_thankyouwidget.h"
#include <QDebug>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QDate>
#include <QTime>
#include "global.h"
#include <QMessageBox>
#include <QTimer>


ThankyouWidget::ThankyouWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ThankyouWidget)
{
    ui->setupUi(this);
    m_bLiveTimer = false;

    m_nTimerCount = 0;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_Time()));
    m_timer->start(1000);
}
void ThankyouWidget::on_Time()
{
    if(m_bLiveTimer ==false)
        return;

    m_nTimerCount++;
    if(m_nTimerCount >=5)
    {
        m_bLiveTimer =false;
        emit callGotoHome();
    }
}

void ThankyouWidget::ResetTimer()
{
    m_bLiveTimer = true;
    m_nTimerCount = 0;
}


ThankyouWidget::~ThankyouWidget()
{
    m_timer->stop();
    delete m_timer;
    delete ui;
}

void ThankyouWidget::setVisitorInfo(QImage img, Person* person, QString strVisitorType)
{
    ui->lblName->setText(person->m_strFirst + " " +  person->m_strLast + "!");

    ui->lbl_name->setText(person->m_strFirst + " " +  person->m_strLast);
//    ui->lbl_type->setText(person->m_strType);
    ui->lbl_company->setText(person->m_strCompany);

    ui->lbl_type->setText(strVisitorType);

    QString strStaffName = GlobalData::getInstance()->getStaffName(g_nStaffId);
    ui->lbl_person_visiting->setText(strStaffName);

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    ui->lbl_date->setText(date.toString("dd/MM/yyyy"));
    ui->lbl_date_2->setText(time.toString("h:mmap"));



    QImage scaledImg = img.scaled(ui->lbl_photo->size(), Qt::KeepAspectRatio);
    ui->lbl_photo->setPixmap(QPixmap::fromImage(scaledImg));
    ui->lblThanku->setText("THANK YOU " + person->m_strFirst);

    ui->lbl_client->setText(g_strClientName);
}

void ThankyouWidget::on_btnCheckOut_clicked()
{
    m_bLiveTimer = false;
    qDebug() << "ThankyouWidget::Click CheckOut";
    emit callGotoHome();
}

void ThankyouWidget::print()
{
    QString strPrinterName = "Brother QL-720NW";
    //QString strPrinterName = "Brother DCP-7060D";

    QPrinterInfo info;
    bool bFind = false;
    QList<QPrinterInfo> lst = QPrinterInfo::availablePrinters();
    foreach(QPrinterInfo inf , lst)
    {
        QString strName = inf.printerName();
//        QMessageBox::about(this, "Printer", inf.printerName());
        if(strPrinterName == strName)
        {
            bFind = true;
            info = inf;
            break;
            //QMessageBox::about(this, trUtf8("Some text"), "Brother DCP-7060D");

        }
    }
    if(bFind==false)
    {
        QMessageBox::about(this, APP_TITLE, "Can't find Printer");
        return;
    }


    QPrinter printer(info, QPrinter::HighResolution);
//    QMessageBox::about(this, trUtf8("Some text1"), printer.printerName());
    printer.setFullPage(false);

    //printer.setPrinterName("Brother ql720 nw");

    printer.setCopyCount(1);
    printer.setDoubleSidedPrinting(false);
    printer.setDuplex(QPrinter::DuplexNone);
    printer.setColorMode(QPrinter::Color);

    printer.setPaperSize(QSizeF(62, 100), QPrinter::Millimeter);
    printer.setPageSizeMM(QSizeF(62, 100));
    printer.setResolution(g_printerDPI);

//    printer.setPageSize(QPrinter::Letter);
//    printer.setPaperSize(QPrinter::Letter);
    printer.setPaperSource(QPrinter::Auto);
    printer.setCreator("Inkjet Plumber");
    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(4, 4, 4, 4, QPrinter::Millimeter);
//    printer.setPageLayout(page_layout);
    printer.setDocName("Inkjet Plumber Maintenance Job");
    printer.setOutputFormat(QPrinter::NativeFormat);

    //    printer_.setOutputFileName("/Users/jefft/Downloads/ijp.pdf");
    //    printer_.setOutputFormat(QPrinter::PdfFormat);

    if (!printer.isValid())
    {
        QMessageBox::about(this, APP_TITLE, trUtf8("Printer is not valid"));
        return;
    }


    QPainter painter(&printer);
    double width, height;
    width = double(ui->printerForm->width());
    height = double(ui->printerForm->height());

    QRect printerRect =printer.pageRect();

    double xscale = printerRect.width() / width;
    double yscale = printerRect.height() / height;

    double scale = qMin(xscale, yscale);
    painter.translate(0 + printerRect.width()/ 2,
                      0 + printerRect.height()/2);
    painter.scale(scale, scale);
    painter.translate(-width/ 2, -height/ 2);

    ui->printerForm->render(&painter);
}
