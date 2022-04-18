#ifndef INSERTDIALOG_H
#define INSERTDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QFileDialog>

namespace Ui {
class InsertDialog;
}

class InsertDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InsertDialog(QWidget *parent = 0);
    ~InsertDialog();

private slots:
    void on_CloseButton_clicked();
    void on_BrowserButton_clicked();

    void on_AddButton_clicked();

private:
    Ui::InsertDialog *ui;

public:
    QPixmap m_Image;
};

#endif // INSERTDIALOG_H
