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
    NCVGLWidget *ncvSim = new NCVGLWidget( glFormat,this );
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
    ncvSim->setNeuronAttribute("Inst_Voltage",voltages,sizeof(GLfloat),GL_R32F,NCVGLWidget::Shared);
    ncvSim->setNeuronAttribute("Inst_Firing",firings,sizeof(GLubyte),GL_R8,NCVGLWidget::Shared);
    ncvSim->setVisualizationParameter("MaxVoltage",100.0f,NCVGLWidget::Shared);
    ncvSim->setVisualizationParameter("MinVoltage",0.0f,NCVGLWidget::Shared);
    ncvSim->setVisualizationParameter("FireColor",QVector3D(1.0,0.0,0.0),NCVGLWidget::Shared);
    ncvSim->setVisualizationParameter("IdleColor",QVector3D(0.0,1.0,0.0),NCVGLWidget::Shared);
    /* ###################################################################
    Neural Network Example
    ###################################################################*/

}

MainWindow::~MainWindow()
{
    delete ui;
}
