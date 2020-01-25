#ifndef TAB_STYLE_H
#define TAB_STYLE_H

#include <QtWidgets>

class Tab_style : public QProxyStyle
{
    Q_OBJECT

public:
    Tab_style();
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                              const QSize &size, const QWidget *widget) const;
    void drawControl(ControlElement element, const QStyleOption *option,
                     QPainter *painter, const QWidget *widget) const;

};

#endif // TAB_STYLE_H
