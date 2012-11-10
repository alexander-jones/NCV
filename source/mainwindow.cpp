#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
#endif
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGLFormat glFormat;
    glFormat.setVersion( 4,0 );
    glFormat.setDoubleBuffer(true);
    glFormat.setProfile( QGLFormat::CompatibilityProfile ); // Requires >=Qt-4.8.0
    glFormat.setSampleBuffers( true );
    qDebug() << "Running OpenGL Version " <<glFormat.majorVersion() << "." <<glFormat.minorVersion();

    // Create a GLWidget requesting our format
    NCV *ncvSim = new NCV( glFormat,this );
    this->setCentralWidget(ncvSim);
    QVector3D worldSize = QVector3D(200000 , 200000,200000);

    int numNeurons = 10000;

    QVector3D * neruonPositions = new QVector3D[numNeurons];
    GLfloat * voltages = new GLfloat[numNeurons];
    GLubyte * firings = new GLubyte[numNeurons];
    for (int i = 0; i <numNeurons; i ++)
    {
        int x = (abs(rand() * rand() * rand() )% (int)worldSize.x()) - (worldSize.x()/2);
        int y = (abs(rand() * rand() * rand() ) % (int)worldSize.y()) - (worldSize.y()/2);
        int z = (abs(rand() * rand() * rand() ) % (int)worldSize.z()) - (worldSize.z()/2);
        neruonPositions[i] = QVector3D(x,y,z);
        voltages[i] = (float)(abs(rand()) % 100);
        firings[i] = i % 2;
    }

    int numConnections = numNeurons * 5;
    GLuint * nueronIN = new GLuint[numConnections];
    GLuint * nueronOUT = new GLuint[numConnections];
    for (int i = 0; i < numConnections; i++)
    {
        nueronIN[i] = (i % numNeurons) +1;
        nueronOUT[i] = nueronIN[i];
        while (nueronOUT[i] == nueronIN[i])
            nueronOUT[i] = (rand() % numNeurons) +1;
    }

    /* ###################################################################
    Neural Network Creation Example
    ###################################################################*/
    ncvSim->createNeurons(numNeurons,neruonPositions);
    ncvSim->createConnections(numConnections,nueronIN,nueronOUT);

    // attach voltage data to Inst_Voltage parameter
    // voltage data is composed of 3 component float vector, so specify that
    // share data with both neuron / connection shaders
    // high precision format for data (more expensive but more accurate)
    ncvSim->setNeuronAttributeArray("Inst_Voltage",voltages,GL_FLOAT,3,NCV::Shared);
    ncvSim->setNeuronAttributeArray("Inst_Firing",firings,GL_UNSIGNED_BYTE,1,NCV::Shared);
    ncvSim->setVisualizationParameter("MaxVoltage",100.0f,NCV::Shared);
    ncvSim->setVisualizationParameter("MinVoltage",0.0f,NCV::Shared);
    ncvSim->setVisualizationParameter("FireColor",QVector3D(1.0,0.0,0.0),NCV::Shared);
    ncvSim->setVisualizationParameter("IdleColor",QVector3D(0.0,1.0,0.0),NCV::Shared);
    /* ###################################################################
    Neural Network Example
    ###################################################################*/

}

MainWindow::~MainWindow()
{
    delete ui;
}
