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
    connect(m_clusterFileEdit,SIGNAL(textChanged(QString)),this,SLOT(m_clusterEditChanged(QString)));
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
    m_launched = false;
}

void LIFModelDistributionWidget::loadProject(QString projectDir)
{
    m_projectDir = projectDir;
    m_topologyFilename = m_projectDir + "/tmp/topology";
    m_modelFileEdit->clear();
    m_modelDependencyWidget->clear();
    m_clusterFileEdit->clear();
    m_timeSpinBox->setValue(10);
    m_timeUnitsComboBox->setCurrentIndex(3);
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

void LIFModelDistributionWidget:: m_clusterEditChanged(QString newText)
{
    //m_commandBridge->checkForFile(newText);
    //connect(m_commandBridge,SIGNAL(fileFound(filepath)),this,SLOT(m_clusterEditValid(QString)));
    //connect(m_commandBridge,SIGNAL(fileNotFound(filepath)),this,SLOT(m_clusterEditInvalid(QString)));
}

void LIFModelDistributionWidget:: m_clusterEditValid(QString newText)
{
    m_clusterValid = true;
    disconnect(m_commandBridge,SIGNAL(fileFound(filepath)),this,SLOT(m_clusterEditValid(QString)));
    disconnect(m_commandBridge,SIGNAL(fileNotFound(filepath)),this,SLOT(m_clusterEditInvalid(QString)));
}
void LIFModelDistributionWidget:: m_clusterEditInvalid(QString newText)
{
    m_clusterValid = false;
    disconnect(m_commandBridge,SIGNAL(fileFound(filepath)),this,SLOT(m_clusterEditValid(QString)));
    disconnect(m_commandBridge,SIGNAL(fileNotFound(filepath)),this,SLOT(m_clusterEditInvalid(QString)));
}

void LIFModelDistributionWidget::m_browseClusterPressed()
{
    QString clusterPath = QFileDialog::getOpenFileName(this,"Find Cluster File",m_projectDir);
    if (clusterPath != "")
        m_clusterFileEdit->setText(clusterPath);
}



void LIFModelDistributionWidget::m_distributionStarted(NCSApplicationBridge * app)
{
    m_currentApplication = app;
    disconnect(m_commandBridge,SIGNAL(applicationStarted(NCSApplicationBridge*)),this,SLOT(m_distributionStarted(NCSApplicationBridge*)));
    connect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_distributionFailed(NCSApplicationBridge::ApplicationError)));
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_distributionFinished()));
}

void LIFModelDistributionWidget::m_destroyDistribution()
{
    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_distributionFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_distributionFinished()));
    delete m_currentApplication;
    m_currentApplication = NULL;
}

void LIFModelDistributionWidget::m_simulationStarted(NCSApplicationBridge * app)
{
    m_currentApplication = app;
    disconnect(m_commandBridge,SIGNAL(applicationStarted(NCSApplicationBridge*)),this,SLOT(m_simulationStarted(NCSApplicationBridge*)));
    connect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_simulationFailed(NCSApplicationBridge::ApplicationError)));
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_simulationFinished()));
    connect(m_currentApplication,SIGNAL(readyReadStandardOutput()),this,SLOT(m_readStandardOutput()));
    connect(m_currentApplication,SIGNAL(readyReadStandardError()),this,SLOT(m_readStandardError()));
}
void LIFModelDistributionWidget::m_destroySimulation()
{
    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_simulationFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_simulationFinished()));
    disconnect(m_currentApplication,SIGNAL(readyReadStandardOutput()),this,SLOT(m_readStandardOutput()));
    disconnect(m_currentApplication,SIGNAL(readyReadStandardError()),this,SLOT(m_readStandardError()));
    delete m_currentApplication;
    m_currentApplication = NULL;
}


void LIFModelDistributionWidget::m_launchSimulationPressed()
{
    if (m_launching)
        return;

    if (m_modelFileEdit->text() == "")
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
    connect(m_commandBridge,SIGNAL(applicationStarted(NCSApplicationBridge*)),this,SLOT(m_distributionStarted(NCSApplicationBridge*)));
    launchTriggered();
    m_commandBridge->launchApplication("ncsDistributor",distArgs);
}

void LIFModelDistributionWidget::m_distributionFailed(NCSApplicationBridge::ApplicationError err)
{
    QMessageBox msgBox;
    m_readStandardOutput();
    m_readStandardError();
    msgBox.addButton("Ok", QMessageBox::ActionRole);
    msgBox.setText("Distribution Failed." );
    msgBox.exec();
    m_launching = false;
    launchFailed();
    m_destroyDistribution();

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
    distributed(m_topologyFilename );
    m_readStandardOutput();
    m_readStandardError();
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

    connect(m_commandBridge,SIGNAL(applicationStarted(NCSApplicationBridge*)),this,SLOT(m_simulationStarted(NCSApplicationBridge*)));
    m_commandBridge->launchApplication("simulator",simArgs,cluster.machines.count(),hostFilePath);

}

void LIFModelDistributionWidget::m_simulationFailed(NCSApplicationBridge::ApplicationError err)
{
    m_readStandardOutput();
    m_readStandardError();
    QMessageBox msgBox;
    msgBox.addButton("Ok", QMessageBox::ActionRole);
    msgBox.setText("Simulation Launching Failed." );
    msgBox.exec();
    m_launching = false;
    m_destroySimulation();
}

void LIFModelDistributionWidget::m_readStandardError()
{
    qDebug() << m_currentApplication->readAllStandardError();
}
void LIFModelDistributionWidget::m_readStandardOutput()
{
    QString out = m_currentApplication->readAllStandardOutput();

    if (!m_launched)
        qDebug() << out;

    if (out.contains("Running simulation..."))
    {
        m_launched = true;
        launched(m_reportHost);
    }
}

void LIFModelDistributionWidget::m_simulationFinished()
{
    m_launching = false;
    m_destroySimulation();
}

QIcon LIFModelDistributionWidget::icon()
{
    return QIcon(":/media/lifDistributor.png");
}

QString LIFModelDistributionWidget::title()
{
    return "LIF Distributor";
}
