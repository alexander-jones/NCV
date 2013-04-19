#include "mainwindow.h"
#include <QMenuBar>
#include <QFileDialog>
bool removeDir(const QString & dirName)
{
    bool result;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_neurons = NULL;
    m_connections = NULL;

    this->setWindowTitle("NCV");
    m_rootPath = QDir::homePath() + "/NCV";
    if (!QDir(m_rootPath).exists())
        QDir().mkpath(m_rootPath);

    if (!QDir("./user").exists())
        QDir(".").mkdir("./user");

    if (!QDir("./tmp").exists())
        QDir(".").mkdir("./tmp");

    m_menuBar = new QMenuBar(this);
    m_fileMenu = new QMenu(tr("File"),this);
    QAction * openProjAction  = m_fileMenu->addAction("Open Project");
    connect(openProjAction,SIGNAL(triggered()),this,SLOT(m_openProjectPressed()));
    QAction * newProjAction  = m_fileMenu->addAction("New Project");
    connect(newProjAction,SIGNAL(triggered()),this,SLOT(m_newProjectPressed()));
    m_fileMenu->addAction("Quit");
    m_menuBar->addMenu(m_fileMenu);

    m_editMenu = new QMenu(tr("Edit"),this);
   // m_menuBar->addMenu(m_editMenu);

    m_toolsMenu = new QMenu(tr("Tools"),this);
   // m_menuBar->addMenu(m_toolsMenu);
    setMenuBar(m_menuBar);

    m_updateTimer = new QTimer(this);
    m_tabWidget = NULL;
    m_commandBridge = NULL;
    m_dataSource = NULL;

}

void MainWindow::m_openProjectPressed()
{

    QString projDir = QFileDialog::getExistingDirectory(this,"Open Project File",m_rootPath);
    if (projDir != "")
        m_loadProject(projDir);
}

void MainWindow::m_newProjectPressed()
{
    QString projName = QInputDialog::getText(this,"Create New Project","What would you like to name this project?");
    if (projName != "")
    {
        if (QDir(m_rootPath + "/" + projName).exists())
        {
            QMessageBox msgBox;
            msgBox.setText("A project with this name already exists. Creating this project will delete the existing project." );
            msgBox.addButton("Continue", QMessageBox::ActionRole);
            QPushButton * cancelButton= msgBox.addButton("Cancel", QMessageBox::ActionRole);
            msgBox.exec();
            if (msgBox.clickedButton() == cancelButton)
                return;
            else
                QDir(m_rootPath).rmdir(projName);
        }

        QDir(m_rootPath).mkdir(projName);
        m_loadProject(m_rootPath + "/" + projName);
    }
}

void MainWindow::m_loadProject(QString projectDirectory)
{

    if (QDir(projectDirectory + "/tmp").exists())
        removeDir(projectDirectory+"/tmp");

    QDir(projectDirectory).mkdir("tmp");

    if (m_tabWidget != NULL)
        delete m_tabWidget;

    m_tabWidget = new QwwConfigWidget(this);
    m_tabWidget->setIconSize(QSize(64,64));

    this->setCentralWidget(m_tabWidget);

    //NCVBuildWidget * buildWidget = new NCVBuildWidget(projectDirectory,m_tabWidget);
    //addWidget(buildWidget);

    m_connectionWidget = new NCSConnectionWidget(projectDirectory,m_tabWidget);
    addWidget(m_connectionWidget);
    connect(m_connectionWidget,SIGNAL(validNCSBridgeEstablished(NCSCommandBridge*)),this,SLOT(m_setCommandBridge(NCSCommandBridge*)));

    NCSClusterEditor * clusterEditor = new NCSClusterEditor(projectDirectory,m_tabWidget);
    addWidget(clusterEditor);

    NCSModelDistributionWidget * modelWidget = new NCSModelDistributionWidget(projectDirectory,m_tabWidget);
    connect(modelWidget,SIGNAL(distributed(QString)),this,SLOT(m_createNetwork(QString)));
    connect(modelWidget,SIGNAL(launched()),this,SLOT(m_publishNetwork()));
    addWidget(modelWidget);

    IzhModelDistributionWidget * izhModelWidget= new IzhModelDistributionWidget(projectDirectory,m_tabWidget);
    connect(izhModelWidget,SIGNAL(distributed(QString)),this,SLOT(m_createNetwork(QString)));
    connect(izhModelWidget,SIGNAL(launched()),this,SLOT(m_publishNetwork()));
    addWidget(izhModelWidget);


    m_visualizationWidget = new NCVWidget(projectDirectory,m_tabWidget);
    addWidget(m_visualizationWidget);

}

void MainWindow::addWidget(NCSWidgetPlugin *widget)
{
    m_tabWidget->addGroup(widget,widget->icon(),widget->title());
}
void MainWindow::addWidget(NCSApplicationWidgetPlugin *widget)
{
    connect(m_connectionWidget,SIGNAL(validNCSBridgeEstablished(NCSCommandBridge*)),widget,SLOT(initialize(NCSCommandBridge*)));
    addWidget((NCSWidgetPlugin *)widget);
}
void MainWindow::addWidget(NCSDistributionWidgetPlugin *widget)
{
    addWidget((NCSApplicationWidgetPlugin *)widget);
}

void MainWindow::m_distributionFinalized()
{
    //m_tabWidget->setCurrentGroup(m_visualizationWidget);
    //m_distributeWidget->setEnabled(false);
}

void MainWindow::m_updateTimeScale(int value)
{
    float multiplier = (float)value / (float)m_timeScaleSlider->maximum();
    QString str = "Time Scale:";
    str.append(QString(" %1 X").arg(multiplier));
    m_timeScale->setText(str);

}

void MainWindow::m_setCommandBridge(NCSCommandBridge * bridge)
{
    m_commandBridge = bridge;
}

void MainWindow::m_createNetwork(QString topologyFilename)
{
    QFile topologyFile(topologyFilename);
    if (!topologyFile.exists())
        return;

    topologyFile.open(QIODevice::ReadOnly);
    QDataStream dataStream(&topologyFile);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    QVector3D worldSize = QVector3D(50000,50000,50000);
    // establish discrete attribute states;
    QVector<QString> firingStates;
    firingStates.append("Off");
    firingStates.append("On");


    QMap<QString,QColor >firingColors;
    firingColors["Off"] =QColor(0,255,0);
    firingColors["On"] = QColor(255,0,0);

    // establish attribute  colorations
    ContinuousColorSelector * voltageRange = new ContinuousColorSelector();
    voltageRange->setLowThreshold(-80);
    voltageRange->setHighThreshold(40);
    voltageRange->addMarker(-80 ,QColor(0,255,0));
    voltageRange->addMarker(-20,QColor(0,0,255));
    voltageRange->addMarker(40,QColor(255,0,0));


    // create stub neuron data source
    QVector<QVector3D> neuronPositions;

    int numNeurons;
    dataStream >> numNeurons;
    qDebug() << "Neurons Created:" <<numNeurons;
    for (int i = 0; i <numNeurons; i ++)
    {
        int x = (abs(rand() * rand() * rand() )% (int)worldSize.x()) - (worldSize.x()/2);
        int y = (abs(rand() * rand() * rand() ) % (int)worldSize.y()) - (worldSize.y()/2);
        int z = (abs(rand() * rand() * rand() ) % (int)worldSize.z()) - (worldSize.z()/2);
        neuronPositions.append(QVector3D(x, y, z));
    }

    // create stub voltage so that attribute data array is at least present before simulation updates.
    QVector<GLfloat> stubVoltage;
    stubVoltage.resize(numNeurons);


    // create neurons and related attributes
    m_neurons= new NCVNeuronSet(neuronPositions);

    m_continuousNeuronAttributes["voltage"] = new NCVContinuousAttribute(Neuron,voltageRange->lowThreshold(), voltageRange->highThreshold());
    m_continuousNeuronAttributes["voltage"]->attachData(stubVoltage);
    m_continuousNeuronAttributes["voltage"]->attachColoration(voltageRange->getData());
    m_neurons->addAttribute("neuronVoltage",m_continuousNeuronAttributes["voltage"]);


    int numConnections;
    dataStream >> numConnections;
    qDebug() <<  "Connections Created:" << numConnections;
    // create stub connection data
    QVector<NeuronConnection> connections;
    GLuint inNeuron, outNeuron;
    for (int i = 0; i < numConnections; i++)
    {
        dataStream >> inNeuron >> outNeuron;
        connections.append(NeuronConnection(inNeuron,outNeuron));
    }
    // create stub firings so that attribute data array is at least present before simulation updates.
    QVector<GLubyte> stubFirings;
    if (numConnections % 8 == 0)
        stubFirings.resize(numConnections / 8);
    else
        stubFirings.resize((numConnections / 8) + 1);

    //create connections and related attributes
    m_connections = new NCVConnectionSet(m_neurons, connections);

    m_discreteConnectionAttributes["firing"] = new NCVDiscreteAttribute(Connection, firingStates);
    m_discreteConnectionAttributes["firing"]->attachData(stubFirings,1);
    m_discreteConnectionAttributes["firing"]->attachColoration(firingColors);
    m_connections->addAttribute("synapseFire", m_discreteConnectionAttributes["firing"]);

}

void MainWindow::m_publishNetwork()
{
    if (m_dataSource != NULL)
        delete m_dataSource;
    m_dataSource = new NCSDataSource();
    m_dataSource->establishConnection(m_commandBridge->hostname().toStdString(),8951);
    m_dataSource->replaceNeuronSet(m_neurons);
    m_dataSource->replaceConnectionSet(m_connections);
    connect(m_updateTimer,SIGNAL(timeout()),m_dataSource,SLOT(updateCurrentAttributes()));
    m_visualizationWidget->setNeurons(m_neurons);
    m_visualizationWidget->setConnections(m_connections);
    m_updateTimer->start(50);
}

MainWindow::~MainWindow()
{
}
