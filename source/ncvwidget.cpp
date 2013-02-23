#include "ncvwidget.h"


NCVWidget::NCVWidget(QWidget *parent) :
    QWidget(parent)
{

    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_layout->setSpacing(0);

    m_managementSidebar= new ManagementSidebar();
    m_managementSidebar->show();
    m_layout->addWidget(m_managementSidebar);

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

    m_createNetwork(50000,50000,QVector3D(50000 , 50000,50000));

    m_frameCount = 0;
    m_timer.start();
    connect(m_canvas,SIGNAL(frameRendered()),this,SLOT(m_newFrameReceived()));
    connect(m_canvas, SIGNAL(invalidGraphicsConfigurationDetected()),this,SLOT(m_reportFatalError()));

}
void NCVWidget::m_createNetwork(int numNeurons,int numConnections, QVector3D worldSize)
{
    // establish discrete attribute states;
    QVector<QString> firingStates;
    firingStates.append("Off");
    firingStates.append("On");

    QVector<QString> typeStates;
    typeStates.append("Ovoid");
    typeStates.append("Izekivich");
    typeStates.append("Pyramidal");
    typeStates.append("Granule");
    typeStates.append("Double Pyramidal");

    // establish attribute  colorations
    QVector<QColor> stubVoltageColoration; // coloration replaced by attribute widget (will be changed in later push)
    stubVoltageColoration.append(QColor(0,0,0));

    QMap<QString,QColor >firingColors;
    firingColors["Off"] =QColor(0,255,0);
    firingColors["On"] = QColor(255,0,0);

    QMap<QString,QColor >typeColors;
    typeColors["Ovoid"] = QColor(125,125,0);
    typeColors["Izekivich"] = QColor(255,0,125);
    typeColors["Pyramidal"] = QColor(0,125,125);
    typeColors["Granule"] = QColor(75,255,125);
    typeColors["Double Pyramidal"] = QColor(255,255,0);

    // create stub neuron data source
    QVector<QVector3D> neuronPositions;
    QVector<GLfloat> neuronVoltages;
    QVector<GLubyte> neuronFirings;
    QVector<GLubyte> neuronTypes;

    int fireVal;
    float threshold = 25.0f;

    for (int i = 0; i <numNeurons; i ++)
    {
        int x = (abs(rand() * rand() * rand() )% (int)worldSize.x()) - (worldSize.x()/2);
        int y = (abs(rand() * rand() * rand() ) % (int)worldSize.y()) - (worldSize.y()/2);
        int z = (abs(rand() * rand() * rand() ) % (int)worldSize.z()) - (worldSize.z()/2);
        neuronPositions.append(QVector3D(x,y,z));
        neuronVoltages.append((float)(abs(rand() % 120)) -80.0);

        if (neuronVoltages[i] >= threshold)
            fireVal = 1;
        else
            fireVal = 0;

        if(i %8 == 0)
            neuronFirings.append(0);

        neuronFirings[i/8] += fireVal<<(i % 8);

        neuronTypes.append(rand() % 5);
    }


    // create neurons and related attributes
    m_neurons= new NCVNeuronSet(neuronPositions);
    connect(m_managementSidebar->attributeWidget(),SIGNAL(currentNeuronAttributeSet(QString)),m_neurons,SLOT(setCurrentAttribute(QString)));

    m_neuronContinuousAttributes["voltage"] = new NCVContinuousAttribute(Neuron,-80,40);
    m_neuronContinuousAttributes["voltage"]->attachData(neuronVoltages);
    m_neuronContinuousAttributes["voltage"]->attachColoration(stubVoltageColoration);
    m_neurons->addAttribute("voltage",m_neuronContinuousAttributes["voltage"]);

    m_neuronDiscreteAttributes["firing"] = new NCVDiscreteAttribute(Neuron,firingStates);
    m_neuronDiscreteAttributes["firing"]->attachData(neuronFirings,1);
    m_neuronDiscreteAttributes["firing"]->attachColoration(firingColors);
    m_neurons->addAttribute("firing",m_neuronDiscreteAttributes["firing"]);


    m_neuronDiscreteAttributes["type"] = new NCVDiscreteAttribute(Neuron,typeStates);
    m_neuronDiscreteAttributes["type"]->attachData(neuronTypes);
    m_neuronDiscreteAttributes["type"]->attachColoration(typeColors);
    m_neurons->addAttribute("type",m_neuronDiscreteAttributes["type"]);

    m_canvas->setNeurons(m_neurons); // attach neurons to canvas

    // add neuron widgets to attribute widget
    m_managementSidebar->attributeWidget()->addDiscreteAttribute("firing",m_neuronDiscreteAttributes["firing"]);
    m_managementSidebar->attributeWidget()->addDiscreteAttribute("type",m_neuronDiscreteAttributes["type"]);
    m_managementSidebar->attributeWidget()->addContinuousAttribute("voltage",m_neuronContinuousAttributes["voltage"]);

    // create stub connection data
    QVector<NeuronConnection> connections;
    for (int i = 0; i < numConnections; i++)
    {
        connections.append(NeuronConnection(i % numNeurons,i % numNeurons));
        while (connections[i].outNeuron == connections[i].inNeuron)
            connections[i].outNeuron = (rand() % numNeurons) +1;
    }

    QVector<GLfloat> connectionVoltages;
    QVector<GLubyte> connectionFirings;

    for (int i = 0; i <numConnections; i ++)
    {
        GLfloat inVoltage = neuronVoltages[connections[i].inNeuron];
        GLfloat outVoltage = neuronVoltages[connections[i].outNeuron];

        connectionVoltages.append((outVoltage + inVoltage)/2);
        if (connectionVoltages[i] >= threshold)
            fireVal = 1;
        else
            fireVal = 0;

        if(i %8 == 0)
            connectionFirings.append(0);
        connectionFirings[i/8] += fireVal<<(i % 8);


    }

    //create connections and related attributes
    m_connections= new NCVConnectionSet(m_neurons,connections);
    connect(m_managementSidebar->attributeWidget(),SIGNAL(currentConnectionAttributeSet(QString)),m_connections,SLOT(setCurrentAttribute(QString)));

    m_connectionContinuousAttributes["voltage"] = new NCVContinuousAttribute(Connection,-80,40);
    m_connectionContinuousAttributes["voltage"]->attachData(connectionVoltages);
    m_connectionContinuousAttributes["voltage"]->attachColoration(stubVoltageColoration);
    m_connections->addAttribute("voltage",m_connectionContinuousAttributes["voltage"]);

    m_connectionDiscreteAttributes["firing"] = new NCVDiscreteAttribute(Connection,firingStates);
    m_connectionDiscreteAttributes["firing"]->attachData(connectionFirings,1);
    m_connectionDiscreteAttributes["firing"]->attachColoration(firingColors);
    m_connections->addAttribute("firing",m_connectionDiscreteAttributes["firing"]);

    m_managementSidebar->attributeWidget()->addDiscreteAttribute("firing",m_connectionDiscreteAttributes["firing"]);
    m_managementSidebar->attributeWidget()->addContinuousAttribute("voltage",m_connectionContinuousAttributes["voltage"]);

    m_canvas->setConnections(m_connections); // connect to canvas

}

void NCVWidget::m_collapseButtonPressed()
{
    if (!m_collapsed)
    {
        m_collapsed = true;
        m_layout->removeWidget(m_managementSidebar);
        m_managementSidebar->hide();
        m_collapseButton->setText(m_expandText);
        m_collapseButton->setToolTip("Click to expand the management sidebar");

    }
    else
    {
        m_collapsed = false;
        m_layout->insertWidget(0,m_managementSidebar);
        m_managementSidebar->show();
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
        m_managementSidebar->setFPS((float)m_frameCount/((float)m_timer.elapsed() /1000.0f));
        m_timer.start();
        m_frameCount = 0;
    }
}


NCVWidget::~NCVWidget()
{
   delete m_layout;
   delete m_managementSidebar;

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
