#include "izhmodeldistributionwidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include "core/ncscluster.h"

IzhModelDistributionWidget::IzhModelDistributionWidget(QWidget *parent) :
    NCSDistributionWidgetPlugin(parent)
{

    m_commandBridge = NULL;
    m_currentApplication = NULL;
    m_launching = false;
    m_distributionOutputDir = "izhout";

    m_layout = new QVBoxLayout();

    m_neuronFileVector = new QWidgetVector();
    m_neuronFileLabel = new QLabel("Neuron File:");
    m_neuronFileVector->addWidget(m_neuronFileLabel);
    m_neuronFileEdit = new QLineEdit();
    m_neuronFileVector->addWidget(m_neuronFileEdit);
    m_brosweNeuronFileButton = new QPushButton("Browse");
    connect(m_brosweNeuronFileButton,SIGNAL(clicked()),this,SLOT(m_browseNeuronFilePressed()));
    m_neuronFileVector->addWidget(m_brosweNeuronFileButton);
    m_layout->addWidget(m_neuronFileVector);


    m_synapseFileVector = new QWidgetVector();
    m_synapseFileLabel = new QLabel("Synapse File:");
    m_synapseFileVector->addWidget(m_synapseFileLabel);
    m_synapseFileEdit = new QLineEdit();
    m_synapseFileVector->addWidget(m_synapseFileEdit);
    m_brosweSynapseFileButton = new QPushButton("Browse");
    connect(m_brosweSynapseFileButton,SIGNAL(clicked()),this,SLOT(m_browseSynapseFilePressed()));
    m_synapseFileVector->addWidget(m_brosweSynapseFileButton);
    m_layout->addWidget(m_synapseFileVector);


    m_currentFileVector = new QWidgetVector();
    m_currentFileLabel = new QLabel("Current File:");
    m_currentFileVector->addWidget(m_currentFileLabel);
    m_currentFileEdit = new QLineEdit();
    m_currentFileVector->addWidget(m_currentFileEdit);
    m_brosweCurrentFileButton = new QPushButton("Browse");
    connect(m_brosweCurrentFileButton,SIGNAL(clicked()),this,SLOT(m_browseCurrentFilePressed()));
    m_currentFileVector->addWidget(m_brosweCurrentFileButton);
    m_layout->addWidget(m_currentFileVector);


    m_clusterFileVector = new QWidgetVector();
    m_clusterFileLabel = new QLabel("Cluster File:");
    m_clusterFileVector->addWidget(m_clusterFileLabel);
    m_clusterFileEdit = new QLineEdit();
    m_clusterFileVector->addWidget(m_clusterFileEdit);
    m_brosweClusterFileButton = new QPushButton("Browse");
    connect(m_brosweClusterFileButton,SIGNAL(clicked()),this,SLOT(m_browseClusterFilePressed()));
    m_clusterFileVector->addWidget(m_brosweClusterFileButton);
    m_layout->addWidget(m_clusterFileVector);


    m_neuronCountVector = new QWidgetVector();
    m_neuronCountLabel = new QLabel("Neuron Count: ");
    m_neuronCountVector->addWidget(m_neuronCountLabel);
    m_neuronCountSpinBox = new QSpinBox();
    m_neuronCountSpinBox->setMinimum(1);
    m_neuronCountSpinBox->setMaximum(1000000);
    m_neuronCountVector->addWidget(m_neuronCountSpinBox);
    m_layout->addWidget(m_neuronCountVector);

    m_timeWidgetVector = new QWidgetVector();
    m_timeLabel = new QLabel("Simulation Runtime: ");
    m_timeWidgetVector->addWidget(m_timeLabel);
    m_timeSpinBox = new QSpinBox();
    m_timeSpinBox->setMinimum(1);
    m_timeSpinBox->setMaximum(10000);
    m_timeWidgetVector->addWidget(m_timeSpinBox);
    m_timeUnitsComboBox = new QComboBox();
    m_timeUnitsComboBox->addItem("Milliseconds");
    m_timeUnitsComboBox->addItem("Seconds");
    m_timeUnitsComboBox->addItem("Minutes");
    m_timeUnitsComboBox->addItem("Hours");
    m_timeUnitsComboBox->addItem("Days");
    m_timeUnitsComboBox->addItem("Forever");
    connect(m_timeUnitsComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(m_timeUnitSelected(QString)));
    m_timeWidgetVector->addWidget(m_timeUnitsComboBox);
    m_layout->addWidget(m_timeWidgetVector);

    m_launchSimulationButton = new QPushButton("Launch Simulation");
    connect(m_launchSimulationButton,SIGNAL(clicked()),this,SLOT(m_launchSimulationPressed()));
    m_layout->addWidget(m_launchSimulationButton);

    this->setLayout(m_layout);
    this->setEnabled(false);

}

void IzhModelDistributionWidget::loadProject(NCSProjectPortal project)
{
    m_projectDir = project.parentDirectory();
    m_topologyFilename = m_projectDir + "/tmp/topology";
    m_neuronFileEdit->clear();
    m_synapseFileEdit->clear();
    m_currentFileEdit->clear();
    m_clusterFileEdit->clear();
    m_timeSpinBox->setValue(10);
    m_timeUnitsComboBox->setCurrentIndex(3);
}

QIcon IzhModelDistributionWidget::icon()
{
    return QIcon(":/resources/images/izhDistributor.png");
}

QString IzhModelDistributionWidget::title()
{
    return "Izhikevich Distributor";
}


QString IzhModelDistributionWidget::name()
{
    return "izh-launcher";
}

float IzhModelDistributionWidget::version()
{
    return 1.0;
}

void IzhModelDistributionWidget::setCommandBridge(NCSCommandBridge * bridge)
{
    this->setEnabled(true);
    m_commandBridge = bridge;
}

void IzhModelDistributionWidget::initialize()
{
}

void IzhModelDistributionWidget::cleanup()
{
}

void IzhModelDistributionWidget::m_distributionStarted(NCSApplicationBridge * app)
{
    m_currentApplication = app;
    disconnect(m_commandBridge,SIGNAL(applicationBridgeLaunched(NCSApplicationBridge*)),this,SLOT(m_distributionStarted(NCSApplicationBridge*)));
    connect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_distributionFailed(NCSApplicationBridge::ApplicationError)));
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_distributionFinished()));
}

void IzhModelDistributionWidget::m_destroyDistribution()
{
    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_distributionFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_distributionFinished()));
    delete m_currentApplication;
    m_currentApplication = NULL;
}

void IzhModelDistributionWidget::m_simulationStarted(NCSApplicationBridge * app)
{
    m_currentApplication = app;
    disconnect(m_commandBridge,SIGNAL(applicationBridgeLaunched(NCSApplicationBridge*)),this,SLOT(m_simulationStarted(NCSApplicationBridge*)));
    connect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_simulationFailed(NCSApplicationBridge::ApplicationError)));
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_simulationFinished()));
    connect(m_currentApplication,SIGNAL(readyReadStandardOutput()),this,SLOT(m_readStandardOutput()));
    connect(m_currentApplication,SIGNAL(readyReadStandardError()),this,SLOT(m_readStandardError()));
}
void IzhModelDistributionWidget::m_destroySimulation()
{
    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_simulationFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_simulationFinished()));
    disconnect(m_currentApplication,SIGNAL(readyReadStandardOutput()),this,SLOT(m_readStandardOutput()));
    disconnect(m_currentApplication,SIGNAL(readyReadStandardError()),this,SLOT(m_readStandardError()));
    delete m_currentApplication;
    m_currentApplication = NULL;
}


void IzhModelDistributionWidget::m_launchSimulationPressed()
{
    if (!m_fieldsValid())
        return;

    m_launching = true;

    QString neuronFile = m_getFilename(m_neuronFileEdit->text());
    QString synapseFile = m_getFilename(m_synapseFileEdit->text());
    QString currentFile = m_getFilename(m_currentFileEdit->text());
    QString clusterFile = m_getFilename(m_clusterFileEdit->text());


    NCSCommandArguments distArgs;
    distArgs << NCSCommandFileArgument(neuronFile,m_neuronFileEdit->text(),NCSCommandFileArgument::UploadBeforeExecution);
    distArgs << NCSCommandFileArgument(synapseFile,m_synapseFileEdit->text(),NCSCommandFileArgument::UploadBeforeExecution);
    distArgs << NCSCommandFileArgument(currentFile,m_currentFileEdit->text(),NCSCommandFileArgument::UploadBeforeExecution);
    distArgs << QString::number(m_neuronCountSpinBox->value());
    distArgs << NCSCommandFileArgument(clusterFile,m_clusterFileEdit->text(),NCSCommandFileArgument::UploadBeforeExecution);
    distArgs << m_distributionOutputDir << "-topology" << NCSCommandFileArgument("topology",m_topologyFilename,NCSCommandFileArgument::DownloadAfterExecution);
    connect(m_commandBridge,SIGNAL(applicationBridgeLaunched(NCSApplicationBridge*)),this,SLOT(m_distributionStarted(NCSApplicationBridge*)));
    m_commandBridge->launchApplicationBridge("izhDistributor",distArgs);

}

bool IzhModelDistributionWidget::m_fieldsValid()
{
    QMessageBox msgBox;
    msgBox.addButton("Ok", QMessageBox::ActionRole);
    if (!QFile(m_neuronFileEdit->text()).exists())
    {
        msgBox.setText("The neuron model file entered could not be found." );
        msgBox.exec();
        return false;
    }
    if (!QFile(m_synapseFileEdit->text()).exists())
    {
        msgBox.setText("The synapse model file entered could not be found." );
        msgBox.exec();
        return false;
    }
    if (!QFile(m_currentFileEdit->text()).exists())
    {
        msgBox.setText("The current file entered could not be found." );
        msgBox.exec();
        return false;
    }
    if (!QFile(m_clusterFileEdit->text()).exists())
    {
        msgBox.setText("The cluster file entered could not be found." );
        msgBox.exec();
        return false;
    }

    return true;

}

QString IzhModelDistributionWidget::m_getFilename(QString path)
{
#ifdef Win32
    QStringList pathSegments = path.split("\\");

#else
    QStringList pathSegments = path.split("/");
#endif
    return pathSegments.at(pathSegments.size()-1);
}

void IzhModelDistributionWidget::m_distributionFailed(NCSApplicationBridge::ApplicationError err)
{
    m_readStandardOutput();
    m_readStandardError();
    QMessageBox msgBox;
    msgBox.addButton("Ok", QMessageBox::ActionRole);
    msgBox.setText("Distribution Failed." );

    if (err == NCSApplicationBridge::Crashed)
        msgBox.setDetailedText("The distributor has crashed." );
    else if (err == NCSApplicationBridge::OtherApplicationRunning)
        msgBox.setDetailedText("Another instance of this distributor is running." );
    else if (err == NCSApplicationBridge::Timedout)
        msgBox.setDetailedText("Communication with the distributor has timed out. The host systems might not contain enough RAM for distribution." );
    else if (err == NCSApplicationBridge::UnknownError)
        msgBox.setDetailedText("There was an unknown error." );
    else if (err == NCSApplicationBridge::SyncDownloadError)
        msgBox.setDetailedText("There was a problem downloading files from the NCS installation." );
    else if (err == NCSApplicationBridge::SyncUploadError)
        msgBox.setDetailedText("There was a problem uploading files to the NCS installation." );
    else if (err == NCSApplicationBridge::WriteError)
        msgBox.setDetailedText("There was a problem writing data to the distributor." );
    else if (err == NCSApplicationBridge::ReadError)
        msgBox.setDetailedText("There was a problem reading data from the distributor." );

    msgBox.exec();
    m_launching = false;
    m_destroyDistribution();
}
void IzhModelDistributionWidget::m_distributionFinished()
{
    m_readStandardOutput();
    m_readStandardError();

    distributed(m_topologyFilename );
    m_destroyDistribution();

    QString timeArg;
    if (m_timeUnitsComboBox->currentText() == "Milliseconds")
        timeArg = QString::number((float)m_timeSpinBox->value() / 1000.0f);
    else if (m_timeUnitsComboBox->currentText() == "Seconds")
        timeArg = QString::number(m_timeSpinBox->value());
    else if (m_timeUnitsComboBox->currentText() == "Minutes")
        timeArg = QString::number(m_timeSpinBox->value() * 60);
    else if (m_timeUnitsComboBox->currentText() == "Hours")
        timeArg = QString::number(m_timeSpinBox->value() * 360);
    else if (m_timeUnitsComboBox->currentText() == "Days")
        timeArg = QString::number(m_timeSpinBox->value() * 360 * 24);
    else if (m_timeUnitsComboBox->currentText() == "Forever")
        timeArg = QString::number(-1);


    NCSCommandArguments simArgs;
    simArgs << m_distributionOutputDir << timeArg;

    NCSCluster cluster;
    cluster.read(m_clusterFileEdit->text());
    QString hostFilePath =  m_projectDir + "/tmp/hostfile";
    cluster.writeHostfile(hostFilePath);
    m_reportHost = cluster.reportHost();

    launchTriggered();
    connect(m_commandBridge,SIGNAL(applicationBridgeLaunched(NCSApplicationBridge*)),this,SLOT(m_simulationStarted(NCSApplicationBridge*)));
    m_commandBridge->launchApplicationBridge("simulator",simArgs,cluster.machines.count(),hostFilePath);

}

void IzhModelDistributionWidget::m_simulationFailed(NCSApplicationBridge::ApplicationError err)
{
    QMessageBox msgBox;
    msgBox.addButton("Ok", QMessageBox::ActionRole);
    msgBox.setText("Simulation Launching Failed." );

    if (err == NCSApplicationBridge::Crashed)
        msgBox.setDetailedText("The simulator has crashed." );
    else if (err == NCSApplicationBridge::OtherApplicationRunning)
        msgBox.setDetailedText("Another instance of this simulator is running." );
    else if (err == NCSApplicationBridge::Timedout)
        msgBox.setDetailedText("Communication with the simulator has timed out." );
    else if (err == NCSApplicationBridge::UnknownError)
        msgBox.setDetailedText("There was an unknown error." );
    else if (err == NCSApplicationBridge::SyncDownloadError)
        msgBox.setDetailedText("There was a problem downloading files from the NCS installation." );
    else if (err == NCSApplicationBridge::SyncUploadError)
        msgBox.setDetailedText("There was a problem uploading files to the NCS installation." );
    else if (err == NCSApplicationBridge::WriteError)
        msgBox.setDetailedText("There was a problem writing data to the simulator." );
    else if (err == NCSApplicationBridge::ReadError)
        msgBox.setDetailedText("There was a problem reading data from the simulator." );

    msgBox.exec();
    m_launching = false;
    launchFailed();
    m_destroySimulation();
}
void IzhModelDistributionWidget::m_simulationFinished()
{
    m_launching = false;
    m_destroySimulation();
}

void IzhModelDistributionWidget::m_readStandardError()
{
    qDebug() << m_currentApplication->readAllStandardError();
}
void IzhModelDistributionWidget::m_readStandardOutput()
{
    QString out = m_currentApplication->readAllStandardOutput();
    qDebug() << out;
    if (out.contains("Running simulation..."))
        launched(m_reportHost);
}

void IzhModelDistributionWidget::m_browseClusterFilePressed()
{
    QString file = QFileDialog::getOpenFileName(this,"Open Cluster File",m_projectDir);
    if (file != "")
        m_clusterFileEdit->setText(file);
}
void IzhModelDistributionWidget::m_browseNeuronFilePressed()
{
    QString file = QFileDialog::getOpenFileName(this,"Open Neuron File",m_projectDir,tr("NCS Model Input (*.in)"));
    if (file != "")
        m_neuronFileEdit->setText(file);
}
void IzhModelDistributionWidget::m_browseSynapseFilePressed()
{
    QString file = QFileDialog::getOpenFileName(this,"Open Synapse File",m_projectDir,tr("NCS Model Input (*.in)"));
    if (file != "")
        m_synapseFileEdit->setText(file);
}
void IzhModelDistributionWidget::m_browseCurrentFilePressed()
{
    QString file = QFileDialog::getOpenFileName(this,"Open Neuron File",m_projectDir,tr("Neuron Model Input (*.in)"));
    if (file != "")
        m_currentFileEdit->setText(file);
}
void IzhModelDistributionWidget::m_timeUnitSelected(QString unit)
{
    if (unit == "Forever")
        m_timeSpinBox->setEnabled(false);
    else
        m_timeSpinBox->setEnabled(true);
}
