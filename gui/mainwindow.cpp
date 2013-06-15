#include "mainwindow.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QDomDocument>
#include "gui/ncsinstallationdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    m_neurons = NULL;
    m_connections = NULL;
    m_project = NULL;

    this->setWindowTitle("NCV");
    m_rootPath = QDir::homePath() + "/NCV_Projects";
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
    QAction * exitApplicationAction = m_fileMenu->addAction("Exit");
    connect(exitApplicationAction,SIGNAL(triggered()),this,SLOT(close()));
    m_menuBar->addMenu(m_fileMenu);

    m_installationDialog = new NCSInstallationDialog(this);
    m_editMenu = new QMenu(tr("Edit"),this);
    QAction * ncsInstall  = m_editMenu->addAction("Change NCS Installation");
    connect(ncsInstall,SIGNAL(triggered()),m_installationDialog,SLOT(show()));
    m_menuBar->addMenu(m_editMenu);

    setMenuBar(m_menuBar);

    m_reportingManager = new NetworkUpdateManager(this);


    m_simulationToolbar = new QToolBar("Simulation");

    m_simulationLoadingLabel = new QLabel();
    m_simulationLoadingMovie = new QMovie(":/resources/images/loading.gif");
    m_simulationToolbar->addWidget(m_simulationLoadingLabel);

    m_runSimulationButton = new QToolButton();
    m_runSimulationButton->setIcon(QIcon(":/resources/images/playIcon.png"));
    m_runSimulationButton->setText("Run Simulation");
    connect(m_runSimulationButton,SIGNAL(pressed()),this,SLOT(m_runSimulationPressed()));
    m_simulationToolbar->addWidget(m_runSimulationButton);

    m_stopSimulationButton = new QToolButton();
    m_stopSimulationButton->setIcon(QIcon(":/resources/images/stopIcon.png"));
    m_stopSimulationButton->setText("Stop Simulation");
    connect(m_stopSimulationButton,SIGNAL(pressed()),this,SLOT(m_stopSimulationPressed()));
    m_simulationToolbar->addWidget(m_stopSimulationButton);

    m_simulationTimeSlider = new QSlider();
    m_simulationTimeSlider->setToolTip("Adjust the  Update Interval");
    m_simulationTimeSlider->setOrientation(Qt::Horizontal);
    m_simulationTimeSlider->setMinimum(1);
    m_simulationTimeSlider->setValue(100);
    m_simulationTimeSlider->setMaximum(200);
    m_simulationTimeSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_simulationToolbar->addWidget(m_simulationTimeSlider);

    connect(m_simulationTimeSlider,SIGNAL(sliderMoved(int)),this,SLOT(m_updateTimeScale(int)));
    m_timeScaleLabel = new QLabel();
    m_timeScaleLabel->setTextFormat(Qt::RichText);
    m_timeScaleLabel->setText("[ <b>" + QString::number(m_simulationTimeSlider->value()) +"</b> msec ] ");
    m_timeScaleLabel->setEnabled(false);
    m_simulationToolbar->addWidget(m_timeScaleLabel);
    m_simulationToolbar->setMovable(false);

    m_setSimulationToolbar(false);

    m_applicationToolbar = this->addToolBar("Application" );
    QWidget * spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_applicationToolbar->addWidget(spacer);

    m_contextIconButton = new QToolButton();
    m_contextIconButton->setAutoRaise(true);
    m_contextIconButton->setIcon(QIcon(":/resources/images/ncs.png"));

    m_applicationToolbar->addWidget(m_contextIconButton);
    m_ncsContextLabel = new QLabel();
    m_ncsContextLabel->setText("[<b> Unconnected </b>]");
    m_ncsContextLabel->setTextFormat(Qt::RichText);
    m_applicationToolbar->addWidget(m_ncsContextLabel);

    m_projectIconButton = new QToolButton();
    m_projectIconButton->setAutoRaise(true);
    m_projectIconButton->setIcon(QIcon(":/resources/images/projectIcon.png"));

    m_applicationToolbar->addWidget(m_projectIconButton);
    m_projectLabel = new QLabel();
    m_projectLabel->setText("[<b> No Project </b>]");
    m_projectLabel->setTextFormat(Qt::RichText);
    m_applicationToolbar->addWidget(m_projectLabel);

    m_applicationToolbar->setMovable(false);

    m_applicationMapper = new QSignalMapper(this);
    connect(m_applicationMapper,SIGNAL(mapped(QObject*)),this,SLOT(m_ncsApplicationFinished(QObject*)));
    m_applicationLauncher = NULL;
    m_commandBridge = NULL;
    m_simulationApplicationIndex = -1;

    m_applicationLauncher = new QxtConfigWidget(this);
    m_applicationLauncher->hide();
    m_applicationLauncher->setIconSize(QSize(64,64));

    PythonEditor * pythonEditor = new PythonEditor(m_applicationLauncher);
    addPlugin(pythonEditor);

    NCSClusterEditor * clusterEditor = new NCSClusterEditor(m_applicationLauncher);
    addPlugin(clusterEditor);

    LIFModelDistributionWidget * modelWidget = new LIFModelDistributionWidget(m_applicationLauncher);
    addPlugin(modelWidget);

    IzhModelDistributionWidget * izhModelWidget= new IzhModelDistributionWidget(m_applicationLauncher);
    addPlugin(izhModelWidget);

    NCVWidget * visualizationWidget = new NCVWidget(m_applicationLauncher);
    addPlugin(visualizationWidget);

    for (int i = 0; i < m_applicationPlugins.count(); i ++)
        m_setPluginEnabled(m_applicationPlugins[i],false);

    for (int i = 0; i < m_subscriberPlugins.count(); i ++)
        m_setPluginEnabled(m_subscriberPlugins[i],false);

    m_welcomeWidget = new NCSWelcomeWidget();
    connect(m_welcomeWidget,SIGNAL(quickPanelTriggered(int)),this,SLOT(m_welcomeWidgetPanelSelected(int)));
    QString connectionDesc = "Integrate with the powerful NCS framework to enable simulation features.";
    m_welcomeWidget->addQuickPanel(QPixmap(":/resources/images/ncs.png"), "Connect to NCS",connectionDesc);
    QString projectDesc = "Create a new project and organize your resources into a single location.";
    m_welcomeWidget->addQuickPanel(QPixmap(":/resources/images/projectIcon.png"), "Start a Project",projectDesc);
    m_welcomeWidget->setPanelEnabled(1,false);
    QString developDesc = "Open an existing project and start simulating. ";
    m_welcomeWidget->addQuickPanel(QPixmap(":/resources/images/setupIcon.png"), "Open a Project",developDesc);
    m_welcomeWidget->setPanelEnabled(2,false);

    this->setCentralWidget(m_welcomeWidget);
    this->showMaximized();

    connect(m_installationDialog,SIGNAL(attemptSuccesful(NCSCommandBridge*)),this,SLOT(m_setCommandBridge(NCSCommandBridge*)));
    if (m_installationDialog->hasDefaultInstallation())
    {
        connect(m_installationDialog,SIGNAL(attemptFailed(NCSInstallationDialog::BridgeCreationError)),this,SLOT(m_defaultNCSInstallationFailed(NCSInstallationDialog::BridgeCreationError)));
        m_installationDialog->attemptDefault();
    }



}


MainWindow::~MainWindow()
{
}

void MainWindow::m_welcomeWidgetPanelSelected(int index)
{
    if (index == 0)
        m_installationDialog->show();
    else if (index == 1)
        m_newProjectPressed();
    else if (index == 2)
        m_openProjectPressed();
}

void MainWindow::addPlugin(NCSWidgetPlugin *widget)
{
    m_allPlugins.append(widget);
    m_applicationLauncher->addPage(widget,widget->icon(),widget->title());
}

void MainWindow::addPlugin(NCSApplicationWidgetPlugin *widget)
{
    if (m_commandBridge != NULL)
        widget->setCommandBridge(m_commandBridge);
    m_applicationPlugins.append(widget);
    addPlugin((NCSWidgetPlugin *)widget);

    if (m_commandBridge == NULL)
        m_setPluginEnabled(widget,false);
}
void MainWindow::addPlugin(NCSDistributionWidgetPlugin *widget)
{
    connect(widget,SIGNAL(launchTriggered()),this,SLOT(m_showLoadingSimulation()));
    connect(widget,SIGNAL(launchFailed()),this,SLOT(m_hideLoadingSimulation()));
    connect(widget,SIGNAL(distributed(QString)),this,SLOT(m_createNetwork(QString)));
    connect(widget,SIGNAL(launched(QString)),this,SLOT(m_publishNetwork(QString)));

    m_distributionPlugins.append(widget);
    addPlugin((NCSApplicationWidgetPlugin *)widget);
}
void MainWindow::addPlugin(NCSSubscriberWidgetPlugin *widget)
{
    m_subscriberPlugins.append(widget);
    addPlugin((NCSWidgetPlugin *)widget);

    if (m_simulationApplicationIndex == -1)
        m_setPluginEnabled(widget,false);
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
        if (m_project != NULL)
            m_closeProject();
        m_reportingManager->disconnectFromHost();
    }
    else
        event->ignore();
}

void MainWindow::m_runSimulationPressed()
{
    disconnect(m_runSimulationButton,SIGNAL(pressed()),this,SLOT(m_runSimulationPressed()));
    connect(m_runSimulationButton,SIGNAL(pressed()),this,SLOT(m_pauseSimulationPressed()));
    connect(m_simulationTimeSlider,SIGNAL(sliderMoved(int)),m_reportingManager,SLOT(setUpdateInterval(int)));
    m_runSimulationButton->setIcon(QIcon(":/resources/images/pauseIcon.png"));
    m_runSimulationButton->setText("Pause Simulation");
    m_reportingManager->setUpdateInterval(m_simulationTimeSlider->value());
    m_reportingManager->startUpdates();
    m_stopSimulationButton->setEnabled(true);
}

void MainWindow::m_pauseSimulationPressed()
{
    disconnect(m_runSimulationButton,SIGNAL(pressed()),this,SLOT(m_pauseSimulationPressed()));
    connect(m_runSimulationButton,SIGNAL(pressed()),this,SLOT(m_runSimulationPressed()));
    disconnect(m_simulationTimeSlider,SIGNAL(sliderMoved(int)),m_reportingManager,SLOT(setUpdateInterval(int)));
    m_runSimulationButton->setIcon(QIcon(":/resources/images/playIcon.png"));
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
        m_setPluginEnabled(m_distributionPlugins[i],true);
    for (int i = 0; i < m_subscriberPlugins.count(); i ++)
    {
        m_setPluginEnabled(m_subscriberPlugins[i],false);
        int launcherIndex = m_applicationLauncher->indexOf(m_subscriberPlugins[i]);
   }


}

void MainWindow::m_disconnectFromSimulator(bool destroy )
{
    m_timeScaleLabel->setText("[ <b>" + QString::number(m_simulationTimeSlider->value()) +"</b> msec ]");

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
    QString filepath = QFileDialog::getOpenFileName(this,"Open Project File",m_rootPath,tr("NCS Project file (*.ncsproj)"));

    if (filepath != "")
    {
        if (m_project != NULL)
            m_closeProject();
        m_loadProject(filepath);
    }
}

void MainWindow::m_newProjectPressed()
{
    QString projName = QInputDialog::getText(this,"Create New Project","What would you like to name this project?");
    if (projName != "")
    {
        if (m_project != NULL)
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
        m_loadProject(m_rootPath + "/" + projName + "/" + projName + ".ncsproj");
    }
}

void MainWindow::m_loadProject(QString filepath)
{

    if (this->centralWidget() == m_welcomeWidget)
    {
        this->insertToolBar(m_applicationToolbar,m_simulationToolbar);
        this->setCentralWidget(m_applicationLauncher);
        m_welcomeWidget->hide();
        m_applicationLauncher->show();
    }

    m_project = new NCSProject(filepath,this);
    m_projectLabel->setText("[<font color= '#dd4814' > " +m_project->name() + " </font>]");
    if (QDir(m_project->parentDirectory() + "/tmp").exists())
        m_removeDir(m_project->parentDirectory() +"/tmp");

    QDir(m_project->parentDirectory()).mkdir("tmp");

    for (int i = 0; i < m_allPlugins.count(); i ++)
        m_project->registerPlugin(m_allPlugins[i]);

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

    if (QDir(m_project->parentDirectory() + "/tmp").exists())
        m_removeDir(m_project->parentDirectory()+"/tmp");

    m_project->save();
    delete m_project;
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
    m_reportingManager->setUpdateInterval(value);
    m_timeScaleLabel->setText("[ <font color= '#772953' > <b>" + QString::number(value) +"</b> msec </font>]");

}


void MainWindow::m_defaultNCSInstallationFailed(NCSInstallationDialog::BridgeCreationError)
{
    QMessageBox msgBox;
    msgBox.setText("NCV could not validate the default NCS host. Fill out the following dialog to connect to NCS."  );
    msgBox.addButton("Ok", QMessageBox::ActionRole);
    msgBox.exec();

    disconnect(m_installationDialog,SIGNAL(attemptFailed(NCSInstallationDialog::BridgeCreationError)),this,SLOT(m_defaultNCSInstallationFailed(NCSInstallationDialog::BridgeCreationError)));

    m_installationDialog->show();
}

void MainWindow::m_setCommandBridge(NCSCommandBridge * bridge)
{
    if (this->centralWidget() == m_welcomeWidget)
    {
        m_welcomeWidget->setPanelEnabled(1,true);
        m_welcomeWidget->setPanelEnabled(2,true);
    }
    m_commandBridge = bridge;
    connect(m_commandBridge,SIGNAL(applicationBridgeLaunched(NCSApplicationBridge*)),this,SLOT(m_ncsApplicationLaunched(NCSApplicationBridge*)));
    for (int i = 0; i < m_applicationPlugins.count(); i ++)
    {
        m_applicationPlugins[i]->setCommandBridge(bridge);
        m_setPluginEnabled(m_applicationPlugins[i],true);
    }
    m_ncsContextLabel->setText("[<font color= '#dd4814' > <i>NCS6</i>@<b>" + m_commandBridge->hostname()+ "</b> </font>]");
}

void MainWindow::m_setPluginEnabled(NCSWidgetPlugin * plugin, bool enable)
{
    int index = m_applicationLauncher->indexOf(plugin);
    m_applicationLauncher->setPageEnabled(index,enable);
    QString title = m_applicationLauncher->pageTitle(index);
    if (enable)
        m_applicationLauncher->setPageToolTip(index,title);
    else
    {
        m_applicationLauncher->setPageToolTip(index,title + "<br><font color='#dd4814'>Start a simulation to activate</font>");

        if (m_applicationLauncher->currentIndex() == index)
        {
            if (plugin->inherits(NCSDistributionWidgetPlugin::staticMetaObject.className()))
                m_applicationLauncher->setCurrentIndex(m_applicationLauncher->indexOf(m_subscriberPlugins[0]));

            if (plugin->inherits(NCSSubscriberWidgetPlugin::staticMetaObject.className()))
                m_applicationLauncher->setCurrentIndex(m_applicationLauncher->indexOf(m_distributionPlugins[0]));

        }
    }
}

void MainWindow::m_ncsApplicationLaunched(NCSApplicationBridge * app)
{

    m_activeApplications.append(app);
    qDebug() << "launched " <<app->applicationName();
    if (app->applicationName() == "simulator")
        m_simulationApplicationIndex = m_activeApplications.count() -1;

    m_applicationMapper->setMapping(app,app);
    connect(app,SIGNAL(executionFinished()), m_applicationMapper,SLOT(map()));
    connect(app,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)), m_applicationMapper,SLOT(map()));
}

void MainWindow::m_ncsApplicationFinished(QObject * app)
{
    NCSApplicationBridge * application = dynamic_cast<NCSApplicationBridge *> (app);
    qDebug() << "closed" << application->applicationName();
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

    unsigned int numNeurons;
    dataStream >> numNeurons;
    qDebug() << "Neurons Created:" <<numNeurons;
    for (unsigned int i = 0; i <numNeurons; i ++)
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

    unsigned int numConnections;
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
    for (unsigned int i = 0; i < numConnections; i++)
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

void MainWindow::m_publishNetwork(QString reportHost)
{


    if (m_reportingManager->connectToHost(reportHost.toStdString(),8951))
        qDebug() << "NCV connected to NCS simulation reporting from " << reportHost;
    else
    {
        m_hideLoadingSimulation();
        QMessageBox msgBox;
        msgBox.setText("NCV could not connect to the report host of NCS. This may be due to another simulator instance running on the host or because of problems with the internet connection."  );
        msgBox.addButton("Ok", QMessageBox::ActionRole);
        msgBox.exec();
        m_disconnectFromSimulator();
        return;
    }

    for (int i = 0; i < m_distributionPlugins.count(); i ++)
        m_setPluginEnabled(m_distributionPlugins[i],false);

    m_reportingManager->setNeurons(m_neurons);
    if (m_connections != NULL)
        m_reportingManager->setConnections(m_connections);

    for (int i = 0; i < m_subscriberPlugins.count(); i ++)
    {
        m_subscriberPlugins[i]->setNeurons(m_neurons);
        m_subscriberPlugins[i]->setConnections(m_connections);
        m_setPluginEnabled(m_subscriberPlugins[i],true);
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

bool MainWindow::m_removeDir(const QString & dirName)
{
    bool result;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = m_removeDir(info.absoluteFilePath());
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
