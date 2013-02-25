#ifndef DISCRETECOLORSELECTOR_H
#define DISCRETECOLORSELECTOR_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QColorDialog>
#include <QEvent>
#include <QPushButton>
#include "combowidget.h"
#include "qcustomplot.h"
#include "colorbutton.h"
#include <QScrollArea>


class DiscreteColorSelector : public QScrollArea
{
    Q_OBJECT
public:
    explicit DiscreteColorSelector(QWidget *parent = 0);
    QColor color(QString state);
    QMap<QString,QColor> states();
    
signals:
    void changed();
    void stateChanged(QString name, QColor color);

public slots:
    void addState(QString name, QColor color);
    void setState(QString name, QColor color);
    void setStates(QMap<QString,QColor> states);
    void removeState(QString name);

private slots:
    void m_setState(QString name, QColor color);
    void m_buttonClicked(QString );


private:
    QSignalMapper * m_mapper;
    QWidget * m_widget;
    QHBoxLayout * m_layout;
    QMap<QString,QColor> m_states;
    QMap<QString, ColorButton *>m_colorButtons;


};

#endif // DISCRETECOLORSELECTOR_H
