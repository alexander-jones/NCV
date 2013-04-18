#include "mainwindow.h"
#include "ui_mainwindow.h"
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
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_neurons = NULL;
    m_connections = NULL;
    ui->setupUi(this);

    this->setWindowTitle("NCV");
    m_rootPath = QDir::homePath() + "/NCV";
    if (!QDir(m_rootPath).exists())
        QDir().mkpath(m_rootPath);

    if (!QDir("./user").exists())
        QDir(".").mkdir("./user");

    if (!QDir("./tmp").exists())
        QDir(".").mkdir("./tmp");


    m_menuBar = new QMenuBar();
    m_fileMenu = new QMenu(tr("File"));
    QAction * openProjAction  = m_fileMenu->addAction("Open Project");
    connect(openProjAction,SIGNAL(triggered()),this,SLOT(m_openProjectPressed()));
    QAction * newProjAction  = m_fileMenu->addAction("New Project");
    connect(newProjAction,SIGNAL(triggered()),this,SLOT(m_newProjectPressed()));
    m_fileMenu->addAction("Quit");
    m_menuBar->addMenu(m_fileMenu);

    m_editMenu = new QMenu(tr("Edit"));
   // m_menuBar->addMenu(m_editMenu);

    m_toolsMenu = new QMenu(tr("Tools"));
   // m_menuBar->addMenu(m_toolsMenu);
    setMenuBar(m_menuBar);

    m_tabWidget = NULL;

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

    m_tabWidget = new QwwConfigWidget();
    m_tabWidget->setIconSize(QSize(64,64));

    this->setCentralWidget(m_tabWidget);

    //NCVBuildWidget * buildWidget = new NCVBuildWidget(projectDirectory,m_tabWidget);
    //addWidget(buildWidget);

    m_connectionWidget = new NCSConnectionWidget(projectDirectory,m_tabWidget);
    addWidget(m_connectionWidget);

    NCSClusterEditor * clusterEditor = new NCSClusterEditor(projectDirectory,m_tabWidget);
    addWidget(clusterEditor);

    NCSModelDistributionWidget * modelWidget = new NCSModelDistributionWidget(projectDirectory,m_tabWidget);
    connect(modelWidget,SIGNAL(distributed(QString)),this,SLOT(m_createNetwork(QString)));
    connect(modelWidget,SIGNAL(launched()),this,SLOT(m_publishNetwork()));
    addWidget(modelWidget);

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

    QVector<QString> typeStates;
    typeStates.append("Ovoid");
    typeStates.append("Izekivich");
    typeStates.append("Pyramidal");
    typeStates.append("Granule");
    typeStates.append("Double Pyramidal");

    // establish attribute  colorations
    ContinuousColorSelector * voltageRange = new ContinuousColorSelector();
    voltageRange->setLowThreshold(-80);
    voltageRange->setHighThreshold(40);
    voltageRange->addMarker(-80 ,QColor(0,255,0));
    voltageRange->addMarker(-20,QColor(0,0,255));
    voltageRange->addMarker(40,QColor(255,0,0));


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

    int numNeurons;
    dataStream >> numNeurons;
    qDebug() << "Neurons Created:" <<numNeurons;
    for (int i = 0; i <numNeurons; i ++)
    {
        int x = (abs(rand() * rand() * rand() )% (int)worldSize.x()) - (worldSize.x()/2);
        int y = (abs(rand() * rand() * rand() ) % (int)worldSize.y()) - (worldSize.y()/2);
        int z = (abs(rand() * rand() * rand() ) % (int)worldSize.z()) - (worldSize.z()/2);
        neuronPositions.append(QVector3D(x, y, z));
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

    m_continuousNeuronAttributes["voltage"] = new NCVContinuousAttribute(Neuron,voltageRange->lowThreshold(), voltageRange->highThreshold());
    m_continuousNeuronAttributes["voltage"]->attachData(neuronVoltages);
    m_continuousNeuronAttributes["voltage"]->attachColoration(voltageRange->getData());
    m_neurons->addAttribute("voltage (mV)",m_continuousNeuronAttributes["voltage"]);

    m_discreteNeuronAttributes["firing"] = new NCVDiscreteAttribute(Neuron, firingStates);
    m_discreteNeuronAttributes["firing"]->attachData(neuronFirings,1);
    m_discreteNeuronAttributes["firing"]->attachColoration(firingColors);
    m_neurons->addAttribute("firing",m_discreteNeuronAttributes["firing"]);

    m_discreteNeuronAttributes["type"] = new NCVDiscreteAttribute(Neuron,typeStates);
    m_discreteNeuronAttributes["type"]->attachData(neuronTypes);
    m_discreteNeuronAttributes["type"]->attachColoration(typeColors);
    m_neurons->addAttribute("type",m_discreteNeuronAttributes["type"]);

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
    m_connections = new NCVConnectionSet(m_neurons, connections);

    m_continuousConnectionAttributes["voltage"] = new NCVContinuousAttribute(Connection, voltageRange->lowThreshold(), voltageRange->highThreshold());
    m_continuousConnectionAttributes["voltage"]->attachData(connectionVoltages);
    m_continuousConnectionAttributes["voltage"]->attachColoration(voltageRange->getData());
    m_connections->addAttribute("voltage (mV)", m_continuousConnectionAttributes["voltage"]);

    m_discreteConnectionAttributes["firing"] = new NCVDiscreteAttribute(Connection, firingStates);
    m_discreteConnectionAttributes["firing"]->attachData(connectionFirings, 1);
    m_discreteConnectionAttributes["firing"]->attachColoration(firingColors);
    m_connections->addAttribute("firing", m_discreteConnectionAttributes["firing"]);

    m_publishNetwork();
}

void MainWindow::m_publishNetwork()
{
    m_visualizationWidget->setNeurons(m_neurons);
    m_visualizationWidget->setConnections(m_connections);
}

MainWindow::~MainWindow()
{

    delete ui;
}
