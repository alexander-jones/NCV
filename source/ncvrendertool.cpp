#include "ncvrendertool.h"
#include <QGroupBox>
NCVRenderTool::NCVRenderTool(QWidget *parent) :
    QwwTaskPanel(parent)
{
    m_connections = NULL;
    m_neurons = NULL;

	this->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    m_discreteNeuronMapper = new QSignalMapper(this);
    m_discreteConnectionMapper = new QSignalMapper(this);
    m_continuousNeuronMapper = new QSignalMapper(this);
    m_continuousConnectionMapper = new QSignalMapper(this);
    connect(m_discreteNeuronMapper,SIGNAL(mapped(const QString& )),this,SLOT(m_discreteNeuronColorationChanged(const QString&)));
    connect(m_discreteConnectionMapper,SIGNAL(mapped(const QString& )),this,SLOT(m_discreteConnectionColorationChanged(const QString&  )));
    connect(m_continuousNeuronMapper,SIGNAL(mapped(const QString& )),this,SLOT(m_continuousNeuronColorationChanged(const QString&)));
    connect(m_continuousConnectionMapper,SIGNAL(mapped(const QString& )),this,SLOT(m_continuousConnectionColorationChanged(const QString&)));

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

    m_neuronVector = new QWidgetVector();
    m_neuronVector->setDirection(QWidgetVector::TopToBottom);

    m_renderNeuronVector = new QWidgetVector();
    m_renderNeuronVector->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    m_renderNeuronVector->setAlignment(Qt::AlignCenter);
    m_renderNeuronVector->addWidget(new QLabel("Render:"));
    m_renderNeuronSwitch = new QSwitch();
    connect(m_renderNeuronSwitch,SIGNAL(switched(bool)),this,SLOT(m_neuronRenderSwitched(bool)));
    m_renderNeuronVector->addWidget(m_renderNeuronSwitch);
    m_neuronVector->addWidget(m_renderNeuronVector);


    m_neuronAttributeComboWidget = new ComboWidget();
    m_neuronAttributeComboWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    m_neuronVector->addWidget(m_neuronAttributeComboWidget);

    m_neuronScaleSlider = new QSlider(Qt::Horizontal);
    m_neuronScaleSlider->setMinimum(100);
    m_neuronScaleSlider->setMaximum(2000);
    m_neuronScaleSlider->setValue(825);
    m_neuronVector->addWidget(m_neuronScaleSlider);

    this->addTask(m_neuronVector,QIcon(":/assets/neuronIcon.png"),"Neurons");


    m_connectionVector = new QWidgetVector();
    m_connectionVector->setDirection(QWidgetVector::TopToBottom);

    m_renderConnectionVector = new QWidgetVector();
    m_renderConnectionVector->setAlignment(Qt::AlignCenter);
    m_renderConnectionVector->addWidget(new QLabel("Render:"));
    m_renderConnectionSwitch = new QSwitch();
    connect(m_renderConnectionSwitch,SIGNAL(switched(bool)),this,SLOT(m_connectionRenderSwitched(bool)));
    m_renderConnectionVector->addWidget(m_renderConnectionSwitch);
    m_connectionVector->addWidget(m_renderConnectionVector);


    m_connectionAttributeComboWidget = new ComboWidget();
    m_connectionAttributeComboWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    m_connectionVector->addWidget(m_connectionAttributeComboWidget);

    m_connectionScaleSlider = new QSlider(Qt::Horizontal);
    m_connectionScaleSlider->setMinimum(10);
    m_connectionScaleSlider->setMaximum(60);
    m_connectionScaleSlider->setValue(15);
    m_connectionVector->addWidget(m_connectionScaleSlider);

    this->addTask(m_connectionVector,QIcon(":/assets/connectionIcon.png"),"Connections");
	
    //this->addTask(new QWidget(),QIcon(":/assets/groupIcon.png"),"Groupings");


    connect(m_neuronAttributeComboWidget,SIGNAL(widgetChanged(QString)),this,SLOT(m_currentNeuronAttributeSet(QString)));
    connect(m_connectionAttributeComboWidget,SIGNAL(widgetChanged(QString)),this,SLOT(m_currentConnectionAttributeSet(QString)));

}

void  NCVRenderTool::setNeurons(NCVNeuronSet * neurons)
{
    m_neurons = neurons;
    if (m_neurons != NULL)
        disconnect(m_neuronScaleSlider,SIGNAL(valueChanged(int)),m_neurons,SLOT(setScale(int)));
    QMap<QString, NCVAttribute * > attributes = m_neurons->attributes();

    for (QMap<QString, NCVAttribute * >::iterator it = attributes.begin();it != attributes.end(); it++)
    {
        QString name = it.key();
        NCVAttribute * attribute = it.value();
        if (attribute->type() == Discrete)
        {
            NCVDiscreteAttribute * attrib = dynamic_cast<NCVDiscreteAttribute *>(attribute);
            m_createDiscreteAttributeColorWidget(name,attrib);
        }
        else if (attribute->type() == Continuous)
        {
            NCVContinuousAttribute * attrib = dynamic_cast<NCVContinuousAttribute *>(attribute);
            m_createContinuousAttributeColorWidget(name,attrib);
        }

    }
    connect(m_neuronScaleSlider,SIGNAL(valueChanged(int)),m_neurons,SLOT(setScale(int)));

}

void NCVRenderTool::m_neuronRenderSwitched(bool on)
{
    if (on)
    {
        m_neuronScaleSlider->setEnabled(true);
        m_neuronAttributeComboWidget->setEnabled(true);
    }
    else
    {
        m_neuronScaleSlider->setEnabled(false);
        m_neuronAttributeComboWidget->setEnabled(false);
    }
    neuronRenderSet(on);

}
void NCVRenderTool::m_connectionRenderSwitched(bool on)
{
    if (on)
    {
        m_connectionScaleSlider->setEnabled(true);
        m_connectionAttributeComboWidget->setEnabled(true);
    }
    else
    {
        m_connectionScaleSlider->setEnabled(false);
        m_connectionAttributeComboWidget->setEnabled(false);
    }
    connectionRenderSet(on);
}

void  NCVRenderTool::setConnections(NCVConnectionSet * connections)
{
    if (m_connections != NULL)
        disconnect(m_connectionScaleSlider,SIGNAL(valueChanged(int)),m_connections,SLOT(setScale(int)));
    m_connections = connections;
    QMap<QString, NCVAttribute * > attributes = m_connections->attributes();
    for (QMap<QString, NCVAttribute * >::iterator it = attributes.begin();it != attributes.end(); it++)
    {
        QString name = it.key();
        NCVAttribute * attribute = it.value();
        if (attribute->type() == Discrete)
        {
            NCVDiscreteAttribute * attrib = dynamic_cast<NCVDiscreteAttribute *>(attribute);
            m_createDiscreteAttributeColorWidget(name,attrib);
        }
        else if (attribute->type() == Continuous)
        {
            NCVContinuousAttribute * attrib = dynamic_cast<NCVContinuousAttribute *>(attribute);
            m_createContinuousAttributeColorWidget(name,attrib);
        }

    }

    connect(m_connectionScaleSlider,SIGNAL(valueChanged(int)),m_connections,SLOT(setScale(int)));
}


void NCVRenderTool::m_createDiscreteAttributeColorWidget(QString name,NCVDiscreteAttribute *attrib)
{
    if (attrib->elementType() == Neuron)
    {
        m_neuronDiscreteWidgets[name] = new DiscreteColorSelector();
        if (m_connectionDiscreteWidgets.contains(name))
            m_attributeShared[name] = m_attributeLinked[name] = true;
        else
            m_attributeShared[name] = m_attributeLinked[name] = false;


        connect(m_neuronDiscreteWidgets[name],SIGNAL(changed()),m_discreteNeuronMapper,SLOT(map()));
        m_discreteNeuronMapper->setMapping(m_neuronDiscreteWidgets[name],name);

        QMap<QString,QColor> states = attrib->coloration();
        for (QMap<QString,QColor>::iterator it = states.begin(); it != states.end(); it++)
            m_neuronDiscreteWidgets[name]->addState(it.key(),it.value());


        m_neuronAttributeComboWidget->addWidget(m_neuronDiscreteWidgets[name],name);



    }
    else if(attrib->elementType() == Connection)
    {

        m_connectionDiscreteWidgets[name] = new DiscreteColorSelector();
        if (m_neuronDiscreteWidgets.contains(name))
            m_attributeShared[name] = m_attributeLinked[name] = true;

        else
           m_attributeShared[name] = m_attributeLinked[name] = false;

        connect(m_connectionDiscreteWidgets[name],SIGNAL(changed()),m_discreteConnectionMapper,SLOT(map()));
        m_discreteConnectionMapper->setMapping(m_connectionDiscreteWidgets[name],name);

        QMap<QString,QColor> states = attrib->coloration();
        for (QMap<QString,QColor>::iterator it = states.begin(); it != states.end(); it++)
            m_connectionDiscreteWidgets[name]->addState(it.key(),it.value());

        m_connectionAttributeComboWidget->addWidget(m_connectionDiscreteWidgets[name],name);
    }
}

void NCVRenderTool::m_createContinuousAttributeColorWidget(QString name,NCVContinuousAttribute *attrib)
{
    if (attrib->elementType() == Neuron)
    {
        m_neuronContinuousWidgets[name] = new ContinuousColorSelector();
        if (m_connectionContinuousWidgets.contains(name))
            m_attributeShared[name] = m_attributeLinked[name] = true;
        else
            m_attributeShared[name] = m_attributeLinked[name] = false;

        m_neuronContinuousWidgets[name]->setLowThreshold(attrib->minValue());
        m_neuronContinuousWidgets[name]->setHighThreshold(attrib->maxValue());
        m_neuronContinuousWidgets[name]->setData(attrib->coloration());

        connect(m_neuronContinuousWidgets[name],SIGNAL(colorRangeChanged()),m_continuousNeuronMapper,SLOT(map()));
        m_continuousNeuronMapper->setMapping(m_neuronContinuousWidgets[name],name);

        m_neuronAttributeComboWidget->addWidget(m_neuronContinuousWidgets[name],name);

    }
    else if(attrib->elementType() == Connection)
    {
        m_connectionContinuousWidgets[name] = new ContinuousColorSelector();
        if (m_neuronContinuousWidgets.contains(name))
           m_attributeShared[name] = m_attributeLinked[name] = true;
        else
           m_attributeShared[name] =  m_attributeLinked[name] = false;

        m_connectionContinuousWidgets[name]->setLowThreshold(attrib->minValue());
        m_connectionContinuousWidgets[name]->setHighThreshold(attrib->maxValue());
        m_connectionContinuousWidgets[name]->setData(attrib->coloration());

        connect(m_connectionContinuousWidgets[name],SIGNAL(colorRangeChanged()),m_continuousConnectionMapper,SLOT(map()));
        m_continuousConnectionMapper->setMapping(m_connectionContinuousWidgets[name],name);

        m_connectionAttributeComboWidget->addWidget(m_connectionContinuousWidgets[name],name);

    }
}

void NCVRenderTool::m_updateNeuronLinkIcon()
{
    if (m_attributeLinked[m_neuronAttributeComboWidget->currentWidgetKey()])
        m_neuronLinkButton->setIcon(m_linkedIcon);
    else
        m_neuronLinkButton->setIcon(m_unlinkedIcon);
}

void NCVRenderTool::m_updateConnectionLinkIcon()
{
    if (m_attributeLinked[m_connectionAttributeComboWidget->currentWidgetKey()])
        m_connectionLinkButton->setIcon(m_linkedIcon);
    else
        m_connectionLinkButton->setIcon(m_unlinkedIcon);
}

void NCVRenderTool::m_currentNeuronAttributeSet(QString name)
{
    if (m_attributeShared[name])
    {
        if (!m_neuronAttributeComboWidget->containsTool(m_neuronLinkButton))
        {
            m_neuronAttributeComboWidget->addTool(m_neuronLinkButton);
            m_neuronLinkButton->show();
        }
        if ( m_connectionAttributeComboWidget->currentWidgetKey() == name && !m_connectionAttributeComboWidget->containsTool(m_connectionLinkButton))
        {
            m_connectionAttributeComboWidget->addTool(m_connectionLinkButton);
            m_connectionLinkButton->show();
        }

        m_updateNeuronLinkIcon();
        m_updateConnectionLinkIcon();
    }
    else if ( m_neuronAttributeComboWidget->containsTool(m_neuronLinkButton))
    {
        m_neuronAttributeComboWidget->removeTool(m_neuronLinkButton);
        m_neuronLinkButton->hide();
    }
    m_neurons->setCurrentAttribute(name);


}


void NCVRenderTool::m_currentConnectionAttributeSet(QString name)
{
    if (m_attributeShared[name])
    {

        if ( !m_connectionAttributeComboWidget->containsTool(m_connectionLinkButton))
        {
            m_connectionAttributeComboWidget->addTool(m_connectionLinkButton);
            m_connectionLinkButton->show();
        }

        if ( m_neuronAttributeComboWidget->currentWidgetKey() == name && !m_neuronAttributeComboWidget->containsTool(m_neuronLinkButton))
        {
            m_neuronAttributeComboWidget->addTool(m_neuronLinkButton);
            m_neuronLinkButton->show();
        }

        m_updateNeuronLinkIcon();
        m_updateConnectionLinkIcon();
    }
    else if ( m_connectionAttributeComboWidget->containsTool(m_connectionLinkButton))
    {
        m_connectionAttributeComboWidget->removeTool(m_connectionLinkButton);
        m_connectionLinkButton->hide();
    }
    m_connections->setCurrentAttribute(name);

}

bool NCVRenderTool::m_continuousAttributeConsistent(QString attributeName)
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


bool NCVRenderTool::m_discreteAttributeConsistent(QString attributeName)
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

void NCVRenderTool::m_checkAndFixInconsistantColorations(QString attributeName)
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
void NCVRenderTool::m_discreteNeuronColorationChanged( QString attributeName)
{
    QMap<QString,QColor> states =  m_neuronDiscreteWidgets[attributeName]->states();
    NCVDiscreteAttribute * attrib = dynamic_cast<NCVDiscreteAttribute *>((m_neurons->attributes())[attributeName]);
    attrib->attachColoration(states);

    if (m_neuronAttributeComboWidget->containsTool(m_neuronLinkButton) && m_neuronLinkButton->icon().cacheKey() == m_linkedIcon.cacheKey() )
    {

        if (m_connectionAttributeComboWidget->containsWidget(attributeName)  && m_connectionDiscreteWidgets.contains(attributeName) && !m_discreteAttributeConsistent(attributeName))
        {

             for (QMap<QString,QColor>::iterator it = states.begin(); it != states.end(); it++)
                 if (m_neuronDiscreteWidgets[attributeName]->color(it.key()) != m_connectionDiscreteWidgets[attributeName]->color(it.key()))
                     m_connectionDiscreteWidgets[attributeName]->setState(it.key(),m_neuronDiscreteWidgets[attributeName]->color(it.key()));

        }

    }
}

void NCVRenderTool::m_continuousNeuronColorationChanged( QString attributeName)
{
    NCVContinuousAttribute * attrib = dynamic_cast<NCVContinuousAttribute *>((m_neurons->attributes())[attributeName]);

    attrib->attachColoration(m_neuronContinuousWidgets[attributeName]->getData());
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
void NCVRenderTool::m_discreteConnectionColorationChanged( QString attributeName)
{

    QMap<QString,QColor> states =  m_connectionDiscreteWidgets[attributeName]->states();
    NCVDiscreteAttribute * attrib = dynamic_cast<NCVDiscreteAttribute *>((m_connections->attributes())[attributeName]);
    attrib->attachColoration(states);
    if (m_connectionAttributeComboWidget->containsTool(m_connectionLinkButton)&&  m_connectionLinkButton->icon().cacheKey() == m_linkedIcon.cacheKey() )
    {
        if (m_neuronAttributeComboWidget->containsWidget(attributeName) && m_neuronDiscreteWidgets.contains(attributeName) && !m_discreteAttributeConsistent(attributeName))
        {
            QMap<QString,QColor> states =  m_neuronDiscreteWidgets[attributeName]->states();
            for (QMap<QString,QColor>::iterator it = states.begin(); it != states.end(); it++)
                if (m_neuronDiscreteWidgets[attributeName]->color(it.key()) != m_connectionDiscreteWidgets[attributeName]->color(it.key()))
                    m_neuronDiscreteWidgets[attributeName]->setState(it.key(),m_connectionDiscreteWidgets[attributeName]->color(it.key()));

        }

    }

}

void NCVRenderTool::m_continuousConnectionColorationChanged( QString attributeName)
{
    NCVContinuousAttribute * attrib = dynamic_cast<NCVContinuousAttribute *>((m_connections->attributes())[attributeName]);
    attrib->attachColoration(m_connectionContinuousWidgets[attributeName]->getData());
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

void NCVRenderTool::m_neuronLinkButtonPressed()
{
    QString attributeName = m_neuronAttributeComboWidget->currentWidgetKey();
    m_attributeLinked[attributeName] = !m_attributeLinked[attributeName];
    m_checkAndFixInconsistantColorations(attributeName);
    m_updateNeuronLinkIcon();
    m_updateConnectionLinkIcon();

}

void NCVRenderTool::m_connectionLinkButtonPressed()
{
    QString attributeName = m_connectionAttributeComboWidget->currentWidgetKey();
    m_attributeLinked[attributeName] = !m_attributeLinked[attributeName];

    m_checkAndFixInconsistantColorations(attributeName);
    m_updateNeuronLinkIcon();
    m_updateConnectionLinkIcon();
}

