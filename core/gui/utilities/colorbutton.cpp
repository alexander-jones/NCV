#include "colorbutton.h"

ColorButton::ColorButton(QWidget *parent )
    :OrientationButton(parent)
{
    this->setAutoFillBackground(true);
}

QColor ColorButton::color()
{
    return m_color;
}

void ColorButton::setColor(QColor color, QColor textColor)
{
    QColor darkColor = QColor(color.red() / 2, color.green() / 2, color.blue() / 2);
    m_color = color;
    m_textColor = textColor;

    this->setStyleSheet(" QPushButton:enabled{ background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(" +  QString::number(color.red()) + ",  "+  QString::number(color.green()) + ", "+  QString::number(color.blue()) +"), \
                        stop: 1 rgb( " +QString::number(darkColor.red()) + ",  "+  QString::number(darkColor.green()) + ", "+  QString::number(darkColor.blue()) +"));\
                        color: rgb( " +QString::number(textColor.red()) + ",  "+  QString::number(textColor.green()) + ", "+  QString::number(textColor.blue()) +"); } " );

    colorChanged( color);
}


