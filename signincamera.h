#ifndef SIGNINCAMERA_H
#define SIGNINCAMERA_H

#include <QWidget>
#include <QTimerEvent>
#include "global.h"

namespace Ui {
class SignInCamera;
}

class SignInCamera : public QWidget
{
    Q_OBJECT

public:
    explicit SignInCamera(QWidget *parent = 0);
    ~SignInCamera();
    void updatedAppMode();
private:
    Ui::SignInCamera *ui;
    void timerEvent(QTimerEvent *);
signals:
    void captureProcessed(int status, QImage* img, int nID, int sessionId, int historyId, int staffId, PersonResult* personResult, float score);

};

#endif // SIGNINCAMERA_H
