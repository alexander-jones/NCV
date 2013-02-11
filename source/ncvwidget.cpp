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
    glFormat.setVersion( 4,0 );
    glFormat.setDoubleBuffer(true);
    glFormat.setProfile( QGLFormat::CompatibilityProfile ); // Requires >=Qt-4.8.0
    glFormat.setSampleBuffers( true );
    m_visualization = new NCV( glFormat );
    m_visualization->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_layout->addWidget(m_visualization);
   // m_layout->addWidget(m_statusSidebar);

    setLayout(m_layout);
    setFocus();

    connect(m_visualization,SIGNAL(frameRendered()),this,SLOT(m_newFrameReceived()));
    connect(m_visualization,SIGNAL(newNeuronBitAttribute(QString,QColor,QColor)),m_managementSidebar->attributeWidget(),SLOT(addNeuronBitAttribute(QString,QColor,QColor)));
    connect(m_visualization,SIGNAL(newConnectionBitAttribute(QString,QColor,QColor)),m_managementSidebar->attributeWidget(),SLOT(addConnectionBitAttribute(QString,QColor,QColor)));
    connect(m_visualization,SIGNAL(newConnectionRangeAttribute(QString,float,float)),m_managementSidebar->attributeWidget(),SLOT(addConnectionRangeAttribute(QString,float,float)));
    connect(m_visualization,SIGNAL(newNeuronRangeAttribute(QString,float,float)),m_managementSidebar->attributeWidget(),SLOT(addNeuronRangeAttribute(QString,float,float)));

    connect(m_managementSidebar->attributeWidget(),SIGNAL(connectionRangeColorationChanged(QString,QRgb*,int)),m_visualization,SLOT(setConnectionRangeColoration(QString,QRgb*,int)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(neuronRangeColorationChanged(QString,QRgb*,int)),m_visualization,SLOT(setNeuronRangeColoration(QString,QRgb*,int)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(neuronBitColorationChanged(QString,QColor,QColor)),m_visualization,SLOT(setNeuronFlagColoration(QString,QColor,QColor)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(connectionBitColorationChanged(QString,QColor,QColor)),m_visualization,SLOT(setConnectionFlagColoration(QString,QColor,QColor)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(currentConnectionAttributeSet(QString)),m_visualization,SLOT(setConnectionAttributeToRender(QString)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(currentNeuronAttributeSet(QString)),m_visualization,SLOT(setNeuronAttributeToRender(QString)));

    connect(m_visualization, SIGNAL(invalidGraphicsConfigurationDetected()),this,SLOT(m_reportFatalError()));


    QVector3D worldSize = QVector3D(50000 , 50000,50000);

    int numNeurons = 10000;


    QVector3D * neuronPositions = new QVector3D[numNeurons];

    GLfloat * neuronVoltages = new GLfloat[numNeurons];
    GLuint * neuronFirings;
    if (numNeurons %32 == 0)
        neuronFirings = new GLuint[numNeurons/32];
    else
        neuronFirings = new GLuint[(numNeurons/32) + 1];



    int fireVal;
    float threshold = 80.0f;

    for (int i = 0; i <numNeurons; i ++)
    {
        int x = (abs(rand() * rand() * rand() )% (int)worldSize.x()) - (worldSize.x()/2);
        int y = (abs(rand() * rand() * rand() ) % (int)worldSize.y()) - (worldSize.y()/2);
        int z = (abs(rand() * rand() * rand() ) % (int)worldSize.z()) - (worldSize.z()/2);
        neuronPositions[i] = QVector3D(x,y,z);
        neuronVoltages[i] = (float)(abs(rand()) % 100);
        if (neuronVoltages[i] >= threshold)
            fireVal = 1;
        else
            fireVal = 0;

        if(i %32 == 0)
            neuronFirings[i/32] = 0;
        neuronFirings[i/32] += fireVal<<(i % 32);
    }

    int numConnections = numNeurons *1 ;
    GLuint * neuronIN = new GLuint[numConnections];
    GLuint * neuronOUT = new GLuint[numConnections];
    for (int i = 0; i < numConnections; i++)
    {
        neuronIN[i] = (i % numNeurons) ;
        neuronOUT[i] = neuronIN[i];
        while (neuronOUT[i] == neuronIN[i])
            neuronOUT[i] = (rand() % numNeurons) +1;
    }

    GLfloat * connectionVoltages = new GLfloat[numConnections];
    GLuint * connectionFirings;
    if (numConnections %32 == 0)
        connectionFirings = new GLuint[numConnections/32];
    else
        connectionFirings = new GLuint[(numConnections/32) + 1];

    for (int i = 0; i <numConnections; i ++)
    {
        GLfloat inVoltage = neuronVoltages[neuronIN[i]];
        GLfloat outVoltage = neuronVoltages[neuronOUT[i]];

        connectionVoltages[i] = outVoltage - inVoltage;
        if (connectionVoltages[i] >= threshold)
            fireVal = 1;
        else
            fireVal = 0;

        if(i %32 == 0)
            connectionFirings[i/32] = 0;
        connectionFirings[i/32] += fireVal<<(i % 32);


    }



    /* ###################################################################
    Neural Network Creation Example
    ###################################################################*/
    m_visualization->createNeurons(numNeurons,neuronPositions,NULL);
    m_visualization->createConnections(numConnections,neuronIN,neuronOUT,NULL);

    m_visualization->createNeuronFlagAttribute("firing",QVector3D(1.0,0.0,0.0),QVector3D(0.0,1.0,0.0));
    m_visualization->createNeuronRangeAttribute("voltage",0,100.2f);

    m_visualization->createConnectionFlagAttribute("firing",QVector3D(1.0,0.0,0.0),QVector3D(0.0,1.0,0.0));
    m_visualization->createConnectionRangeAttribute("voltage",0,100.2f);

    m_visualization->setNeuronFlagAttribute("firing",neuronFirings);
    m_visualization->setNeuronRangeAttribute("voltage",neuronVoltages);
    m_visualization->setConnectionFlagAttribute("firing",connectionFirings);
    m_visualization->setConnectionRangeAttribute("voltage",connectionVoltages);

    /* ###################################################################
    Neural Network Example
    ###################################################################*/

    m_frameCount = 0;
    m_timer.start();
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
   //delete m_visualization;


}



void NCVWidget::m_reportFatalError()
{

    QMessageBox msgBox;
    msgBox.setText("Graphics Configuration Incapable");
    msgBox.setInformativeText("Cannot create a valid OpenGL 4.0 context.");
    msgBox.setStandardButtons(QMessageBox::Ok);

    msgBox.exec();
}
