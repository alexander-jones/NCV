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
        QColor fadeColor = QColor(color.red() / 2, color.green() / 2, color.blue() / 2);
        m_color = color;
        m_textColor = textColor;

        this->setStyleSheet(" background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(" +  QString::number(color.red()) + ",  "+  QString::number(color.green()) + ", "+  QString::number(color.blue()) +"), \
                            stop: 1 rgb( " +QString::number(fadeColor.red()) + ",  "+  QString::number(fadeColor.green()) + ", "+  QString::number(fadeColor.blue()) +"));\
                            color: rgb( " +QString::number(textColor.red()) + ",  "+  QString::number(textColor.green()) + ", "+  QString::number(textColor.blue()) +");");

        colorChanged( color);
    }
    void setEnabled(bool enabled)
    {
        OrientationButton::setEnabled(enabled);
    }

signals:
    void colorChanged(QColor color);

private:

    QColor m_textColor;
    QString m_text;
    QColor m_color;

};

#endif // COLORBUTTON_H
