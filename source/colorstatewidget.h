#ifndef COLORSTATEWIDGET_H
#define COLORSTATEWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QColorDialog>
#include <QEvent>
#include <QPushButton>
#include "combowidget.h"
#include "qcustomplot.h"
#include "orientationbutton.h"
#include <QScrollArea>

struct State
{
    State()
    {
        this->name = "NONE";
        this->color = QColor(0,0,0);
    }

    State(QString name, QColor color)
    {
        this->name = name;
        this->color = color;
    }

    QString name;
    QColor color;
};
class ColorButton : public OrientationButton
    {
    Q_OBJECT

public:
    ColorButton(QWidget *parent = 0)
        :OrientationButton(parent)
    {
        this->setAutoFillBackground(true);
        setColor(QColor(0,0,0));
    }
    QColor color() {return m_color;}
    void setColor(QColor color)
    {
        m_color = color;
        QColor fadeColor = QColor(color.red() / 2, color.green() / 2, color.blue() / 2);

        this->setStyleSheet(" background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(" +  QString::number(color.red()) + ",  "+  QString::number(color.green()) + ", "+  QString::number(color.blue()) +"), \
                            stop: 1 rgb( " +QString::number(fadeColor.red()) + ",  "+  QString::number(fadeColor.green()) + ", "+  QString::number(fadeColor.blue()) +"));\
                            color: rgb( " +QString::number(255-color.red()) + ",  "+  QString::number(255-color.green()) + ", "+  QString::number(255-color.blue()) +");");

        colorChanged( color);
    }
signals:
    void colorChanged(QColor color);
private:
    QString m_text;
    QColor m_color;

};

class ColorStateWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit ColorStateWidget(QWidget *parent = 0);
    QColor color(QString state);
    QVector<State> colors();
    
signals:
    void changed();
    void stateChanged(QString state, QColor color);

public slots:
    void addState(QString state, QColor color);
    void setState(QString state, QColor color);

private slots:
    void m_buttonClicked(QString );


private:
    QSignalMapper * m_mapper;
    QWidget * m_widget;
    QHBoxLayout * m_layout;
    QVector<State> m_states;
    QHash<QString, ColorButton *>m_colorButtons;


};

#endif // COLORSTATEWIDGET_H
