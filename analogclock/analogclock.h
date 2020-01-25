#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <QWidget>
#include <QDir>
#include <QSettings>


//! [0]
class AnalogClock : public QWidget
{
    Q_OBJECT


public:
    AnalogClock(QWidget *parent = 0);


protected:
    void paintEvent(QPaintEvent *event);
};
//! [0]

#endif
