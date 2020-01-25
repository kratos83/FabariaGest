#include <QtGui/QtGui>

#include "analogclock.h"

AnalogClock::AnalogClock(QWidget *parent)
    : QWidget(parent)
{

    QTimer *timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    timer->start(1000);

    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(QImage(":/images/clock-night.png")));
    setPalette(palette);
    resize(240, 240);
}

void AnalogClock::paintEvent(QPaintEvent *)

{

    static const int hourHand[8] = { -2, 18, 2, 18, 2, -60, -2, -60 };
    static const int minuteHand[8] = { -2, 28, 2, 28, 2, -70, -2, -70 };
    static const int secondHand[12] = {-1, 0, -1, -70, 1,-70, 1, 0, 4, 35, -4, 35};

    QColor hourColor(127, 0, 127);
    QColor minuteColor(0, 127, 127, 191);
    QColor secondColor(255,0,0);

    int side = qMin(width(), height());
    QTime time = QTime::currentTime();

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);

    painter.scale(side / 240.0, side / 240.0);

    painter.setPen(Qt::NoPen);

    painter.setBrush(hourColor);

    painter.save();

    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    painter.drawConvexPolygon(QPolygon(4, hourHand));
    painter.restore();

    for (int i = 0; i < 12; ++i) {
        painter.drawEllipse(84, -2, 4, 4);
        painter.rotate(30.0);
    }

    painter.setPen(hourColor);

    painter.setPen(Qt::NoPen);

    painter.setBrush(minuteColor);

    painter.save();
    painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter.drawConvexPolygon(QPolygon(4, minuteHand));
    painter.restore();

    for (int j = 0; j < 60; ++j) {
        if ((j % 5) != 0)
            painter.drawLine(94, 0, 97, 0);
        painter.rotate(6.0);
    }

    painter.setPen(minuteColor);

    painter.setPen(Qt::NoPen);
    painter.setBrush(secondColor);

    painter.save();
    painter.rotate(6.0 * (time.second()));
    painter.drawConvexPolygon(QPolygon(6, secondHand));
    painter.restore();

    painter.setPen(secondColor);

    painter.drawEllipse(-5,-5,10,10);

}
