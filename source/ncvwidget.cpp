#include "ncvwidget.h"

NCVWidget::NCVWidget(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_managementSidebar= new ManagementSidebar();
    m_managementSidebar->show();
    //m_statusSidebar= new RightToolBar();
    //m_statusSidebar->show();

    // Create a GLWidget requesting our format
    m_visualization = new NCV(  );
    m_visualization->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);





    m_layout->addWidget(m_managementSidebar);
    m_layout->addWidget(m_visualization);
   // m_layout->addWidget(m_statusSidebar);

    setLayout(m_layout);
    setFocus();

    connect(m_visualization,SIGNAL(newFPSReading(float)),m_managementSidebar,SLOT(updateFPS(float)));
    connect(m_visualization,SIGNAL(newCurrentCamera(QString)),m_managementSidebar->cameraSidebar(),SLOT(switchCamera(QString)));
    connect(m_visualization,SIGNAL(newCamera(QGLXCamera*,QString)),m_managementSidebar->cameraSidebar(),SLOT(addCamera(QGLXCamera*,QString)));
    connect(m_visualization,SIGNAL(cameraUpdated(QString)),m_managementSidebar->cameraSidebar(),SLOT(updateCamera(QString)));

    connect(m_managementSidebar->cameraSidebar(),SIGNAL(cameraCreated(QGLXCamera*,QString)),m_visualization,SLOT(addCamera(QGLXCamera*,QString)));
    connect(m_managementSidebar->cameraSidebar(),SIGNAL(cameraSwitched(QString)),m_visualization,SLOT(switchCamera(QString)));
    connect(m_managementSidebar->cameraSidebar(),SIGNAL(cameraDeleted(QString)),m_visualization,SLOT(deleteCamera(QString)));

    connect(m_managementSidebar->lightingSidebar(),SIGNAL(lightCreated(QGLXLight *,QString)),m_visualization,SLOT(addLight(QGLXLight *,QString)));
    connect(m_managementSidebar->lightingSidebar(),SIGNAL(lightDeleted(QString)),m_visualization,SLOT(deleteLight(QString)));
    connect(m_managementSidebar->lightingSidebar(),SIGNAL(newMaterial(QGLXMaterial*)),m_visualization,SLOT(setMaterial(QGLXMaterial*)));

    connect(m_visualization, SIGNAL(invalidGraphicsConfigurationDetected()),this,SLOT(m_reportFatalError()));


    QVector3D worldSize = QVector3D(50000 , 50000,50000);

    int numNeurons = 1000;

    QVector3D * neuronPositions = new QVector3D[numNeurons];
    GLfloat * voltages = new GLfloat[numNeurons];
    GLubyte * firings = new GLubyte[numNeurons/8];
    for (int i = 0; i <numNeurons; i ++)
    {
        int x = (abs(rand() * rand() * rand() )% (int)worldSize.x()) - (worldSize.x()/2);
        int y = (abs(rand() * rand() * rand() ) % (int)worldSize.y()) - (worldSize.y()/2);
        int z = (abs(rand() * rand() * rand() ) % (int)worldSize.z()) - (worldSize.z()/2);
        neuronPositions[i] = QVector3D(x,y,z);
        voltages[i] = (float)(abs(rand()) % 100);
        if(i %8 == 0)
            firings[i%8] = 0;
        firings[i%8] += 1<<(i % 8);

    }

    int numConnections = numNeurons *1;
    GLuint * neuronIN = new GLuint[numConnections];
    GLuint * neuronOUT = new GLuint[numConnections];
    for (int i = 0; i < numConnections; i++)
    {
        neuronIN[i] = (i % numNeurons) +1;
        neuronOUT[i] = neuronIN[i];
        while (neuronOUT[i] == neuronIN[i])
            neuronOUT[i] = (rand() % numNeurons) +1;
    }

    /* ###################################################################
    Neural Network Creation Example
    ###################################################################*/
    m_visualization->createNeurons(numNeurons,neuronPositions);
    m_visualization->createConnections(numConnections,neuronIN,neuronOUT);

    // attach voltage data to Inst_Voltage parameter
    // voltage data is composed of 3 component float vector, so specify that
    // share data with both neuron / connection shaders
    m_visualization->setNeuronFlagAttribute("firing",firings,QVector3D(1.0,0.0,0.0),QVector3D(0.0,1.0,0.0));
    m_visualization->setNeuronRangeAttribute("voltage",voltages,0,100.0f);
    m_visualization->setNeuronAttributeToRender("voltage");
    m_visualization->setConnectionAttributeToRender("firing");
    /* ###################################################################
    Neural Network Example
    ###################################################################*/

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
