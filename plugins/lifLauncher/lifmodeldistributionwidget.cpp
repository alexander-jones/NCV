#include "lifmodeldistributionwidget.h"
#include <QMessageBox>
#include <QFileDialog>

LIFModelDistributionWidget::LIFModelDistributionWidget(QWidget *parent) :
    NCSDistributionWidgetPlugin(parent)
{
    m_commandBridge = NULL;
    m_launching = false;
    m_distributionOutputDir = "ncsout";

    m_layout = new QVBoxLayout();

    m_mainGroupVector = new QGroupVector();

    m_modelLayout = new QHBoxLayout();
    m_modelFileEdit = new QLineEdit();
    m_modelLayout->addWidget(m_modelFileEdit);
    m_loadModelFileButton = new QPushButton("Load Model File ");
    connect(m_loadModelFileButton,SIGNAL(clicked()),this,SLOT(m_modelLoadPressed()));
    m_modelLayout->addWidget(m_loadModelFileButton);
    m_mainGroupVector->addGroup(m_modelLayout,"Model File:");

    m_modelIncludeLayout = new QVBoxLayout();
    m_modelDependencyWidget = new QListWidget();
    m_modelDependencyWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    m_modelIncludeLayout->addWidget(m_modelDependencyWidget);

    m_modelButtonVector = new QWidgetVector();
    m_appendModelFileButton = new QPushButton("Append Model Include");
    connect(m_appendModelFileButton,SIGNAL(clicked()),this,SLOT(m_modelAppendPressed()));
    m_modelButtonVector->addWidget(m_appendModelFileButton);
    m_removeModelFileButton = new QPushButton("Remove Model Include");
    connect(m_removeModelFileButton,SIGNAL(clicked()),this,SLOT(m_modelRemovePressed()));
    m_modelButtonVector->addWidget(m_removeModelFileButton);

    m_modelIncludeLayout->addWidget(m_modelButtonVector);
    m_mainGroupVector->addGroup(m_modelIncludeLayout,"Model Include Files:");

    m_layout->addWidget(m_mainGroupVector);

    m_clusterWidgetVector = new QWidgetVector();
    m_clusterFileLabel = new QLabel("Load Cluster to Distribute across: ");
    m_clusterWidgetVector->addWidget(m_clusterFileLabel);
    m_clusterFileEdit = new QLineEdit();
    m_clusterWidgetVector->addWidget(m_clusterFileEdit);
    m_browseClusterButton = new QPushButton("Browse");
    connect(m_browseClusterButton,SIGNAL(clicked()),this,SLOT(m_browseClusterPressed()));
    m_clusterWidgetVector->addWidget(m_browseClusterButton);
    m_layout->addWidget(m_clusterWidgetVector);

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
    //m_launchSimulationButton->setEnabled(false);
    m_layout->addWidget(m_launchSimulationButton);
    this->setLayout(m_layout);
    this->setEnabled(false);
    m_currentApplication = NULL;
}

void LIFModelDistributionWidget::openPortal(NCSProjectPortal project)
{
    //QDomElement element = project.newElement("data");
    //project.rootElement().appendChild(element);
    m_projectDir = project.parentDirectory();
    m_topologyFilename = m_projectDir + "/tmp/topology";
    m_modelFileEdit->clear();
    m_modelDependencyWidget->clear();
    m_clusterFileEdit->clear();
    m_timeSpinBox->setValue(10);
    m_timeUnitsComboBox->setCurrentIndex(3);
}

void LIFModelDistributionWidget::closePortal()
{
}

QString LIFModelDistributionWidget::name()
{
    return "lif-launcher";
}

float LIFModelDistributionWidget::version()
{
    return 1.0;
}


void LIFModelDistributionWidget::setCommandBridge(NCSCommandBridge * bridge)
{
    this->setEnabled(true);
    m_commandBridge = bridge;

}
void LIFModelDistributionWidget::initialize()
{
}

void LIFModelDistributionWidget::cleanup()
{
}

void LIFModelDistributionWidget::m_modelLoadPressed()
{
    QString modelFile = QFileDialog::getOpenFileName(this,"Open Model Files",m_projectDir,tr("NCS Model Input (*.in)"));
    if (modelFile != "")
        m_modelFileEdit->setText(modelFile);
}

void LIFModelDistributionWidget:: m_modelAppendPressed()
{
    QStringList list = QFileDialog::getOpenFileNames(this,"Open Model Files",m_projectDir,tr("NCS Model Include (*.inc)"));
    for (int i = 0;i < list.size(); i ++)
        m_modelDependencyWidget->addItem(list.at(i));
}

void LIFModelDistributionWidget:: m_modelRemovePressed()
{
    QList<QListWidgetItem*> itemsSelected = m_modelDependencyWidget->selectedItems();
    for (int i = 0;i < itemsSelected.size(); i ++)
    {
        m_modelDependencyWidget->removeItemWidget(itemsSelected[i]);
        delete itemsSelected[i];
    }

}

void LIFModelDistributionWidget::m_browseClusterPressed()
{
    QString clusterPath = QFileDialog::getOpenFileName(this,"Find Cluster File",m_projectDir);
    if (clusterPath != "")
        m_clusterFileEdit->setText(clusterPath);

}


void  LIFModelDistributionWidget::m_toy()
{
}

void LIFModelDistributionWidget::m_distributionStarted(NCSApplicationBridge * app)
{
    m_currentApplication = app;
    disconnect(m_commandBridge,SIGNAL(applicationBridgeLaunched(NCSApplicationBridge*)),this,SLOT(m_distributionStarted(NCSApplicationBridge*)));
    connect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_distributionFailed(NCSApplicationBridge::ApplicationError)));
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_distributionFinished()));
    connect(m_currentApplication,SIGNAL(readyReadStandardOutput()),this,SLOT(m_readDistributorOutput()));
    connect(m_currentApplication,SIGNAL(readyReadStandardError()),this,SLOT(m_readDistributorError()));
}

void LIFModelDistributionWidget::m_destroyDistribution()
{
    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_distributionFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_distributionFinished()));
    disconnect(m_currentApplication,SIGNAL(readyReadStandardOutput()),this,SLOT(m_readDistributorOutput()));
    disconnect(m_currentApplication,SIGNAL(readyReadStandardError()),this,SLOT(m_readDistributorError()));
    delete m_currentApplication;
    m_currentApplication = NULL;
}

void LIFModelDistributionWidget::m_simulationStarted(NCSApplicationBridge * app)
{
    m_currentApplication = app;
    disconnect(m_commandBridge,SIGNAL(applicationBridgeLaunched(NCSApplicationBridge*)),this,SLOT(m_simulationStarted(NCSApplicationBridge*)));
    connect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_simulationFailed(NCSApplicationBridge::ApplicationError)));
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_simulationFinished()));
    connect(m_currentApplication,SIGNAL(readyReadStandardOutput()),this,SLOT(m_readSimulatorOutput()));
    connect(m_currentApplication,SIGNAL(readyReadStandardError()),this,SLOT(m_readSimulatorError()));
}
void LIFModelDistributionWidget::m_destroySimulation()
{
    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_simulationFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_simulationFinished()));
    disconnect(m_currentApplication,SIGNAL(readyReadStandardOutput()),this,SLOT(m_readSimulatorOutput()));
    disconnect(m_currentApplication,SIGNAL(readyReadStandardError()),this,SLOT(m_readSimulatorError()));
    delete m_currentApplication;
    m_currentApplication = NULL;
}


void LIFModelDistributionWidget::m_launchSimulationPressed()
{
    if (m_launching)
        return;

    if (!m_fieldsValid())
        return;

    m_launching = true;
    QVector<NCSCommandFileArgument> modelFileDependencies;

    for (int i = 0;i < m_modelDependencyWidget->count(); i ++)
    {
        QString dependency = m_modelDependencyWidget->item(i)->text();
#ifdef Win32
        QStringList dependencySegments = dependency.split("\\");
#else
        QStringList dependencySegments = dependency.split("/");
#endif
        QString dependencySim = dependencySegments.at(dependencySegments.size() -1);
        modelFileDependencies.append(NCSCommandFileArgument(dependencySim,dependency,NCSCommandFileArgument::UploadBeforeExecution));
    }

#ifdef Win32
    QStringList modelPathSegments = m_modelFileEdit->text().split("\\");
    QStringList clusterPathSegments = m_clusterFileEdit->text().split("\\");

#else
    QStringList modelPathSegments = m_modelFileEdit->text().split("/");
    QStringList clusterPathSegments = m_clusterFileEdit->text().split("/");
#endif
    QString modelFile = modelPathSegments.at(modelPathSegments.size()-1);
    QString clusterFile = clusterPathSegments.at(clusterPathSegments.size()-1);

    qDebug() << modelFileDependencies.count();

    NCSCommandArguments distArgs;
    distArgs << NCSCommandFileArgument(modelFile,m_modelFileEdit->text(),NCSCommandFileArgument::UploadBeforeExecution,modelFileDependencies);
    distArgs << NCSCommandFileArgument(clusterFile,m_clusterFileEdit->text(),NCSCommandFileArgument::UploadBeforeExecution);
    distArgs << m_distributionOutputDir << "-topology" << NCSCommandFileArgument("topology",m_topologyFilename,NCSCommandFileArgument::DownloadAfterExecution);
    connect(m_commandBridge,SIGNAL(applicationBridgeLaunched(NCSApplicationBridge*)),this,SLOT(m_distributionStarted(NCSApplicationBridge*)));
    launchTriggered();
    m_commandBridge->launchApplicationBridge("ncsDistributor",distArgs);
}

bool LIFModelDistributionWidget::m_fieldsValid()
{
    QMessageBox msgBox;
    msgBox.addButton("Ok", QMessageBox::ActionRole);
    if (!QFile(m_modelFileEdit->text()).exists())
    {
        msgBox.setText("The model file entered could not be found." );
        msgBox.exec();
        return false;
    }
    for (int i =0; i < m_modelDependencyWidget->count(); i ++)
    {
        if (!QFile(m_modelDependencyWidget->item(i)->text()).exists())
        {
            msgBox.setText("The model dependency '" + m_modelDependencyWidget->item(i)->text() + "'' could not be found." );
            msgBox.exec();
            return false;
        }
    }
    if (!QFile(m_clusterFileEdit->text()).exists())
    {
        msgBox.setText("The cluster file entered could not be found." );
        msgBox.exec();
        return false;
    }

    return true;
}

void LIFModelDistributionWidget::m_distributionFailed(NCSApplicationBridge::ApplicationError err)
{

    m_destroyDistribution();

    if (!m_launching)
        return;

    QMessageBox msgBox;
    msgBox.addButton("Ok", QMessageBox::ActionRole);
    msgBox.setText("Simulation Distribution Failed." );

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
    launchFailed();

}

void LIFModelDistributionWidget::m_timeUnitSelected(QString unit)
{
    if (unit == "Forever")
        m_timeSpinBox->setEnabled(false);
    else
        m_timeSpinBox->setEnabled(true);

}
void LIFModelDistributionWidget::m_distributionFinished()
{
    m_destroyDistribution();

    if (!m_launching)
        return;

    distributed(m_topologyFilename );

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

    connect(m_commandBridge,SIGNAL(applicationBridgeLaunched(NCSApplicationBridge*)),this,SLOT(m_simulationStarted(NCSApplicationBridge*)));
    m_commandBridge->launchApplicationBridge("simulator",simArgs,cluster.machines.count(),hostFilePath);

}

void LIFModelDistributionWidget::m_simulationFailed(NCSApplicationBridge::ApplicationError err)
{
    if (!m_launching)
        return;

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


void LIFModelDistributionWidget::m_readDistributorError()
{
    QString err = m_currentApplication->readAllStandardError();
    int errorIndex = err.indexOf(QRegExp("File:.*syntax error"));
    if (errorIndex != -1 )
    {
        m_launching = false;
        launchFailed();

        err.replace("File: ","");
        err.replace(": syntax error","");
        QStringList strings = err.split(":");


        QMessageBox msgBox;
        msgBox.addButton("Ok", QMessageBox::ActionRole);
        msgBox.setText("Simulation Distribution Failed." );
        msgBox.setDetailedText("There was a syntax error on line" +strings[1] + "in the file" + strings[0] );
        msgBox.exec();
    }


}
void LIFModelDistributionWidget::m_readDistributorOutput()
{
    QString out = m_currentApplication->readAllStandardOutput();

    if (m_launching)
        qDebug() << out;

}

void LIFModelDistributionWidget::m_readSimulatorError()
{
    QString err = m_currentApplication->readAllStandardError();
    qDebug() << err;

}
void LIFModelDistributionWidget::m_readSimulatorOutput()
{
    QString out = m_currentApplication->readAllStandardOutput();

    if (m_launching)
    {
        qDebug() << out;
        if (m_launching && out.contains("Running simulation..."))
        {
            m_launching = false;
            launched(m_reportHost);
        }
    }
}

void LIFModelDistributionWidget::m_simulationFinished()
{
    m_launching = false;
    m_destroySimulation();
}

QIcon LIFModelDistributionWidget::icon()
{
    return QIcon(":/resources/images/lifDistributor.png");
}

QString LIFModelDistributionWidget::title()
{
    return "LIF Distributor";
}
