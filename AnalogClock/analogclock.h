#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QTime>

namespace Ui {
class AnalogClock;
}

class AnalogClock : public QWidget
{
    Q_OBJECT

public:
    explicit AnalogClock(QWidget *parent = 0);
    ~AnalogClock();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::AnalogClock *ui;
};

#endif // ANALOGCLOCK_H
