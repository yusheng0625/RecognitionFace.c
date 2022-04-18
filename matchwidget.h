#ifndef MATCHWIDGET_H
#define MATCHWIDGET_H

#include <QWidget>

namespace Ui {
class MatchWidget;
}

class MatchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatchWidget(QWidget *parent = 0);
    ~MatchWidget();

private slots:
    void on_btnContinue_clicked();

    void on_btnGoBack_clicked();

    void on_btn_notYou_clicked();
signals:
    void clickedContinue(int nPersonId, QImage faceImg, int staffId);
    void clickedContinueSecondaryScore(int nPersonId, QImage faceImg, int staffId);
    void clickedGoBack();

private:
    Ui::MatchWidget *ui;
    QString m_strFirst;
    QString m_strLast;
    int     m_nStaffId;
public:
    int m_nPersonId;
    QImage m_faceImage;
    void SetData(int nPersonId, int nHistId, QImage faceImg, int nSessionId, float score, int staffId);
    void timerEvent(QTimerEvent* event);
    bool m_bPrimaryThresoldMatched;

};

#endif // MATCHWIDGET_H
