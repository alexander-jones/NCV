#include "qwidgetvector.h"

QWidgetVector::QWidgetVector(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    this->setLayout(m_layout);
}

void QWidgetVector::addWidget(QWidget *widget)
{
    m_layout->addWidget(widget);
}
bool QWidgetVector::containsWidget(QWidget *widget)
{
    return m_layout->indexOf(widget) == -1;
}
void QWidgetVector::insertWidget(int index,QWidget *widget)
{
    m_layout->insertWidget(index,widget);
}
int QWidgetVector::indexOfWidget(QWidget *widget)
{
    return m_layout->indexOf(widget);
}
void QWidgetVector::removeWidget(QWidget *widget)
{
    m_layout->removeWidget(widget);
}
void QWidgetVector::setDirection(QBoxLayout::Direction direction)
{
    m_layout->setDirection(direction);
}

void QWidgetVector::setAlignment(Qt::Alignment alignment)
{
    m_layout->setAlignment(alignment);
}
