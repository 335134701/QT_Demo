#include "analogclock.h"
#include "ui_analogclock.h"

AnalogClock::AnalogClock(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnalogClock)
{
    ui->setupUi(this);
    QTimer *timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
    timer->start(1000);

}

AnalogClock::~AnalogClock()
{
    delete ui;
}
void AnalogClock::paintEvent(QPaintEvent *)
{
    static const QPoint hourHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -40)
    };
    static const QPoint minuteHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -60)
    };
/**
    static const QPoint secondHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -80)
    };
**/
    QColor hourColor(127, 0, 127);
    QColor minuteColor(127, 127, 191);
 //   QColor secondColor(25, 48, 191);

    int side = qMin(width(), height());
    QTime time = QTime::currentTime();
    QPainter painter(this);
    //渲染
    painter.setRenderHint(QPainter::Antialiasing);
    //坐标系转换
    painter.translate(width() / 2, height() / 2);
    //按（sx，sy）缩放坐标系
    painter.scale(side / 200.0, side / 200.0);

    painter.setPen(Qt::NoPen);
    //设置颜色
    painter.setBrush(hourColor);
    painter.save();
    //顺时针旋转坐标系， 给定的角度参数以度为单位
    //根据时间顺时针旋转小时指针
    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    //绘制小时指针
    painter.drawConvexPolygon(hourHand, 3);
    painter.restore();
    painter.setPen(hourColor);
    for (int i = 0; i < 12; ++i) {
        painter.drawLine(82, 0, 96, 0);
        painter.rotate(30.0);
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(minuteColor);
    painter.save();
    painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();
    painter.setPen(minuteColor);
    for (int j = 0; j < 60; ++j) {
        if ((j % 5) != 0)
            painter.drawLine(86, 0, 96, 0);
        painter.rotate(6.0);
    }
/**
    painter.setPen(Qt::NoPen);
    painter.setBrush(secondColor);
    painter.save();
    painter.rotate(time.second());
    painter.drawConvexPolygon(secondHand, 3);
    painter.restore();
    painter.setPen(secondColor);
    for (int z = 0; z < 360; ++z) {
        if ((z % 6) != 0)
        painter.drawLine(90, 0, 96, 0);
        painter.rotate(1);
    }
    **/
}
