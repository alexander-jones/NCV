#include "discretecolorselector.h"

DiscreteColorSelector::DiscreteColorSelector(QWidget *parent) :
    QScrollArea(parent)
{
    m_widget = new QWidget();
    m_mapper = new QSignalMapper(this);
    m_layout = new QHBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setContentsMargins(0,0,0,0);

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
    m_colorButtons[name]->setText(name);

    m_layout->addWidget(m_colorButtons[name]);

    connect(m_colorButtons[name],SIGNAL(clicked()),m_mapper,SLOT(map()));
    m_mapper->setMapping(m_colorButtons[name],name);

    m_setState(name,color);

}

void DiscreteColorSelector::setStates(QMap<QString,QColor> states)
{
    for (QMap<QString, QColor>::iterator it = states.begin(); it != states.end(); it ++)
    {
        m_setState(it.key(),it.value());
    }
    changed();
}

void DiscreteColorSelector::setState(QString name, QColor color)
{
    m_setState(name,color);
    stateChanged(name,color);
    changed();
}

void DiscreteColorSelector::m_setState(QString name, QColor color)
{
    float brightness = color.redF() *0.299 + color.greenF()*0.587 + color.blueF()*0.114;
    QColor textColor;
    if (brightness > 0.5)
        textColor = QColor(0,0,0);
    else
        textColor = QColor(255,255,255);
    m_colorButtons[name]->setColor(color,textColor);
    m_states[name] = color;
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
