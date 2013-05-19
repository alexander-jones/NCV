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


    m_reportingManager = new NetworkUpdateManager(this);


    m_simulationToolbar = this->addToolBar("Simulation");
    m_simulationLoadingLabel = new QLabel();
    m_simulationLoadingMovie = new QMovie(":/media/loading.gif");
    m_simulationToolbar->addWidget(m_simulationLoadingLabel);
    m_simulationToolbar->setMovable(false);
    m_runSimulationButton = m_simulationToolbar->addAction(QIcon(":/media/playIcon.png"), "Run Simulation");
    connect(m_runSimulationButton,SIGNAL(triggered()),this,SLOT(m_runSimulationPressed()));
    m_stopSimulationButton = m_simulationToolbar->addAction(QIcon(":/media/stopIcon.png"), "Stop Simulation");
    connect(m_stopSimulationButton,SIGNAL(triggered()),this,SLOT(m_stopSimulationPressed()));
    m_simulationTimeSlider = new QSlider(this);
    m_simulationTimeSlider->setToolTip("Adjust the  Update Interval");
    m_simulationTimeSlider->setOrientation(Qt::Horizontal);
    m_simulationTimeSlider->setMinimum(1);
    m_simulationTimeSlider->setValue(100);
    m_simulationTimeSlider->setMaximum(5000);
    connect(m_simulationTimeSlider,SIGNAL(sliderMoved(int)),this,SLOT(m_updateTimeScale(int)));
    m_simulationToolbar->addWidget(m_simulationTimeSlider);
    m_timeScaleLabel = new QLabel();
    m_timeScaleLabel->setTextFormat(Qt::RichText);
    m_timeScaleLabel->setText(" ( <b>" + QString::number(m_simulationTimeSlider->value()) +"</b> msec ) ");
    m_timeScaleLabel->setEnabled(false);
    m_simulationToolbar->addWidget(m_timeScaleLabel);
    m_ncsInstallationLabel = new QLabel();
    m_ncsInstallationLabel->setText("<b> [ Unconnected ] </b>");
    m_ncsInstallationLabel->setTextFormat(Qt::RichText);
    m_simulationToolbar->addWidget(m_ncsInstallationLabel);

    m_setSimulationToolbar(false);


    m_applicationMapper = new QSignalMapper(this);
    connect(m_applicationMapper,SIGNAL(mapped(QObject*)),this,SLOT(m_ncsApplicationFinished(QObject*)));
    m_applicationLauncher = NULL;
    m_commandBridge = NULL;
    m_simulationApplicationIndex = -1;



    m_applicationLauncher = new QxtConfigWidget(this);
    m_applicationLauncher->setIconSize(QSize(64,64));

    NCSConnectionWidget * connectionWidget = new NCSConnectionWidget(m_applicationLauncher);
    addPlugin(connectionWidget);

    NCSClusterEditor * clusterEditor = new NCSClusterEditor(m_applicationLauncher);
    addPlugin(clusterEditor);

    LIFModelDistributionWidget * modelWidget = new LIFModelDistributionWidget(m_applicationLauncher);
    addPlugin(modelWidget);

    IzhModelDistributionWidget * izhModelWidget= new IzhModelDistributionWidget(m_applicationLauncher);
    addPlugin(izhModelWidget);

    NCVWidget * visualizationWidget = new NCVWidget(m_applicationLauncher);
    addPlugin(visualizationWidget);

    this->setCentralWidget(m_applicationLauncher);
    m_applicationLauncher->setEnabled(false);
}



void MainWindow::addPlugin(NCSWidgetPlugin *widget)
{
    m_allPlugins.append(widget);
    int index = m_applicationLauncher->addPage(widget,widget->icon(),widget->title());
}

void MainWindow::addPlugin(NCSConnectionWidgetPlugin *widget)
{
    connect(widget,SIGNAL(bridgeEstablished(NCSCommandBridge*)),this,SLOT(m_setCommandBridge(NCSCommandBridge*)));

    for (int i = 0; i < m_applicationPlugins.count(); i ++)
        connect(widget,SIGNAL(bridgeEstablished(NCSCommandBridge*)),m_applicationPlugins[i],SLOT(setCommandBridge(NCSCommandBridge*)));

    m_connectionPlugins.append(widget);
    addPlugin((NCSWidgetPlugin *)widget);
}

void MainWindow::addPlugin(NCSApplicationWidgetPlugin *widget)
{
    for (int i = 0; i < m_connectionPlugins.count(); i ++)
        connect(m_connectionPlugins[i],SIGNAL(bridgeEstablished(NCSCommandBridge*)),widget,SLOT(setCommandBridge(NCSCommandBridge*)));
    m_applicationPlugins.append(widget);
    addPlugin((NCSWidgetPlugin *)widget);
}
void MainWindow::addPlugin(NCSDistributionWidgetPlugin *widget)
{
    connect(widget,SIGNAL(launchTriggered()),this,SLOT(m_showLoadingSimulation()));
    connect(widget,SIGNAL(launchFailed()),this,SLOT(m_hideLoadingSimulation()));
    connect(widget,SIGNAL(distributed(QString)),this,SLOT(m_createNetwork(QString)));
    connect(widget,SIGNAL(launched()),this,SLOT(m_publishNetwork()));

    m_distributionPlugins.append(widget);
    addPlugin((NCSApplicationWidgetPlugin *)widget);
}
void MainWindow::addPlugin(NCSSubscriberWidgetPlugin *widget)
{
    m_subscriberPlugins.append(widget);
    addPlugin((NCSWidgetPlugin *)widget);
}

void MainWindow::closeEvent(QCloseEvent * event)
{

    QMessageBox msgBox;
    msgBox.setText("Are you sure that you want to exit NCV? "  );
    QPushButton * exitButton =  msgBox.addButton("Exit", QMessageBox::ActionRole);
    msgBox.addButton("Cancel", QMessageBox::ActionRole);
    msgBox.exec();

    if (msgBox.clickedButton() == exitButton)
    {
        m_closeProject();
        m_reportingManager->disconnectFromHost();
    }
    else
        event->ignore();
}
void MainWindow::m_runSimulationPressed()
{
    disconnect(m_runSimulationButton,SIGNAL(triggered()),this,SLOT(m_runSimulationPressed()));
    connect(m_runSimulationButton,SIGNAL(triggered()),this,SLOT(m_pauseSimulationPressed()));
    connect(m_simulationTimeSlider,SIGNAL(sliderMoved(int)),m_reportingManager,SLOT(setUpdateInterval(int)));
    m_runSimulationButton->setIcon(QIcon(":/media/pauseIcon.png"));
    m_runSimulationButton->setText("Pause Simulation");
    m_reportingManager->setUpdateInterval(m_simulationTimeSlider->value());
    m_reportingManager->startUpdates();
    m_stopSimulationButton->setEnabled(true);
}

void MainWindow::m_pauseSimulationPressed()
{
    disconnect(m_runSimulationButton,SIGNAL(triggered()),this,SLOT(m_pauseSimulationPressed()));
    connect(m_runSimulationButton,SIGNAL(triggered()),this,SLOT(m_runSimulationPressed()));
    disconnect(m_simulationTimeSlider,SIGNAL(sliderMoved(int)),m_reportingManager,SLOT(setUpdateInterval(int)));
    m_runSimulationButton->setIcon(QIcon(":/media/playIcon.png"));
    m_runSimulationButton->setText("Run Simulation");
    m_reportingManager->stopUpdates();
    m_stopSimulationButton->setEnabled(true);
}

void  MainWindow::m_stopSimulationPressed()
{
    m_pauseSimulationPressed();

    QMessageBox msgBox;
    msgBox.setText("Stop the simulation?" );
    msgBox.addButton("Continue", QMessageBox::ActionRole);
    QPushButton * cancelButton= msgBox.addButton("Cancel", QMessageBox::ActionRole);
    msgBox.exec();
    if (msgBox.clickedButton() == cancelButton)
        return;


    m_disconnectFromSimulator();

    for (int i = 0; i < m_distributionPlugins.count(); i ++)
        m_applicationLauncher->setPageEnabled(m_applicationLauncher->indexOf(m_distributionPlugins[i]),true);


    for (int i = 0; i < m_subscriberPlugins.count(); i ++)
    {
        int launcherIndex = m_applicationLauncher->indexOf(m_subscriberPlugins[i]);
        m_applicationLauncher->setPageEnabled(launcherIndex,false);

        if (m_applicationLauncher->currentIndex() == launcherIndex)
            m_applicationLauncher->setCurrentIndex(m_applicationLauncher->indexOf(m_distributionPlugins[0]));
    }


}

void MainWindow::m_disconnectFromSimulator(bool destroy )
{
    m_timeScaleLabel->setText(" ( <b>" + QString::number(m_simulationTimeSlider->value()) +"</b> msec ) ");

    for (int i = 0; i < m_subscriberPlugins.count(); i ++)
    {
        m_subscriberPlugins[i]->setNeurons(NULL);
        if (m_connections != NULL)
            m_subscriberPlugins[i]->setConnections(NULL);
    }

    m_reportingManager->setNeurons(NULL);
    m_reportingManager->setConnections(NULL);

    m_activeApplications[m_simulationApplicationIndex]->stopExecution(destroy);

    m_simulationApplicationIndex = -1;
    m_setSimulationToolbar(false);

    delete m_neurons;
    delete m_connections;
    m_neurons = NULL;
    m_connections = NULL;
}

void MainWindow::m_openProjectPressed()
{

    QString projDir = QFileDialog::getExistingDirectory(this,"Open Project File",m_rootPath);
    if (projDir != "")
    {
        if (m_projectDirectory != "")
            m_closeProject();
        m_loadProject(projDir);
    }
}

void MainWindow::m_newProjectPressed()
{
    QString projName = QInputDialog::getText(this,"Create New Project","What would you like to name this project?");
    if (projName != "")
    {
        if (m_projectDirectory != "")
            m_closeProject();

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

    m_projectDirectory = projectDirectory;
    m_applicationLauncher->setEnabled(true);

     m_applicationLauncher->setCurrentIndex(m_applicationLauncher->indexOf(m_connectionPlugins[0]));

    for (int i = 0; i < m_applicationPlugins.count(); i ++)
        m_applicationLauncher->setPageEnabled(m_applicationLauncher->indexOf(m_applicationPlugins[i]),false);

    for (int i = 0; i < m_subscriberPlugins.count(); i ++)
        m_applicationLauncher->setPageEnabled(m_applicationLauncher->indexOf(m_subscriberPlugins[i]),false);

    for (int i = 0; i < m_allPlugins.count(); i ++)
        m_allPlugins[i]->loadProject(m_projectDirectory);


}
void MainWindow::m_closeProject()
{

    for (int i = 0; i < m_activeApplications.count(); i ++)
    {

        // if this application has already been untracked, skip.
        if (m_activeApplications[i] == NULL)
            continue;

        QMessageBox msgBox;
        msgBox.setText("The NCS application '" + m_activeApplications[i]->applicationName() + "' is still running.\n Should this application be closed or left running?"  );
        msgBox.addButton("Leave Running", QMessageBox::ActionRole);
        QPushButton * closeButton =  msgBox.addButton("Close", QMessageBox::ActionRole);
        msgBox.exec();

        // if this application finished while the prompt was displayed, skip.
        if (m_activeApplications[i] == NULL)
            continue;

        disconnect(m_activeApplications[i],SIGNAL(executionFinished()), m_applicationMapper,SLOT(map()));
        if (i == m_simulationApplicationIndex)
            m_disconnectFromSimulator(msgBox.clickedButton() == closeButton);
        else
            m_activeApplications[i]->stopExecution(msgBox.clickedButton() == closeButton);
    }
    m_activeApplications.clear();

    if (QDir(m_projectDirectory + "/tmp").exists())
        removeDir(m_projectDirectory+"/tmp");

    m_projectDirectory = "";
    m_ncsInstallationLabel->setText("<b> [ Unconnected ] </b>");
}

void MainWindow::m_showLoadingSimulation()
{
    m_simulationLoadingLabel->setMovie(m_simulationLoadingMovie);
    m_simulationLoadingMovie->start();
}

void MainWindow::m_hideLoadingSimulation()
{
    m_simulationLoadingMovie->stop();
    m_simulationLoadingLabel->clear();
}

void MainWindow::m_updateTimeScale(int value)
{
    int val = pow(value,0.2f);
    m_reportingManager->setUpdateInterval(value);
    m_timeScaleLabel->setText(" <font color= '#772953' >( <b>" + QString::number(value) +"</b> msec )</font> ");

}

void MainWindow::m_setCommandBridge(NCSCommandBridge * bridge)
{
    m_commandBridge = bridge;
    connect(m_commandBridge,SIGNAL(applicationStarted(NCSApplicationBridge*)),this,SLOT(m_ncsApplicationLaunched(NCSApplicationBridge*)));
    for (int i = 0; i < m_applicationPlugins.count(); i ++)
        m_applicationLauncher->setPageEnabled(m_applicationLauncher->indexOf(m_applicationPlugins[i]),true);
    m_ncsInstallationLabel->setText("<font color= '#dd4814' > [ <i>NCS6</i>@<b>" + m_commandBridge->hostname()+ "</b> ] </font> ");
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

    m_activeApplications[index] = NULL;
    if (index == m_simulationApplicationIndex)
        m_simulationApplicationIndex = -1;

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

    for (int i = 0; i < m_distributionPlugins.count(); i ++)
    {
        int launcherIndex = m_applicationLauncher->indexOf(m_distributionPlugins[i]);
        m_applicationLauncher->setPageEnabled(launcherIndex,false);

        if (m_applicationLauncher->currentIndex() == launcherIndex)
            m_applicationLauncher->setCurrentIndex(m_applicationLauncher->indexOf(m_subscriberPlugins[0]));
    }


    qDebug() << m_commandBridge->hostname();
    qDebug() << "NCSDataSource connected:" << m_reportingManager->connectToHost(m_commandBridge->hostname().toStdString(),8951);
    m_reportingManager->setNeurons(m_neurons);
    if (m_connections != NULL)
        m_reportingManager->setConnections(m_connections);

    for (int i = 0; i < m_subscriberPlugins.count(); i ++)
    {
        m_applicationLauncher->setPageEnabled(m_applicationLauncher->indexOf(m_subscriberPlugins[i]),true);
        m_subscriberPlugins[i]->setNeurons(m_neurons);
        if (m_connections != NULL)
            m_subscriberPlugins[i]->setConnections(m_connections);
    }

    m_hideLoadingSimulation();
    m_setSimulationToolbar(true);
}

void MainWindow::m_setSimulationToolbar(bool on)
{
    m_runSimulationButton->setEnabled(on);
    m_simulationTimeSlider->setEnabled(on);
    m_stopSimulationButton->setEnabled(on);
    m_timeScaleLabel->setEnabled(on);
    if (on)
        m_updateTimeScale(m_simulationTimeSlider->value());
    m_simulationLoadingLabel->setEnabled(false);
}

MainWindow::~MainWindow()
{
}
