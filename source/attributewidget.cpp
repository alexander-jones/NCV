#include "attributewidget.h"
#include <QDebug>

AttributeWidget::AttributeWidget(QString text,QWidget *parent) :
    CollapsibleWidget(NULL,text,parent)
{
    m_widget = new QWidget();
    m_layout = new QVBoxLayout();
    m_neuronSidebar = new Sidebar();
    m_layout->addWidget(m_neuronSidebar);
    m_connectionSidebar = new Sidebar();
    m_layout->addWidget(m_connectionSidebar);
    m_widget->setLayout(m_layout);
    this->setWidget(m_widget);
    connect(m_neuronSidebar,SIGNAL(panelChanged(QString)),this,SIGNAL(currentNeuronAttributeSet(QString)));
    connect(m_connectionSidebar,SIGNAL(panelChanged(QString)),this,SIGNAL(currentConnectionAttributeSet(QString)));
}

void AttributeWidget::addNeuronRangeAttribute(QString name, float minVal, float maxVal)
{
    ColorRangeWidget *newWidget = new ColorRangeWidget();
    connect(newWidget,SIGNAL(colorRangeChanged(QRgb*)),this,SIGNAL(neuronRangeColorationChanged(QRgb*)));
    newWidget->setTitle("Neuron Range Coloration:");
    newWidget->setImageSize(300,100);
    newWidget->setLowThreshold(minVal);
    newWidget->setHighThreshold(maxVal);
    newWidget->setRangeToGradient(minVal,maxVal,QColor(0,0,0),QColor(255,255,255));
    m_neuronSidebar->addPanel(newWidget,name);
}

void AttributeWidget::addNeuronBitAttribute(QString name, QColor offColor, QColor onColor)
{
    ColorBitWidget* newWidget = new ColorBitWidget();
    connect(newWidget,SIGNAL(colorsChanged(QColor,QColor)),this,SIGNAL(neuronBitColorationChanged(QColor,QColor)));
    newWidget->setOffColor(offColor);
    newWidget->setOnColor(onColor);
    m_neuronSidebar->addPanel(newWidget,name);
}

void AttributeWidget::addConnectionRangeAttribute(QString name, float minVal, float maxVal)
{
    ColorRangeWidget *newWidget = new ColorRangeWidget();
    connect(newWidget,SIGNAL(colorRangeChanged(QRgb*)),this,SIGNAL(connectionRangeColorationChanged(QRgb*)));

    newWidget->setTitle("Connection Range Coloration:");
    newWidget->setImageSize(300,100);
    newWidget->setLowThreshold(minVal);
    newWidget->setHighThreshold(maxVal);
    newWidget->setRangeToGradient(minVal,maxVal,QColor(0,0,0),QColor(255,255,255));
    m_connectionSidebar->addPanel(newWidget,name);

    qDebug() << "recieved";
}
void AttributeWidget::addConnectionBitAttribute(QString name, QColor offColor, QColor onColor)
{
    ColorBitWidget* newWidget = new ColorBitWidget();
    connect(newWidget,SIGNAL(colorsChanged(QColor,QColor)),this,SIGNAL(connectionBitColorationChanged(QColor,QColor)));
    newWidget->setOffColor(offColor);
    newWidget->setOnColor(onColor);
    m_connectionSidebar->addPanel(newWidget,name);
}

