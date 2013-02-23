#include "attributewidget.h"
#include <QDebug>

AttributeWidget::AttributeWidget(QWidget *parent) :
    QObject(parent)
{
    m_neuronMapper = new QSignalMapper(this);
    m_connectionMapper = new QSignalMapper(this);
    connect(m_neuronMapper,SIGNAL(mapped(const QString& )),this,SLOT(m_neuronColorationChanged(const QString&)));
    connect(m_connectionMapper,SIGNAL(mapped(const QString& )),this,SLOT(m_connectionColorationChanged(const QString&  )));


    m_neuronLinkButton = new QToolButton();
    m_neuronLinkButton->setText("Link");
    m_neuronLinkButton->setToolTip("Set whether neuron and connection colorations should be linked for this attribute.");
    m_connectionLinkButton = new QToolButton();
    m_connectionLinkButton->setText("Link");
    m_connectionLinkButton->setToolTip("Set whether neuron and connection colorations should be linked for this attribute.");
    connect(m_neuronLinkButton,SIGNAL(pressed()),this,SLOT(m_neuronLinkButtonPressed()));
    connect(m_connectionLinkButton,SIGNAL(pressed()),this,SLOT(m_connectionLinkButtonPressed()));

    m_linkedIcon = QIcon(":/assets/linked.png");
    m_unlinkedIcon = QIcon(":/assets/unlinked.png");

    m_neuronSidebar = new ComboWidget();

    m_connectionSidebar = new ComboWidget();

    connect(m_neuronSidebar,SIGNAL(widgetChanged(QString)),this,SLOT(m_currentNeuronAttributeSet(QString)));
    connect(m_connectionSidebar,SIGNAL(widgetChanged(QString)),this,SLOT(m_currentConnectionAttributeSet(QString)));
}

QWidget * AttributeWidget::neuronWidget()
{
    return m_neuronSidebar;
}

QWidget * AttributeWidget::connectionWidget()
{
    return m_connectionSidebar;
}

bool AttributeWidget::m_isSharedAttribute(QString name)
{
    bool sharedAttribute;
    if (m_neuronContinuousWidgets.contains(name)&& m_connectionContinuousWidgets.contains(name))
        sharedAttribute = true;
    else
        sharedAttribute = false;
    if (m_neuronDiscreteWidgets.contains(name)&& m_connectionDiscreteWidgets.contains(name) )

        sharedAttribute = sharedAttribute || true;
    else
        sharedAttribute = sharedAttribute || false;
    return sharedAttribute;
}

bool AttributeWidget::m_continuousAttributeConsistent(QString attributeName)
{
    QVector<QRgb> connData= m_connectionContinuousWidgets[attributeName]->getData();
    QVector<QRgb> neuronData = m_neuronContinuousWidgets[attributeName]->getData();
    bool same = true;
    for (int i = 0; i < connData.count(); i ++)
        if (connData[i] != neuronData[i])
        {
            same = false;
            break;
        }
    return same;
}


bool AttributeWidget::m_discreteAttributeConsistent(QString attributeName)
{
    QMap<QString,QColor> neuronStates =  m_neuronDiscreteWidgets[attributeName]->states();
    QMap<QString,QColor> connectionStates =  m_connectionDiscreteWidgets[attributeName]->states();
    if (neuronStates.count() != connectionStates.count())
        return false;

    for (QMap<QString,QColor>::iterator it = neuronStates.begin(); it != neuronStates.end(); it++)
        if (m_neuronDiscreteWidgets[attributeName]->color(it.key()) != m_connectionDiscreteWidgets[attributeName]->color(it.key()))
           return false;


    return true;
}
void AttributeWidget::m_checkAndFixInconsistantColorations(QString attributeName)
{

    if (m_attributeLinked[attributeName])
    {

        bool isRangeAttrib = m_connectionContinuousWidgets.contains(attributeName) ;
        if ((isRangeAttrib && !m_continuousAttributeConsistent(attributeName)) ||(!isRangeAttrib && !m_discreteAttributeConsistent(attributeName)))
        {
            QMessageBox msgBox;
            msgBox.setText("Attributes being linked have inconsistent coloration.");

            QPushButton *neuronButton = msgBox.addButton(tr("Use Neuron Coloration"), QMessageBox::ActionRole);
            QPushButton *connectionButton = msgBox.addButton(tr("Use Connection Coloration"), QMessageBox::ActionRole);
            QPushButton *cancelLinking = msgBox.addButton(tr("Cancel Linking"), QMessageBox::ActionRole);

            msgBox.exec();

            if (msgBox.clickedButton() == neuronButton)
            {
                if (m_connectionContinuousWidgets.contains(attributeName))
                    m_connectionContinuousWidgets[attributeName]->setMarkers(m_neuronContinuousWidgets[attributeName]->getMarkers());

                else
                {
                    m_connectionDiscreteWidgets[attributeName]->setState("Off",m_neuronDiscreteWidgets[attributeName]->color("Off"));
                    m_connectionDiscreteWidgets[attributeName]->setState("On",m_neuronDiscreteWidgets[attributeName]->color("On"));
                }

            }
            else if (msgBox.clickedButton() == connectionButton)
            {
                if (m_neuronContinuousWidgets.contains(attributeName))
                    m_neuronContinuousWidgets[attributeName]->setMarkers(m_connectionContinuousWidgets[attributeName]->getMarkers());

                else
                {
                    m_neuronDiscreteWidgets[attributeName]->setState("Off",m_connectionDiscreteWidgets[attributeName]->color("Off"));
                    m_neuronDiscreteWidgets[attributeName]->setState("On",m_connectionDiscreteWidgets[attributeName]->color("On"));
                }

            }
            else if (msgBox.clickedButton() == cancelLinking)
                m_attributeLinked[attributeName] = false;
        }
    }
}
void AttributeWidget::m_neuronLinkButtonPressed()
{
    QString attributeName = m_neuronSidebar->currentWidgetName();
    m_attributeLinked[attributeName] = !m_attributeLinked[attributeName];
    m_checkAndFixInconsistantColorations(attributeName);
    m_updateNeuronLinkIcon();
    m_updateConnectionLinkIcon();

}

void AttributeWidget::m_connectionLinkButtonPressed()
{
    QString attributeName = m_connectionSidebar->currentWidgetName();
    m_attributeLinked[attributeName] = !m_attributeLinked[attributeName];

    m_checkAndFixInconsistantColorations(attributeName);
    m_updateNeuronLinkIcon();
    m_updateConnectionLinkIcon();
}


void AttributeWidget::m_updateNeuronLinkIcon()
{
    if (m_attributeLinked[m_neuronSidebar->currentWidgetName()])
        m_neuronLinkButton->setIcon(m_linkedIcon);
    else
        m_neuronLinkButton->setIcon(m_unlinkedIcon);
}

void AttributeWidget::m_updateConnectionLinkIcon()
{
    if (m_attributeLinked[m_connectionSidebar->currentWidgetName()])
        m_connectionLinkButton->setIcon(m_linkedIcon);
    else
        m_connectionLinkButton->setIcon(m_unlinkedIcon);
}

void AttributeWidget::m_currentNeuronAttributeSet(QString name)
{
    currentNeuronAttributeSet(name);
    if (m_attributeShared[name])
    {


        if (!m_neuronSidebar->containsTool(m_neuronLinkButton))
        {
            m_neuronSidebar->addTool(m_neuronLinkButton);
            m_neuronLinkButton->show();
        }
        if ( m_connectionSidebar->currentWidgetName() == name && !m_connectionSidebar->containsTool(m_connectionLinkButton))
        {
            m_connectionSidebar->addTool(m_connectionLinkButton);
            m_connectionLinkButton->show();
        }

        m_updateNeuronLinkIcon();
        m_updateConnectionLinkIcon();
    }
    else if ( m_neuronSidebar->containsTool(m_neuronLinkButton))
    {
        m_neuronSidebar->removeTool(m_neuronLinkButton);
        m_neuronLinkButton->hide();
    }


}


void AttributeWidget::m_currentConnectionAttributeSet(QString name)
{
    currentConnectionAttributeSet(name);
    if (m_attributeShared[name])
    {

        if ( !m_connectionSidebar->containsTool(m_connectionLinkButton))
        {
            m_connectionSidebar->addTool(m_connectionLinkButton);
            m_connectionLinkButton->show();
        }

        if ( m_neuronSidebar->currentWidgetName() == name && !m_neuronSidebar->containsTool(m_neuronLinkButton))
        {
            m_neuronSidebar->addTool(m_neuronLinkButton);
            m_neuronLinkButton->show();
        }

        m_updateNeuronLinkIcon();
        m_updateConnectionLinkIcon();
    }
    else if ( m_connectionSidebar->containsTool(m_connectionLinkButton))
    {
        m_connectionSidebar->removeTool(m_connectionLinkButton);
        m_connectionLinkButton->hide();
    }

}


void AttributeWidget::addContinuousAttribute(QString name, NCVContinuousAttribute *attrib )
{
    if (attrib->elementType() == Neuron)
    {
        m_neuronContinuousAttributes[name] = attrib;
        m_neuronContinuousWidgets[name] = new ContinuousColorSelector();
        if (m_connectionContinuousWidgets.contains(name))
        {
            m_attributeShared[name] = true;
            m_attributeLinked[name] = true;

        }
        else
        {
            m_attributeShared[name] = false;
            m_attributeLinked[name] = false;
        }
        m_neuronContinuousWidgets[name]->setLowThreshold(attrib->minValue());
        m_neuronContinuousWidgets[name]->setHighThreshold(attrib->maxValue());
        m_neuronContinuousWidgets[name]->setWidth(300);
        float step = (attrib->maxValue() -  attrib->minValue()) / 2;
        m_neuronContinuousWidgets[name]->addMarker(attrib->minValue() ,QColor(0,255,0));
        m_neuronContinuousWidgets[name]->addMarker(attrib->minValue() + step,QColor(0,0,255));
        m_neuronContinuousWidgets[name]->addMarker(attrib->minValue() + step*2,QColor(255,0,0));
        m_neuronSidebar->addWidget(m_neuronContinuousWidgets[name],name);
        m_neuronContinuousAttributes[name]->attachColoration(m_neuronContinuousWidgets[name]->getData());

        connect(m_neuronContinuousWidgets[name],SIGNAL(colorRangeChanged()),m_neuronMapper,SLOT(map()));
        m_neuronMapper->setMapping(m_neuronContinuousWidgets[name],name);
    }
    else if(attrib->elementType() == Connection)
    {

        m_connectionContinuousAttributes[name] = attrib;
        m_connectionContinuousWidgets[name] = new ContinuousColorSelector();
        if (m_neuronContinuousWidgets.contains(name))
        {
            m_attributeShared[name] = true;
            m_attributeLinked[name] = true;
        }
        else
        {
            m_attributeShared[name] = false;
            m_attributeLinked[name] = false;
        }

        m_connectionContinuousWidgets[name]->setLowThreshold(attrib->minValue());
        m_connectionContinuousWidgets[name]->setHighThreshold(attrib->maxValue());
        m_connectionContinuousWidgets[name]->setWidth(300);
        float step = (attrib->maxValue() -  attrib->minValue()) / 2;
        m_connectionContinuousWidgets[name]->addMarker(attrib->minValue() ,QColor(0,255,0));
        m_connectionContinuousWidgets[name]->addMarker(attrib->minValue() + step,QColor(0,0,255));
        m_connectionContinuousWidgets[name]->addMarker(attrib->minValue() + step*2,QColor(255,0,0));
        m_connectionSidebar->addWidget(m_connectionContinuousWidgets[name],name);
        m_connectionContinuousAttributes[name]->attachColoration(m_connectionContinuousWidgets[name]->getData());

        connect(m_connectionContinuousWidgets[name],SIGNAL(colorRangeChanged()),m_connectionMapper,SLOT(map()));
        m_connectionMapper->setMapping(m_connectionContinuousWidgets[name],name);
    }
}

void AttributeWidget::addDiscreteAttribute(QString name, NCVDiscreteAttribute * attrib)
{
    if (attrib->elementType() == Neuron)
    {

        m_neuronDiscreteAttributes[name] = attrib;
        m_neuronDiscreteWidgets[name] = new DiscreteColorSelector();
        if (m_connectionDiscreteWidgets.contains(name))
        {
            m_attributeShared[name] = true;
            m_attributeLinked[name] = true;
        }
        else
        {
            m_attributeShared[name] = false;
            m_attributeLinked[name] = false;
        }

        connect(m_neuronDiscreteWidgets[name],SIGNAL(changed()),m_neuronMapper,SLOT(map()));
        m_neuronMapper->setMapping(m_neuronDiscreteWidgets[name],name);


        QMap<QString,QColor> states = attrib->coloration();

        for (QMap<QString,QColor>::iterator it = states.begin(); it != states.end(); it++)
            m_neuronDiscreteWidgets[name]->addState(it.key(),it.value());

        m_neuronSidebar->addWidget(m_neuronDiscreteWidgets[name],name);

    }
    else if(attrib->elementType() == Connection)
    {

        m_connectionDiscreteAttributes[name] = attrib;
        m_connectionDiscreteWidgets[name] = new DiscreteColorSelector();
        if (m_neuronDiscreteWidgets.contains(name))
        {
            m_attributeShared[name] = true;
            m_attributeLinked[name] = true;
        }
        else
        {
            m_attributeShared[name] = false;
            m_attributeLinked[name] = false;
        }
        connect(m_connectionDiscreteWidgets[name],SIGNAL(changed()),m_connectionMapper,SLOT(map()));
        m_connectionMapper->setMapping(m_connectionDiscreteWidgets[name],name);


        QMap<QString,QColor> states = attrib->coloration();
        for (QMap<QString,QColor>::iterator it = states.begin(); it != states.end(); it++)
            m_connectionDiscreteWidgets[name]->addState(it.key(),it.value());

        m_connectionSidebar->addWidget(m_connectionDiscreteWidgets[name],name);
    }
}


void AttributeWidget::m_neuronColorationChanged(const QString & attributeName)
{

    if (m_neuronDiscreteWidgets.contains(attributeName))
    {
        QMap<QString,QColor> states =  m_neuronDiscreteWidgets[attributeName]->states();
        m_neuronDiscreteAttributes[attributeName]->attachColoration(states);
        if (m_neuronSidebar->containsTool(m_neuronLinkButton) && m_neuronLinkButton->icon().cacheKey() == m_linkedIcon.cacheKey() )
        {

            if (m_connectionSidebar->containsWidget(attributeName)  && m_connectionDiscreteWidgets.contains(attributeName) && !m_discreteAttributeConsistent(attributeName))
            {

                 for (QMap<QString,QColor>::iterator it = states.begin(); it != states.end(); it++)
                     if (m_neuronDiscreteWidgets[attributeName]->color(it.key()) != m_connectionDiscreteWidgets[attributeName]->color(it.key()))
                         m_connectionDiscreteWidgets[attributeName]->setState(it.key(),m_neuronDiscreteWidgets[attributeName]->color(it.key()));

            }

        }

    }
    else
    {

        m_neuronContinuousAttributes[attributeName]->attachColoration(m_neuronContinuousWidgets[attributeName]->getData());
        if(m_neuronContinuousWidgets.contains(attributeName))
        {
            if ( m_attributeLinked[attributeName] && m_connectionContinuousWidgets.contains(attributeName))
                m_connectionContinuousWidgets[attributeName]->setMarkers(m_neuronContinuousWidgets[attributeName]->getMarkers());


        }
        else
        {
            if ( m_attributeLinked[attributeName] && m_connectionContinuousWidgets.contains(attributeName))
                m_connectionContinuousWidgets[attributeName]->setMarkers(m_neuronContinuousWidgets[attributeName]->getMarkers());
        }
    }


}

void AttributeWidget::m_connectionColorationChanged(const QString & attributeName)
{
    if (m_connectionDiscreteWidgets.contains(attributeName))
    {
        QMap<QString,QColor> states =  m_connectionDiscreteWidgets[attributeName]->states();
        m_connectionDiscreteAttributes[attributeName]->attachColoration(states);
        if (m_connectionSidebar->containsTool(m_connectionLinkButton)&&  m_connectionLinkButton->icon().cacheKey() == m_linkedIcon.cacheKey() )
        {
            if (m_neuronSidebar->containsWidget(attributeName) && m_neuronDiscreteWidgets.contains(attributeName) && !m_discreteAttributeConsistent(attributeName))
            {
                QMap<QString,QColor> states =  m_neuronDiscreteWidgets[attributeName]->states();
                for (QMap<QString,QColor>::iterator it = states.begin(); it != states.end(); it++)
                    if (m_neuronDiscreteWidgets[attributeName]->color(it.key()) != m_connectionDiscreteWidgets[attributeName]->color(it.key()))
                        m_neuronDiscreteWidgets[attributeName]->setState(it.key(),m_connectionDiscreteWidgets[attributeName]->color(it.key()));

            }

        }

    }
    else
    {
        m_connectionContinuousAttributes[attributeName]->attachColoration(m_connectionContinuousWidgets[attributeName]->getData());
        if(m_connectionContinuousWidgets.contains(attributeName))
        {
            if ( m_attributeLinked[attributeName] && m_neuronContinuousWidgets.contains(attributeName))
                    m_neuronContinuousWidgets[attributeName]->setMarkers(m_connectionContinuousWidgets[attributeName]->getMarkers());


        }
        else
        {
            if ( m_attributeLinked[attributeName] && m_neuronContinuousWidgets.contains(attributeName))
                m_neuronContinuousWidgets[attributeName]->setMarkers(m_connectionContinuousWidgets[attributeName]->getMarkers());
        }

    }

}




