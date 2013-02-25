#include "ncvwidget.h"

NCVWidget::NCVWidget(QWidget *parent) :
    QWidget(parent)
{

    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_layout->setSpacing(0);


    m_framesPerSecond = new QLabel();
    m_framesPerSecond->setAlignment(Qt::AlignCenter);


    m_ncvSidebar = new NCVSidebar();
    m_ncvSidebar->addTab(QIcon(":/assets/presentationIcon.png"),"Presentation");
    m_layout->addWidget(m_ncvSidebar);

    m_collapsed = false;

    m_expandText = ">";
    m_collapseText = "<";
    m_collapseButton = new OrientationButton();
    QFont font = m_collapseButton->font();
    font.setPointSize(5);
    m_collapseButton->setFont(font);

    m_collapseButton->setToolTip("Click to collapse the management sidebar");
    m_collapseButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    m_collapseButton->setMaximumWidth(10);
    m_collapseButton->setText(m_collapseText);

    connect(m_collapseButton,SIGNAL(clicked()),this,SLOT(m_collapseButtonPressed()));
    m_layout->addWidget(m_collapseButton);

    QGLFormat glFormat;
    glFormat.setVersion(3,3 );
    glFormat.setDoubleBuffer(true);
    glFormat.setProfile( QGLFormat::CompatibilityProfile ); // Requires >=Qt-4.8.0
    glFormat.setSampleBuffers( true );
    m_canvas = new NCVCanvas( glFormat );
    m_canvas->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_layout->addWidget(m_canvas);

    setLayout(m_layout);
    setFocus();


    m_frameCount = 0;
    m_timer.start();
    connect(m_canvas,SIGNAL(frameRendered()),this,SLOT(m_newFrameReceived()));
    connect(m_canvas, SIGNAL(invalidGraphicsConfigurationDetected()),this,SLOT(m_reportFatalError()));


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

    m_neuronSidebar = new ComboWidget();
    m_ncvSidebar->setTabWidget("Presentation",Neuron,m_neuronSidebar,true);

    m_connectionSidebar = new ComboWidget();
    m_ncvSidebar->setTabWidget("Presentation",Connection,m_connectionSidebar,true);

    connect(m_neuronSidebar,SIGNAL(widgetChanged(QString)),this,SLOT(m_currentNeuronAttributeSet(QString)));
    connect(m_connectionSidebar,SIGNAL(widgetChanged(QString)),this,SLOT(m_currentConnectionAttributeSet(QString)));

}

void  NCVWidget::setNeurons(NCVNeuronSet * neurons)
{
    m_neurons = neurons;
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

    m_canvas->setNeurons(m_neurons);
}

void  NCVWidget::setConnections(NCVConnectionSet * connections)
{
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

    m_canvas->setConnections(m_connections);
}


void NCVWidget::m_createDiscreteAttributeColorWidget(QString name,NCVDiscreteAttribute *attrib)
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


        m_neuronSidebar->addWidget(m_neuronDiscreteWidgets[name],name);



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

        m_connectionSidebar->addWidget(m_connectionDiscreteWidgets[name],name);
    }
}

void NCVWidget::m_createContinuousAttributeColorWidget(QString name,NCVContinuousAttribute *attrib)
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

        m_neuronSidebar->addWidget(m_neuronContinuousWidgets[name],name);

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

        m_connectionSidebar->addWidget(m_connectionContinuousWidgets[name],name);

    }
}

void NCVWidget::m_updateNeuronLinkIcon()
{
    if (m_attributeLinked[m_neuronSidebar->currentWidgetName()])
        m_neuronLinkButton->setIcon(m_linkedIcon);
    else
        m_neuronLinkButton->setIcon(m_unlinkedIcon);
}

void NCVWidget::m_updateConnectionLinkIcon()
{
    if (m_attributeLinked[m_connectionSidebar->currentWidgetName()])
        m_connectionLinkButton->setIcon(m_linkedIcon);
    else
        m_connectionLinkButton->setIcon(m_unlinkedIcon);
}

void NCVWidget::m_currentNeuronAttributeSet(QString name)
{
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
    m_neurons->setCurrentAttribute(name);


}


void NCVWidget::m_currentConnectionAttributeSet(QString name)
{
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
    m_connections->setCurrentAttribute(name);

}

bool NCVWidget::m_continuousAttributeConsistent(QString attributeName)
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


bool NCVWidget::m_discreteAttributeConsistent(QString attributeName)
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

void NCVWidget::m_checkAndFixInconsistantColorations(QString attributeName)
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
void NCVWidget::m_discreteNeuronColorationChanged( QString attributeName)
{
    QMap<QString,QColor> states =  m_neuronDiscreteWidgets[attributeName]->states();
    NCVDiscreteAttribute * attrib = dynamic_cast<NCVDiscreteAttribute *>((m_neurons->attributes())[attributeName]);
    attrib->attachColoration(states);

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

void NCVWidget::m_continuousNeuronColorationChanged( QString attributeName)
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
void NCVWidget::m_discreteConnectionColorationChanged( QString attributeName)
{

    QMap<QString,QColor> states =  m_connectionDiscreteWidgets[attributeName]->states();
    NCVDiscreteAttribute * attrib = dynamic_cast<NCVDiscreteAttribute *>((m_connections->attributes())[attributeName]);
    attrib->attachColoration(states);
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

void NCVWidget::m_continuousConnectionColorationChanged( QString attributeName)
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

void NCVWidget::m_neuronLinkButtonPressed()
{
    QString attributeName = m_neuronSidebar->currentWidgetName();
    m_attributeLinked[attributeName] = !m_attributeLinked[attributeName];
    m_checkAndFixInconsistantColorations(attributeName);
    m_updateNeuronLinkIcon();
    m_updateConnectionLinkIcon();

}

void NCVWidget::m_connectionLinkButtonPressed()
{
    QString attributeName = m_connectionSidebar->currentWidgetName();
    m_attributeLinked[attributeName] = !m_attributeLinked[attributeName];

    m_checkAndFixInconsistantColorations(attributeName);
    m_updateNeuronLinkIcon();
    m_updateConnectionLinkIcon();
}

void NCVWidget::m_collapseButtonPressed()
{
    if (!m_collapsed)
    {
        m_collapsed = true;
        m_layout->removeWidget(m_ncvSidebar);
        m_ncvSidebar->hide();
        m_collapseButton->setText(m_expandText);
        m_collapseButton->setToolTip("Click to expand the management sidebar");

    }
    else
    {
        m_collapsed = false;
        m_layout->insertWidget(0,m_ncvSidebar);
        m_ncvSidebar->show();
        m_collapseButton->setText(m_collapseText);
        m_collapseButton->setToolTip("Click to collapse the management sidebar");
    }

}

void NCVWidget::m_newFrameReceived()
{
    // FPS counter
    m_frameCount ++;
    if (m_timer.elapsed() >= 1000)
    {
        //m_ncvSidebar->setFPS((float)m_frameCount/((float)m_timer.elapsed() /1000.0f));
        m_timer.start();
        m_frameCount = 0;
    }
}


NCVWidget::~NCVWidget()
{
   delete m_layout;
   delete m_ncvSidebar;

   //delete m_statusSidebar;
   //delete m_canvas;


}

void NCVWidget::m_reportFatalError()
{

    QMessageBox msgBox;
    msgBox.setText("Graphics Configuration Incapable");
    msgBox.setInformativeText("Cannot create a valid OpenGL 3.3 context.");
    msgBox.setStandardButtons(QMessageBox::Ok);

    msgBox.exec();
}
/*void NCVSidebar::m_buildOrganizationTab()
{
    m_organizationTaskPanel = new QwwTaskPanel();
    QTreeWidget  *tree = new QTreeWidget ();
    tree->setSelectionMode( QAbstractItemView::MultiSelection);
    tree->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    QFont font = tree->font();
    font.setPointSize(8);
    tree->setFont(font);
    //Set QTreeWidget Column Header
    QTreeWidgetItem* headerItem = new QTreeWidgetItem();
    headerItem->setText(0,QString("Group"));
    headerItem->setText(1,QString("Neruons"));
    headerItem->setText(2,QString("Connections"));
    headerItem->setText(3,QString("Subgroups"));
    tree->setHeaderItem(headerItem);


    QDir* rootDir = new QDir("/");
    QFileInfoList filesList = rootDir->entryInfoList();


    foreach(QFileInfo fileInfo, filesList)
    {
      QTreeWidgetItem* item = new QTreeWidgetItem();
      item->setText(0,fileInfo.fileName());

      item->setText(0,fileInfo.fileName());
      item->setText(1,QString::number(50000));

        //child->setIcon(0,*(new QIcon("folder.jpg")));
      item->setText(2,QString::number(500000));

      if(fileInfo.isDir())
      {
        //item->setIcon(0,*(new QIcon("folder.jpg")));
        addChildren(item,fileInfo.filePath());
      }
      tree->addTopLevelItem(item);
    }


}

void NCVSidebar::addChildren(QTreeWidgetItem* item,QString filePath)
{
    QDir* rootDir = new QDir(filePath);
    QFileInfoList filesList = rootDir->entryInfoList();

    foreach(QFileInfo fileInfo, filesList)
    {
        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setText(0,fileInfo.fileName());
        child->setText(1,QString::number(50000));

          //child->setIcon(0,*(new QIcon("folder.jpg")));
        child->setText(2,QString::number(500000));

        item->addChild(child);
    }
}
*/
