#ifndef SWITCH_H
#define SWITCH_H

#include <QtWidgets>


class Switch: public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)

public:
    //Switch();
    Switch(QWidget* parent = nullptr);
    Switch(const QBrush& brush, QWidget* parent = nullptr);

    QSize sizeHint() const override;

    QBrush brush() const {
        return _brush;
    }
    void setBrush(const QBrush &brsh) {
        _brush = brsh;
    }

    int offset() const {
        return _x;
    }
    void setOffset(int o) {
        _x = o;
        update();
    }
protected:
    void paintEvent(QPaintEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void enterEvent(QEvent*) override;

private:
    bool _switch;
    qreal _opacity;
    int _x, _y, _height, _margin;
    QBrush _thumb, _track, _brush;
    QPropertyAnimation *_anim = nullptr;

public:
    bool GetSwitch();
    void SetSwitch(bool);


};

#endif // SWITCH_H
