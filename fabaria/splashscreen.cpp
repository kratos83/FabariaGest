#include "splashscreen.h"
#include "defines.h"
#include <QtCore>
#include <QtWidgets>

using namespace Defines;

SplashScreen::SplashScreen(QPixmap image, QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    //Centro la schermata
    QRect reale = qApp->desktop()->screenGeometry();
    int x = (reale.width()-width())/2;
    int y = (reale.height()-height())/2;
    move(x,y);
    img = image;
    interfaccia();
}

void SplashScreen::interfaccia()
{
    gridLayout = new QGridLayout(this);
    labelSplash = new QLabel(this);
    gridLayout->addWidget(labelSplash, 0, 0, 1, 1);
    labelMessage = new QLabel(this);
    gridLayout->addWidget(labelMessage, 1, 0, 1, 1);
    labelSplash->setPixmap(img);
}

void SplashScreen::showMessages(QString message)
{
    labelMessage->setText(message);
}

void SplashScreen::finish(QWidget* window)
{
    if(window->isVisible())
        hide();
}

void SplashScreen::setPixmap(QPixmap images)
{
    labelSplash->setPixmap(images);
}
