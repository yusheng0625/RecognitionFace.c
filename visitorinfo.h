#ifndef VISITORINFO_H
#define VISITORINFO_H

#include <QWidget>
#include <QPixmap>
#include "global.h"

namespace Ui {
class visitorinfo;
}


class visitorinfo : public QWidget
{
    Q_OBJECT

public:
    explicit visitorinfo(QWidget *parent = 0);
    ~visitorinfo();

    void setVisitorInfo(QImage img, Person* person, QString strLocation);
    void print();

private slots:
    void on_clickedPrint();
private:
    Ui::visitorinfo *ui;
};

#endif // VISITORINFO_H
