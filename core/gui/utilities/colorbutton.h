#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include "orientationbutton.h"

class ColorButton : public OrientationButton
{
    Q_OBJECT

public:
    ColorButton(QWidget *parent = 0);
    QColor color();
    void setColor(QColor color, QColor textColor);
signals:
    void colorChanged(QColor color);

private:

    QColor m_textColor;
    QString m_text;
    QColor m_color,m_currentColor;

};

#endif // COLORBUTTON_H
