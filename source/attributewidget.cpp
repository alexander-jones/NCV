#include "attributewidget.h"
#include <QDebug>

AttributeWidget::AttributeWidget(QString text,QWidget *parent) :
    CollapsibleWidget(NULL,text,parent)
{
    m_widget = new QWidget();
    m_layout = new QVBoxLayout();


    m_neuronLinkButton = new QPushButton();
    m_neuronLinkButton->setToolTip("Set whether neuron and connection colorations should be linked for this attribute.");
    m_connectionLinkButton = new QPushButton();
    m_connectionLinkButton->setToolTip("Set whether neuron and connection colorations should be linked for this attribute.");
    connect(m_neuronLinkButton,SIGNAL(pressed()),this,SLOT(m_neuronLinkButtonPressed()));
    connect(m_connectionLinkButton,SIGNAL(pressed()),this,SLOT(m_connectionLinkButtonPressed()));

    m_linkedIcon = QIcon(":/assets/linked.png");
    m_unlinkedIcon = QIcon(":/assets/unlinked.png");

    m_neuronSidebar = new Sidebar();
    m_neuronSection = new CollapsibleWidget(m_neuronSidebar,"Neuron Attribute Coloration");
    m_neuronSection->setFixedWidth(300);
    m_neuronSection->underlineHeader(true);
    m_neuronSection->expand();
    m_layout->addWidget(m_neuronSection);

    m_connectionSidebar = new Sidebar();
    m_connectionSection = new CollapsibleWidget(m_connectionSidebar,"Connection Attribute Coloration");
    m_connectionSection->setFixedWidth(300);
    m_connectionSection->underlineHeader(true);
    m_connectionSection->expand();
    m_layout->addWidget(m_connectionSection);

    m_widget->setLayout(m_layout);
    this->setWidget(m_widget);
    connect(m_neuronSidebar,SIGNAL(panelChanged(QString)),this,SLOT(m_currentNeuronAttributeSet(QString)));
    connect(m_connectionSidebar,SIGNAL(panelChanged(QString)),this,SLOT(m_currentConnectionAttributeSet(QString)));
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
    QString attributeName = m_neuronSidebar->currentPanel();
    m_attributeLinked[attributeName] = !m_attributeLinked[attributeName];
    m_checkAndFixInconsistantColorations(attributeName);
    m_updateNeuronLinkIcon();
    m_updateConnectionLinkIcon();

}

void AttributeWidget::m_connectionLinkButtonPressed()
{
    QString attributeName = m_connectionSidebar->currentPanel();
    m_attributeLinked[attributeName] = !m_attributeLinked[attributeName];

    m_checkAndFixInconsistantColorations(attributeName);
    m_updateNeuronLinkIcon();
    m_updateConnectionLinkIcon();
}


void AttributeWidget::m_updateNeuronLinkIcon()
{
    if (m_attributeLinked[m_neuronSidebar->currentPanel()])
        m_neuronLinkButton->setIcon(m_linkedIcon);
    else
        m_neuronLinkButton->setIcon(m_unlinkedIcon);
}

void AttributeWidget::m_updateConnectionLinkIcon()
{
    if (m_attributeLinked[m_connectionSidebar->currentPanel()])
        m_connectionLinkButton->setIcon(m_linkedIcon);
    else
        m_connectionLinkButton->setIcon(m_unlinkedIcon);
}

void AttributeWidget::m_currentNeuronAttributeSet(QString name)
{
    currentNeuronAttributeSet(name);
    if (m_isSharedAttribute(name))
    {

        if ( !m_neuronSidebar->containsTool(m_neuronLinkButton))
            m_neuronSidebar->addTool(m_neuronLinkButton);

        if ( m_connectionSidebar->currentPanel() == name && !m_connectionSidebar->containsTool(m_connectionLinkButton))
            m_connectionSidebar->addTool(m_connectionLinkButton);

    }
    else if ( m_connectionSidebar->containsTool(m_neuronLinkButton))
            m_neuronSidebar->removeTool(m_neuronLinkButton);

    m_updateNeuronLinkIcon();
    m_updateConnectionLinkIcon();
}


void AttributeWidget::m_currentConnectionAttributeSet(QString name)
{
    currentConnectionAttributeSet(name);
    if (m_isSharedAttribute(name))
    {

        if ( !m_connectionSidebar->containsTool(m_connectionLinkButton))
            m_connectionSidebar->addTool(m_connectionLinkButton);

        if ( m_neuronSidebar->currentPanel() == name && !m_neuronSidebar->containsTool(m_neuronLinkButton))
            m_neuronSidebar->addTool(m_neuronLinkButton);
    }
    else if ( m_connectionSidebar->containsTool(m_connectionLinkButton))
            m_connectionSidebar->removeTool(m_connectionLinkButton);

    m_updateNeuronLinkIcon();
    m_updateConnectionLinkIcon();
}

void AttributeWidget::addNeuronRangeAttribute(QString name, float minVal, float maxVal)
{
    m_neuronRangeWidgets[name] = new ColorRangeWidget();
    m_neuronRangeWidgets[name]->setLowThreshold(minVal);
    m_neuronRangeWidgets[name]->setHighThreshold(maxVal);
    m_neuronRangeWidgets[name]->setImageSize(300,25);
    m_neuronRangeWidgets[name]->setRangeToGradient(minVal,maxVal,QColor(0,255,0),QColor(255,0,255));
    m_neuronSidebar->addPanel(m_neuronRangeWidgets[name],name);
    m_attributeLinked[name] = true;
    neuronRangeColorationChanged(name,m_neuronRangeWidgets[name]->getData(),m_neuronRangeWidgets[name]->getImageSize().width());

    connect(m_neuronRangeWidgets[name],SIGNAL(markersChanged(QList<Cursor>)),this,SLOT(m_neuronRangeColorationChanged(QList<Cursor>)));


}

void AttributeWidget::addNeuronBitAttribute(QString name, QColor offColor, QColor onColor)
{
    m_neuronFlagWidgets[name] = new ColorBitWidget();
    connect(m_neuronFlagWidgets[name] ,SIGNAL(colorsChanged(QColor,QColor)),this,SLOT(m_neuronBitColorationChanged(QColor,QColor)));
    m_neuronFlagWidgets[name]->setOffColor(offColor);
    m_neuronFlagWidgets[name]->setOnColor(onColor);
    m_neuronSidebar->addPanel(m_neuronFlagWidgets[name],name);
    m_attributeLinked[name] = true;
}

void AttributeWidget::addConnectionRangeAttribute(QString name, float minVal, float maxVal)
{
    m_connectionRangeWidgets[name] = new ColorRangeWidget();
    m_connectionRangeWidgets[name]->setLowThreshold(minVal);
    m_connectionRangeWidgets[name]->setHighThreshold(maxVal);
    m_connectionRangeWidgets[name]->setImageSize(300,25);
    m_connectionRangeWidgets[name]->setRangeToGradient(minVal,maxVal,QColor(0,255,0),QColor(255,0,255));
    m_connectionSidebar->addPanel(m_connectionRangeWidgets[name],name);
    m_attributeLinked[name] = true;
    connectionRangeColorationChanged(name,m_connectionRangeWidgets[name]->getData(),m_connectionRangeWidgets[name]->getImageSize().width());

    connect(m_connectionRangeWidgets[name],SIGNAL(markersChanged(QList<Cursor>)),this,SLOT(m_connectionRangeColorationChanged(QList<Cursor>)));

}

void AttributeWidget::addConnectionBitAttribute(QString name, QColor offColor, QColor onColor)
{
    m_connectionFlagWidgets[name] = new ColorBitWidget();
    connect(m_connectionFlagWidgets[name],SIGNAL(colorsChanged(QColor,QColor)),this,SLOT(m_connectionBitColorationChanged(QColor,QColor)));
    m_connectionFlagWidgets[name]->setOffColor(offColor);
    m_connectionFlagWidgets[name]->setOnColor(onColor);
    m_connectionSidebar->addPanel(m_connectionFlagWidgets[name],name);
    m_attributeLinked[name] = true;
}


void AttributeWidget::m_neuronBitColorationChanged(QColor offColor,QColor onColor)
{
    QString attributeName = m_neuronSidebar->currentPanel();
    if (m_neuronSidebar->containsTool(m_neuronLinkButton) && m_neuronLinkButton->icon().cacheKey() == m_linkedIcon.cacheKey() )
    {

        if (m_connectionSidebar->containsPanel(attributeName)  && m_connectionFlagWidgets.contains(attributeName) && !m_flagAttributeConsistent(attributeName))
                m_connectionFlagWidgets[attributeName]->setColors(offColor,onColor);

    }
    neuronBitColorationChanged(attributeName, offColor,onColor);
}
void AttributeWidget::m_neuronRangeColorationChanged(QList<Cursor> markers)
{

    QString attributeName = m_neuronSidebar->currentPanel();
    if(m_neuronRangeWidgets.contains(attributeName))
    {
        if ( m_attributeLinked[attributeName] && m_connectionRangeWidgets.contains(attributeName))
            m_connectionRangeWidgets[attributeName]->setMarkers(markers);


        neuronRangeColorationChanged(attributeName,m_neuronRangeWidgets[attributeName]->getData(),m_neuronRangeWidgets[attributeName]->getImageSize().width());
    }
    else
    {
        QString attributeName = m_connectionSidebar->currentPanel();
        if ( m_attributeLinked[attributeName] && m_connectionRangeWidgets.contains(attributeName))
            m_connectionRangeWidgets[attributeName]->setMarkers(markers);
    }

}

void AttributeWidget::m_connectionBitColorationChanged(QColor offColor,QColor onColor)
{
    QString attributeName = m_connectionSidebar->currentPanel();
    if (m_connectionSidebar->containsTool(m_connectionLinkButton)&&  m_connectionLinkButton->icon().cacheKey() == m_linkedIcon.cacheKey() )
    {
        if (m_neuronSidebar->containsPanel(attributeName) && m_neuronFlagWidgets.contains(attributeName) && !m_flagAttributeConsistent(attributeName))
            m_neuronFlagWidgets[attributeName]->setColors(offColor,onColor);

    }
    connectionBitColorationChanged(attributeName,offColor,onColor);
}

void AttributeWidget::m_connectionRangeColorationChanged(QList<Cursor> markers)
{
    QString attributeName = m_connectionSidebar->currentPanel();
    if(m_connectionRangeWidgets.contains(attributeName))
    {
        if ( m_attributeLinked[attributeName] && m_neuronRangeWidgets.contains(attributeName))
            m_neuronRangeWidgets[attributeName]->setMarkers(markers);


        connectionRangeColorationChanged(attributeName,m_connectionRangeWidgets[attributeName]->getData(),m_connectionRangeWidgets[attributeName]->getImageSize().width());
    }
    else
    {
        QString attributeName = m_neuronSidebar->currentPanel();
        if ( m_attributeLinked[attributeName] && m_neuronRangeWidgets.contains(attributeName))
            m_neuronRangeWidgets[attributeName]->setMarkers(markers);
    }


}

