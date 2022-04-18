#include "visitorinfo.h"
#include "ui_visitorinfo.h"
#include <QMessageBox>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QDate>
#include <QTime>

visitorinfo::visitorinfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::visitorinfo)
{
    ui->setupUi(this);
    connect(ui->btnPrinter, SIGNAL(clicked()), this, SLOT(on_clickedPrint()));
}

void visitorinfo::on_clickedPrint()
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

void visitorinfo::print()
{
    on_clickedPrint();
}

void visitorinfo::setVisitorInfo(QImage img, Person* person, QString strLocation)
{
    ui->lbl_name->setText(person->m_strFirst + " " +  person->m_strLast);
    ui->lbl_type->setText(person->m_strType);
    ui->lbl_company->setText("Company: " + person->m_strCompany);
    ui->lbl_person_visiting->setText("Visiting: ");

    QString strTmp = QString("ID: %1").arg(person->m_nId);
    ui->lbl_visitor_id->setText(strTmp);

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    strTmp = time.toString("hh:mmap ") + date.toString("dd-MM-yyyy");
    ui->lbl_date->setText(strTmp);


    QImage scaledImg = img.scaled(ui->lbl_photo->size(), Qt::KeepAspectRatio);
    ui->lbl_photo->setPixmap(QPixmap::fromImage(scaledImg));

    strTmp = QString("Visiting: %1").arg(strLocation);
    ui->lbl_person_visiting->setText(strTmp);
}

visitorinfo::~visitorinfo()
{
    delete ui;
}


