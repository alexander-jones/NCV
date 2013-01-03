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


    connect(m_visualization,SIGNAL(newNeuronBitAttribute(QString,QColor,QColor)),m_managementSidebar->attributeWidget(),SLOT(addNeuronBitAttribute(QString,QColor,QColor)));
    connect(m_visualization,SIGNAL(newConnectionBitAttribute(QString,QColor,QColor)),m_managementSidebar->attributeWidget(),SLOT(addConnectionBitAttribute(QString,QColor,QColor)));
    connect(m_visualization,SIGNAL(newConnectionRangeAttribute(QString,float,float)),m_managementSidebar->attributeWidget(),SLOT(addConnectionRangeAttribute(QString,float,float)));
    connect(m_visualization,SIGNAL(newNeuronRangeAttribute(QString,float,float)),m_managementSidebar->attributeWidget(),SLOT(addNeuronRangeAttribute(QString,float,float)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(connectionRangeColorationChanged(QString,QRgb*,int)),m_visualization,SLOT(changeCurrentConnectionRangeColoration(QString,QRgb*,int)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(neuronRangeColorationChanged(QString,QRgb*,int)),m_visualization,SLOT(changeCurrentNeuronRangeColoration(QString,QRgb*,int)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(neuronBitColorationChanged(QString,QColor,QColor)),m_visualization,SLOT(changeCurrentNeuronFlagColoration(QString,QColor,QColor)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(connectionBitColorationChanged(QString,QColor,QColor)),m_visualization,SLOT(changeCurrentConnectionFlagColoration(QString,QColor,QColor)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(currentConnectionAttributeSet(QString)),m_visualization,SLOT(setConnectionAttributeToRender(QString)));
    connect(m_managementSidebar->attributeWidget(),SIGNAL(currentNeuronAttributeSet(QString)),m_visualization,SLOT(setNeuronAttributeToRender(QString)));

    connect(m_visualization, SIGNAL(invalidGraphicsConfigurationDetected()),this,SLOT(m_reportFatalError()));


    QVector3D worldSize = QVector3D(50000 , 50000,50000);

    int numNeurons = 50000;

    QVector3D * neuronPositions = new QVector3D[numNeurons];
    GLfloat * voltages = new GLfloat[numNeurons];
    GLubyte * firings;
    if (numNeurons %8 == 0)
        firings = new GLubyte[numNeurons/8];
    else
        firings = new GLubyte[(numNeurons/8) + 1];
    int fireVal;
    for (int i = 0; i <numNeurons; i ++)
    {
        int x = (abs(rand() * rand() * rand() )% (int)worldSize.x()) - (worldSize.x()/2);
        int y = (abs(rand() * rand() * rand() ) % (int)worldSize.y()) - (worldSize.y()/2);
        int z = (abs(rand() * rand() * rand() ) % (int)worldSize.z()) - (worldSize.z()/2);
        neuronPositions[i] = QVector3D(x,y,z);
        voltages[i] = (float)(abs(rand()) % 100);
        if (i %2 == 0)
            fireVal = 1;
        else
            fireVal = 0;
        if(i %8 == 0)
            firings[i%8] = 0;
        firings[i%8] += fireVal<<(i % 8);

    }

    int numConnections = numNeurons * 5;
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
    m_visualization->setNeuronRangeAttribute("voltage",voltages,0,100.2f);

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
