#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QProgressBar>
#include <QApplication>
#include <QLoggingCategory>
#include <QtCore>
#include <QtWidgets>

class SplashScreen : public QWidget
{
    Q_OBJECT
public:
    explicit SplashScreen(QPixmap image, QWidget *parent = 0);

public slots:
    void showMessages(QString message);
    void finish(QWidget *window);
    void setPixmap(QPixmap images);
    void interfaccia();
public:
    QPixmap img;
    QGridLayout *gridLayout;
    QLabel *labelSplash;
    QLabel *labelMessage;
};
#endif // SPLASHSCREEN_H
