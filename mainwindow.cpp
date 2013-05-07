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

    setMenuBar(m_menuBar);

    m_updateTimer = new QTimer(this);


    m_simulationToolbar = this->addToolBar("Simulation");
    m_runSimulationButton = m_simulationToolbar->addAction(QIcon(":/media/playIcon.png"), "Run Simulation");
    connect(m_runSimulationButton,SIGNAL(triggered()),this,SLOT(m_runSimulation()));
    m_stopSimulationButton = m_simulationToolbar->addAction(QIcon(":/media/stopIcon.png"), "Stop Simulation");
    connect(m_stopSimulationButton,SIGNAL(triggered()),this,SLOT(m_stopSimulation()));
    m_simulationTimeSlider = new QSlider(this);
    m_simulationTimeSlider->setOrientation(Qt::Horizontal);
    m_simulationTimeSlider->setMinimum(1);
    m_simulationTimeSlider->setValue(100);
    m_simulationTimeSlider->setMaximum(5000);
    connect(m_simulationTimeSlider,SIGNAL(sliderMoved(int)),m_updateTimer,SLOT(start(int)));
    m_simulationToolbar->addWidget(m_simulationTimeSlider);
    m_simulationLoadingLabel = new QLabel();
    m_simulationLoadingMovie = new QMovie(":/media/loading.gif");
    m_simulationToolbar->addWidget(m_simulationLoadingLabel);

    m_setSimulationToolbar(false);


    m_applicationMapper = new QSignalMapper(this);
    connect(m_applicationMapper,SIGNAL(mapped(QObject*)),this,SLOT(m_ncsApplicationFinished(QObject*)));
    m_tabWidget = NULL;
    m_commandBridge = NULL;
    m_dataSource = NULL;
    m_simulationApplicationIndex = -1;

}

void MainWindow::m_runSimulation()
{
    disconnect(m_runSimulationButton,SIGNAL(triggered()),this,SLOT(m_runSimulation()));
    connect(m_runSimulationButton,SIGNAL(triggered()),this,SLOT(m_pauseSimulation()));
    connect(m_simulationTimeSlider,SIGNAL(sliderMoved(int)),m_updateTimer,SLOT(start(int)));
    m_runSimulationButton->setIcon(QIcon(":/media/pauseIcon.png"));
    m_runSimulationButton->setText("Pause Simulation");
    if (!m_updateTimer->isActive())
        m_updateTimer->start(m_simulationTimeSlider->value());
    m_stopSimulationButton->setEnabled(true);
}

void MainWindow::m_pauseSimulation()
{
    disconnect(m_runSimulationButton,SIGNAL(triggered()),this,SLOT(m_pauseSimulation()));
    connect(m_runSimulationButton,SIGNAL(triggered()),this,SLOT(m_runSimulation()));
    disconnect(m_simulationTimeSlider,SIGNAL(sliderMoved(int)),m_updateTimer,SLOT(start(int)));
    m_runSimulationButton->setIcon(QIcon(":/media/playIcon.png"));
    m_runSimulationButton->setText("Run Simulation");
    m_updateTimer->stop();
    m_stopSimulationButton->setEnabled(true);
}

void  MainWindow::m_stopSimulation()
{
    m_pauseSimulation();

    QMessageBox msgBox;
    msgBox.setText("Stop the simulation?" );
    msgBox.addButton("Continue", QMessageBox::ActionRole);
    QPushButton * cancelButton= msgBox.addButton("Cancel", QMessageBox::ActionRole);
    msgBox.exec();
    if (msgBox.clickedButton() == cancelButton)
        return;

    delete m_dataSource;

    if (m_connections != NULL)
        m_visualizationWidget->setConnections(NULL);

    m_visualizationWidget->setNeurons(NULL);

    m_activeApplications[m_simulationApplicationIndex]->setParent(0);
    NCSApplicationBridge * app = m_activeApplications[m_simulationApplicationIndex];
    m_activeApplications.remove(m_activeApplications.indexOf(app));
    app->scheduleDestruction(true);

    m_simulationApplicationIndex = -1;
    m_setSimulationToolbar(false);



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
    connect(m_connectionWidget,SIGNAL(bridgeEstablished(NCSCommandBridge*)),this,SLOT(m_setCommandBridge(NCSCommandBridge*)));

    NCSClusterEditor * clusterEditor = new NCSClusterEditor(projectDirectory,m_tabWidget);
    addWidget(clusterEditor);

    LIFModelDistributionWidget * modelWidget = new LIFModelDistributionWidget(projectDirectory,m_tabWidget);
    connect(modelWidget,SIGNAL(launchTriggered()),this,SLOT(m_showLoadingSimulation()));
    connect(modelWidget,SIGNAL(distributed(QString)),this,SLOT(m_createNetwork(QString)));
    connect(modelWidget,SIGNAL(launched()),this,SLOT(m_publishNetwork()));
    addWidget(modelWidget);

    IzhModelDistributionWidget * izhModelWidget= new IzhModelDistributionWidget(projectDirectory,m_tabWidget);
    connect(izhModelWidget,SIGNAL(launchTriggered()),this,SLOT(m_showLoadingSimulation()));
    connect(izhModelWidget,SIGNAL(distributed(QString)),this,SLOT(m_createNetwork(QString)));
    connect(izhModelWidget,SIGNAL(launched()),this,SLOT(m_publishNetwork()));
    addWidget(izhModelWidget);


    m_visualizationWidget = new NCVWidget(projectDirectory,m_tabWidget);
    addWidget(m_visualizationWidget);

}

void MainWindow::m_showLoadingSimulation()
{
    m_simulationLoadingLabel->setMovie(m_simulationLoadingMovie);
    m_simulationLoadingMovie->start();
}

void MainWindow::addWidget(NCSWidgetPlugin *widget)
{
    m_tabWidget->addGroup(widget,widget->icon(),widget->title());
}
void MainWindow::addWidget(NCSApplicationWidgetPlugin *widget)
{
    connect(m_connectionWidget,SIGNAL(bridgeEstablished(NCSCommandBridge*)),widget,SLOT(initialize(NCSCommandBridge*)));
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

void MainWindow::closeEvent(QCloseEvent * event)
{
    while (m_activeApplications.count() > 0)
    {
        QVector<NCSApplicationBridge*>::iterator it = m_activeApplications.begin();
        NCSApplicationBridge* app = *it;
        QMessageBox msgBox;
        msgBox.setText("The NCS application '" + app->applicationName() + "' is still running.\n Should this application be closed or left running?"  );
        QPushButton * leaveOpenButton =  msgBox.addButton("Leave Running", QMessageBox::ActionRole);
        msgBox.addButton("Close", QMessageBox::ActionRole);
        msgBox.exec();
        m_activeApplications.remove(m_activeApplications.indexOf(app));

        if (msgBox.clickedButton() == leaveOpenButton)
            app->scheduleDestruction(false);
        else
            app->scheduleDestruction(true);
    }
}

void MainWindow::m_setCommandBridge(NCSCommandBridge * bridge)
{
    m_commandBridge = bridge;
    connect(m_commandBridge,SIGNAL(applicationStarted(NCSApplicationBridge*)),this,SLOT(m_ncsApplicationLaunched(NCSApplicationBridge*)));
}

void MainWindow::m_ncsApplicationLaunched(NCSApplicationBridge * app)
{

    m_activeApplications.append(app);
    if (app->applicationName() == "simulator")
        m_simulationApplicationIndex = m_activeApplications.count() -1;

    m_applicationMapper->setMapping(app,app);
    connect(app,SIGNAL(executionFinished()), m_applicationMapper,SLOT(map()));
}

void MainWindow::m_ncsApplicationFinished(QObject * app)
{
    NCSApplicationBridge * application = dynamic_cast<NCSApplicationBridge *> (app);
    disconnect(application,SIGNAL(executionFinished()), m_applicationMapper,SLOT(map()));
    int index = m_activeApplications.indexOf(application);
    if (index != -1)
        m_activeApplications.remove(index);
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

    QVector<QString> firingStates;
    firingStates.append("Off");
    firingStates.append("On");
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
    QVector<float> stubVoltage;
    stubVoltage.resize(numNeurons);

    // create neurons and related attributes
    m_neurons= new NCSNeuronSet(neuronPositions);

    m_continuousNeuronAttributes["voltage"] = new NCSContinuousAttribute(Neuron,-80, 40);
    m_continuousNeuronAttributes["voltage"]->attachData(stubVoltage);
    m_neurons->addAttribute("neuronVoltage",m_continuousNeuronAttributes["voltage"]);


    // create stub firings so that attribute data array is at least present before simulation updates.
    QVector<unsigned char> stubNeuronFirings;
    if (numNeurons % 8 == 0)
        stubNeuronFirings.resize(numNeurons / 8);
    else
        stubNeuronFirings.resize((numNeurons / 8) + 1);

    m_discreteNeuronAttributes["firing"] = new NCSDiscreteAttribute(Neuron, firingStates,1);
    m_discreteNeuronAttributes["firing"]->attachData(stubNeuronFirings);
    m_discreteNeuronAttributes["firing"]->setReportable(false);
    m_neurons->addAttribute("firing", m_discreteNeuronAttributes["firing"]);

    int numConnections;
    dataStream >> numConnections;
    qDebug() <<  "Connections Created:" << numConnections;
    if (numConnections ==0)
    {
        topologyFile.close();
        return;
    }
    // create stub connection data
    QVector<NCSConnection> connections;
    unsigned int inNeuron, outNeuron;
    for (int i = 0; i < numConnections; i++)
    {
        dataStream >> inNeuron >> outNeuron;
        connections.append(NCSConnection(inNeuron,outNeuron));
    }
    // create stub firings so that attribute data array is at least present before simulation updates.
    QVector<unsigned char> stubFirings;
    if (numConnections % 8 == 0)
        stubFirings.resize(numConnections / 8);
    else
        stubFirings.resize((numConnections / 8) + 1);

    //create connections and related attributes
    m_connections = new NCSConnectionSet(m_neurons, connections);

    m_discreteConnectionAttributes["firing"] = new NCSDiscreteAttribute(Connection, firingStates,1);
    m_discreteConnectionAttributes["firing"]->attachData(stubFirings);
    m_discreteConnectionAttributes["firing"]->setReportable(false);
    m_connections->addAttribute("synapseFire", m_discreteConnectionAttributes["firing"]);


    // create stub voltage so that attribute data array is at least present before simulation updates.
    QVector<float> stubConnVoltage;
    stubConnVoltage.resize(numConnections);

    m_continuousConnectionAttributes["voltage"] = new NCSContinuousAttribute(Connection,-80, 40);
    m_continuousConnectionAttributes["voltage"]->attachData(stubConnVoltage);
    m_continuousConnectionAttributes["voltage"]->setReportable(false);
    m_connections->addAttribute("connVoltage",m_continuousConnectionAttributes["voltage"]);

    topologyFile.close();
}

void MainWindow::m_publishNetwork()
{

    m_dataSource = new NCSDataSource();
    qDebug() << m_commandBridge->hostname();
    qDebug() << "NCSDataSource connected:" << m_dataSource->establishConnection(m_commandBridge->hostname().toStdString(),8951);
    m_dataSource->replaceNeuronSet(m_neurons);
    if (m_connections != NULL)
        m_dataSource->replaceConnectionSet(m_connections);
    connect(m_updateTimer,SIGNAL(timeout()),m_dataSource,SLOT(updateAttributes()));

    m_visualizationWidget->setNeurons(m_neurons);
    if (m_connections != NULL)
        m_visualizationWidget->setConnections(m_connections);
    m_simulationLoadingMovie->stop();
    m_simulationLoadingLabel->clear();
    m_setSimulationToolbar(true);
}

void MainWindow::m_setSimulationToolbar(bool on)
{
    m_runSimulationButton->setEnabled(on);
    m_simulationTimeSlider->setEnabled(on);
    m_stopSimulationButton->setEnabled(on);
    m_simulationLoadingLabel->setEnabled(!on);
}

MainWindow::~MainWindow()
{
}
