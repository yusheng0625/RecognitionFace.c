#ifndef SPLASHFORM_H
#define SPLASHFORM_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>


namespace Ui {
class SplashForm;
}

class SplashForm : public QDialog
{
    Q_OBJECT

public:
    explicit SplashForm(QWidget *parent = 0);
    ~SplashForm();
private slots:
    void on_SplashForm_accepted();
    void on_SplashForm_rejected();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::SplashForm *ui;
};

#endif // SPLASHFORM_H
