#include "discretecolorselector.h"
#include <QPainter>

DiscreteColorSelector::DiscreteColorSelector(QWidget *parent) :
    QScrollArea(parent)
{
    m_widget = new QWidget();
    m_mapper = new QSignalMapper(this);
    m_layout = new QHBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);

    connect(m_mapper,SIGNAL(mapped(QString )),this,SLOT(m_buttonClicked(QString )));
    m_widget->setLayout(m_layout);
    m_widget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWidget(m_widget);
    this->setWidgetResizable(true);
    this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    this->setFrameStyle(QFrame::NoFrame);
}

void DiscreteColorSelector::addState(QString name, QColor color)
{
    m_colorButtons[name] = new ColorButton();
    m_colorButtons[name]->setColor(color);
    m_colorButtons[name]->setText(name);

    m_states[name] = color;

    m_layout->addWidget(m_colorButtons[name]);

    connect(m_colorButtons[name],SIGNAL(clicked()),m_mapper,SLOT(map()));
    m_mapper->setMapping(m_colorButtons[name],name);

}

void DiscreteColorSelector::setState(QString name, QColor color)
{
    m_colorButtons[name]->setColor(color);

    m_states[name] = color;
    stateChanged(name,color);
    changed();
}



void DiscreteColorSelector::removeState(QString name)
{
    if (m_states.contains(name))
    {
        m_states.remove(name);
        m_colorButtons.remove(name);
    }
}

QMap<QString,QColor> DiscreteColorSelector::states()
{
    return m_states;
}

QColor DiscreteColorSelector::color(QString name)
{

    return m_states[name];
}

void DiscreteColorSelector::m_buttonClicked(QString name)
{

    QColorDialog colorPicker;
    colorPicker.setWindowTitle("Select " + name + " Color");
    colorPicker.exec();
    QColor col = colorPicker.selectedColor();

    if (col.isValid())
        this->setState(name,col);

}
