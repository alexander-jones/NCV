#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include "orientationbutton.h"

class ColorButton : public OrientationButton
    {
    Q_OBJECT

public:
    ColorButton(QWidget *parent = 0)
        :OrientationButton(parent)
    {
        this->setAutoFillBackground(true);
    }
    QColor color() {return m_color;}
    void setColor(QColor color, QColor textColor)
    {
        QColor darkColor = QColor(color.red() / 2, color.green() / 2, color.blue() / 2);
        m_color = color;
        m_textColor = textColor;

        this->setStyleSheet(" QPushButton:enabled{ background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(" +  QString::number(color.red()) + ",  "+  QString::number(color.green()) + ", "+  QString::number(color.blue()) +"), \
                            stop: 1 rgb( " +QString::number(darkColor.red()) + ",  "+  QString::number(darkColor.green()) + ", "+  QString::number(darkColor.blue()) +"));\
                            color: rgb( " +QString::number(textColor.red()) + ",  "+  QString::number(textColor.green()) + ", "+  QString::number(textColor.blue()) +"); } " );

        colorChanged( color);
    }
signals:
    void colorChanged(QColor color);

private:

    QColor m_textColor;
    QString m_text;
    QColor m_color,m_currentColor;

};

#endif // COLORBUTTON_H
