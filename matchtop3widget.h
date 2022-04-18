#ifndef MATCHTOP3WIDGET_H
#define MATCHTOP3WIDGET_H

#include <QWidget>

namespace Ui {
class matchTop3Widget;
}

class matchTop3Widget : public QWidget
{
    Q_OBJECT

public:
    explicit matchTop3Widget(QWidget *parent = 0);
    ~matchTop3Widget();

private slots:
    void on_btn_top_match2_clicked();

    void on_btn_top_match3_clicked();

    void on_btn_nether_clicked();

signals:
    void clickedContinue(int nPersonId, QImage faceImg, int staffId);
    void clickedGoBack();
private:
    Ui::matchTop3Widget *ui;

    QString m_strFirst;
    QString m_strLast;
    int m_nStaffId;
public:
    int m_nPersonId;
    QImage m_faceImage;
    void SetData(int nPersonId, QImage faceImg, int staffId);
    void timerEvent(QTimerEvent* event);
};

#endif // MATCHTOP3WIDGET_H
