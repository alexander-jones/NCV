#include "colorstatewidget.h"
#include <QPainter>

ColorStateWidget::ColorStateWidget(QWidget *parent) :
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

void ColorStateWidget::addState(QString state, QColor color)
{
    m_colorButtons[state] = new ColorButton();
    m_colorButtons[state]->setColor(color);
    m_colorButtons[state]->setText(state);

    m_states.push_back(State(state,color));

    m_layout->addWidget(m_colorButtons[state]);

    connect(m_colorButtons[state],SIGNAL(clicked()),m_mapper,SLOT(map()));
    m_mapper->setMapping(m_colorButtons[state],state);

}
void ColorStateWidget::setState(QString state, QColor color)
{
    m_colorButtons[state]->setColor(color);
    for (int i = 0 ; i < m_states.count(); i ++)
        if (m_states[i].name == state)
            m_states[i].color = color;
    stateChanged(state,color);
    changed();
}



QVector<State> ColorStateWidget::colors()
{
    return m_states;
}

QColor ColorStateWidget::color(QString state)
{
    return m_colorButtons[state]->color();
}

void ColorStateWidget::m_buttonClicked(QString state)
{

    QColorDialog colorPicker;
    colorPicker.setWindowTitle("Select " + state + " Color");
    colorPicker.exec();
    QColor col = colorPicker.selectedColor();

    if (col.isValid())
        this->setState(state,col);

}
