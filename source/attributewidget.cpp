#include "attributewidget.h"
#include <QDebug>

AttributeWidget::AttributeWidget(QString text,QWidget *parent) :
    QObject(parent)
{
    m_widget = new QWidget();
    m_neuronMapper = new QSignalMapper(this);
    m_connectionMapper = new QSignalMapper(this);



    m_neuronLinkButton = new QToolButton();
    m_neuronLinkButton->setToolTip("Set whether neuron and connection colorations should be linked for this attribute.");
    m_connectionLinkButton = new QToolButton();
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
    if (m_neuronRangeWidgets.contains(name)&& m_connectionRangeWidgets.contains(name))
        sharedAttribute = true;
    else
        sharedAttribute = false;
    if (m_neuronFlagWidgets.contains(name)&& m_connectionFlagWidgets.contains(name) )

        sharedAttribute = sharedAttribute || true;
    else
        sharedAttribute = sharedAttribute || false;
    return sharedAttribute;
}

bool AttributeWidget::m_rangeAttributeConsistent(QString attributeName)
{
    QRgb * connData = m_connectionRangeWidgets[attributeName]->getData();
    QRgb * neuronData = m_neuronRangeWidgets[attributeName]->getData();
    bool same = true;
    for (int i = 0; i < 250; i ++)
        if (connData[i] != neuronData[i])
        {
            same = false;
            break;
        }
    return same;
}

bool AttributeWidget::m_flagAttributeConsistent(QString attributeName)
{
    return m_connectionFlagWidgets[attributeName]->getOnColor() ==m_neuronFlagWidgets[attributeName]->getOnColor()
                    && m_connectionFlagWidgets[attributeName]->getOffColor() ==m_neuronFlagWidgets[attributeName]->getOffColor();
}
void AttributeWidget::m_checkAndFixInconsistantColorations(QString attributeName)
{

    if (m_attributeLinked[attributeName])
    {

        bool isRangeAttrib = m_connectionRangeWidgets.contains(attributeName) ;
        if ((isRangeAttrib && !m_rangeAttributeConsistent(attributeName)) ||(!isRangeAttrib && !m_flagAttributeConsistent(attributeName)))
        {
            QMessageBox msgBox;
            msgBox.setText("Attributes being linked have inconsistent coloration.");

            QPushButton *neuronButton = msgBox.addButton(tr("Use Neuron Coloration"), QMessageBox::ActionRole);
            QPushButton *connectionButton = msgBox.addButton(tr("Use Connection Coloration"), QMessageBox::ActionRole);
            QPushButton *cancelLinking = msgBox.addButton(tr("Cancel Linking"), QMessageBox::ActionRole);

            msgBox.exec();

            if (msgBox.clickedButton() == neuronButton)
            {
                if (m_connectionRangeWidgets.contains(attributeName))
                    m_connectionRangeWidgets[attributeName]->setMarkers(m_neuronRangeWidgets[attributeName]->getMarkers());

                else
                    m_connectionFlagWidgets[attributeName]->setColors(m_neuronFlagWidgets[attributeName]->getOffColor(),m_neuronFlagWidgets[attributeName]->getOnColor());

            }
            else if (msgBox.clickedButton() == connectionButton)
            {
                if (m_neuronRangeWidgets.contains(attributeName))
                    m_neuronRangeWidgets[attributeName]->setMarkers(m_connectionRangeWidgets[attributeName]->getMarkers());

                else
                    m_neuronFlagWidgets[attributeName]->setColors(m_connectionFlagWidgets[attributeName]->getOffColor(),m_connectionFlagWidgets[attributeName]->getOnColor());

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
            m_neuronSidebar->addTool(m_neuronLinkButton);
        if ( m_connectionSidebar->currentWidgetName() == name && !m_connectionSidebar->containsTool(m_connectionLinkButton))
            m_connectionSidebar->addTool(m_connectionLinkButton);

        m_updateNeuronLinkIcon();
        m_updateConnectionLinkIcon();
    }
    else if ( m_connectionSidebar->containsTool(m_neuronLinkButton))
            m_neuronSidebar->removeTool(m_neuronLinkButton);

}


void AttributeWidget::m_currentConnectionAttributeSet(QString name)
{
    currentConnectionAttributeSet(name);
    if (m_attributeShared[name])
    {

        if ( !m_connectionSidebar->containsTool(m_connectionLinkButton))
            m_connectionSidebar->addTool(m_connectionLinkButton);

        if ( m_neuronSidebar->currentWidgetName() == name && !m_neuronSidebar->containsTool(m_neuronLinkButton))
            m_neuronSidebar->addTool(m_neuronLinkButton);

        m_updateNeuronLinkIcon();
        m_updateConnectionLinkIcon();
    }
    else if ( m_connectionSidebar->containsTool(m_connectionLinkButton))
            m_connectionSidebar->removeTool(m_connectionLinkButton);

}


void AttributeWidget::addNeuronRangeAttribute(QString name, float minVal, float maxVal)
{
    m_neuronRangeWidgets[name] = new ColorRangeWidget();
    if (m_connectionRangeWidgets.contains(name))
    {
        m_attributeShared[name] = true;
        m_attributeLinked[name] = true;

    }

    m_neuronRangeWidgets[name]->setLowThreshold(minVal);
    m_neuronRangeWidgets[name]->setHighThreshold(maxVal);
    m_neuronRangeWidgets[name]->setImageSize(300,25);
    m_neuronRangeWidgets[name]->setRangeToGradient(minVal,maxVal,QColor(0,255,0),QColor(255,0,255));
    m_neuronSidebar->addWidget(m_neuronRangeWidgets[name],name);
    neuronRangeColorationChanged(name,m_neuronRangeWidgets[name]->getData(),m_neuronRangeWidgets[name]->getImageSize().width());


    connect(m_neuronRangeWidgets[name],SIGNAL(colorRangeChanged()),m_neuronMapper,SLOT(map()));
    m_neuronMapper->setMapping(m_neuronRangeWidgets[name],name);
    connect(m_neuronMapper,SIGNAL(mapped(const QString& )),this,SLOT(m_neuronColorationChanged(const QString& )));

}

void AttributeWidget::addNeuronBitAttribute(QString name, QColor offColor, QColor onColor)
{
    m_neuronFlagWidgets[name] = new ColorBitWidget();
    if (m_connectionFlagWidgets.contains(name))
    {
        m_attributeShared[name] = true;
        m_attributeLinked[name] = true;
    }

    connect(m_neuronFlagWidgets[name],SIGNAL(colorsChanged(QColor,QColor)),m_neuronMapper,SLOT(map()));
    m_neuronMapper->setMapping(m_neuronFlagWidgets[name],name);
    connect(m_neuronMapper,SIGNAL(mapped(const QString& )),this,SLOT(m_neuronColorationChanged(const QString&)));

    m_neuronFlagWidgets[name]->setOffColor(offColor);
    m_neuronFlagWidgets[name]->setOnColor(onColor);
    m_neuronSidebar->addWidget(m_neuronFlagWidgets[name],name);
}

void AttributeWidget::addConnectionRangeAttribute(QString name, float minVal, float maxVal)
{
    m_connectionRangeWidgets[name] = new ColorRangeWidget();
    if (m_neuronRangeWidgets.contains(name))
    {
        m_attributeShared[name] = true;
        m_attributeLinked[name] = true;
    }


    m_connectionRangeWidgets[name]->setLowThreshold(minVal);
    m_connectionRangeWidgets[name]->setHighThreshold(maxVal);
    m_connectionRangeWidgets[name]->setImageSize(300,25);
    m_connectionRangeWidgets[name]->setRangeToGradient(minVal,maxVal,QColor(0,255,0),QColor(255,0,255));
    m_connectionSidebar->addWidget(m_connectionRangeWidgets[name],name);
    connectionRangeColorationChanged(name,m_connectionRangeWidgets[name]->getData(),m_connectionRangeWidgets[name]->getImageSize().width());


    connect(m_connectionRangeWidgets[name],SIGNAL(colorRangeChanged()),m_connectionMapper,SLOT(map()));
    m_connectionMapper->setMapping(m_connectionRangeWidgets[name],name);
    connect(m_connectionMapper,SIGNAL(mapped(const QString& )),this,SLOT(m_connectionColorationChanged(const QString&  )));
}

void AttributeWidget::addConnectionBitAttribute(QString name, QColor offColor, QColor onColor)
{
    m_connectionFlagWidgets[name] = new ColorBitWidget();
    if (m_neuronFlagWidgets.contains(name))
    {
        m_attributeShared[name] = true;
        m_attributeLinked[name] = true;
    }

    connect(m_connectionFlagWidgets[name],SIGNAL(colorsChanged(QColor,QColor)),m_connectionMapper,SLOT(map()));
    m_connectionMapper->setMapping(m_connectionFlagWidgets[name],name);
    connect(m_connectionMapper,SIGNAL(mapped(const QString& )),this,SLOT(m_connectionColorationChanged(const QString& )));

    m_connectionFlagWidgets[name]->setOffColor(offColor);
    m_connectionFlagWidgets[name]->setOnColor(onColor);
    m_connectionSidebar->addWidget(m_connectionFlagWidgets[name],name);
}

void AttributeWidget::m_neuronColorationChanged(const QString & attributeName)
{

    if (m_neuronFlagWidgets.contains(attributeName))
    {
        if (m_neuronSidebar->containsTool(m_neuronLinkButton) && m_neuronLinkButton->icon().cacheKey() == m_linkedIcon.cacheKey() )
        {

            if (m_connectionSidebar->containsWidget(attributeName)  && m_connectionFlagWidgets.contains(attributeName) && !m_flagAttributeConsistent(attributeName))
                    m_connectionFlagWidgets[attributeName]->setColors(m_neuronFlagWidgets[attributeName]->getOffColor(),m_neuronFlagWidgets[attributeName]->getOnColor());

        }
        neuronBitColorationChanged(attributeName, m_neuronFlagWidgets[attributeName]->getOffColor(),m_neuronFlagWidgets[attributeName]->getOnColor());

    }
    else
    {
        if(m_neuronRangeWidgets.contains(attributeName))
        {
            if ( m_attributeLinked[attributeName] && m_connectionRangeWidgets.contains(attributeName))
                m_connectionRangeWidgets[attributeName]->setMarkers(m_neuronRangeWidgets[attributeName]->getMarkers());


            neuronRangeColorationChanged(attributeName,m_neuronRangeWidgets[attributeName]->getData(),m_neuronRangeWidgets[attributeName]->getImageSize().width());
        }
        else
        {
            if ( m_attributeLinked[attributeName] && m_connectionRangeWidgets.contains(attributeName))
                m_connectionRangeWidgets[attributeName]->setMarkers(m_neuronRangeWidgets[attributeName]->getMarkers());
        }
    }


}

void AttributeWidget::m_connectionColorationChanged(const QString & attributeName)
{
    if (m_connectionFlagWidgets.contains(attributeName))
    {
        if (m_connectionSidebar->containsTool(m_connectionLinkButton)&&  m_connectionLinkButton->icon().cacheKey() == m_linkedIcon.cacheKey() )
        {
            if (m_neuronSidebar->containsWidget(attributeName) && m_neuronFlagWidgets.contains(attributeName) && !m_flagAttributeConsistent(attributeName))
                m_neuronFlagWidgets[attributeName]->setColors(m_connectionFlagWidgets[attributeName]->getOffColor(),m_connectionFlagWidgets[attributeName]->getOnColor());

        }
        connectionBitColorationChanged(attributeName,m_connectionFlagWidgets[attributeName]->getOffColor(),m_connectionFlagWidgets[attributeName]->getOnColor());

    }
    else
    {
        if(m_connectionRangeWidgets.contains(attributeName))
        {
            if ( m_attributeLinked[attributeName] && m_neuronRangeWidgets.contains(attributeName))
                    m_neuronRangeWidgets[attributeName]->setMarkers(m_connectionRangeWidgets[attributeName]->getMarkers());


            connectionRangeColorationChanged(attributeName,m_connectionRangeWidgets[attributeName]->getData(),m_connectionRangeWidgets[attributeName]->getImageSize().width());
        }
        else
        {
            if ( m_attributeLinked[attributeName] && m_neuronRangeWidgets.contains(attributeName))
                m_neuronRangeWidgets[attributeName]->setMarkers(m_connectionRangeWidgets[attributeName]->getMarkers());
        }

    }

}




