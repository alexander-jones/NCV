#include "ncsmodeldistributionwidget.h"
#include <QMessageBox>
#include <QFileDialog>

NCSModelDistributionWidget::NCSModelDistributionWidget(QString projectDir,QWidget *parent) :
    NCSDistributionWidgetPlugin(projectDir,parent)
{
    m_projectDir = projectDir;
    m_commandBridge = NULL;
    m_launching = false;
    m_distributionOutputDir = "ncsout";
    m_topologyFilename = m_projectDir + "/tmp/topology";

    m_layout = new QVBoxLayout();

    m_mainGroupVector = new QGroupVector();
    m_mainGroupVector->setDirection(QGroupVector::TopToBottom);
    m_modelWidgetVector = new QWidgetVector();
    m_modelFileEdit = new QLineEdit();
    m_modelWidgetVector->addWidget(m_modelFileEdit);
    m_loadModelFileButton = new QPushButton("Load Model File ");
    connect(m_loadModelFileButton,SIGNAL(clicked()),this,SLOT(m_modelLoadPressed()));
    m_modelWidgetVector->addWidget(m_loadModelFileButton);
    m_mainGroupVector->addGroup("Model File:");
    m_mainGroupVector->addToGroup("Model File:",m_modelWidgetVector);

    m_modelIncludeVector = new QWidgetVector();
    m_modelIncludeVector->setDirection(QWidgetVector::TopToBottom);
    m_modelDependencyWidget = new QListWidget();
    m_modelDependencyWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    m_modelIncludeVector->addWidget(m_modelDependencyWidget);

    m_modelButtonVector = new QWidgetVector();
    m_appendModelFileButton = new QPushButton("Append Model Include");
    connect(m_appendModelFileButton,SIGNAL(clicked()),this,SLOT(m_modelAppendPressed()));
    m_modelButtonVector->addWidget(m_appendModelFileButton);
    m_removeModelFileButton = new QPushButton("Remove Model Include");
    connect(m_removeModelFileButton,SIGNAL(clicked()),this,SLOT(m_modelRemovePressed()));
    m_modelButtonVector->addWidget(m_removeModelFileButton);

    m_modelIncludeVector->addWidget(m_modelButtonVector);
    m_mainGroupVector->addGroup("Model Include Files:");
    m_mainGroupVector->addToGroup("Model Include Files:",m_modelIncludeVector);

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
    m_timeSpinBox->setMinimum(0);
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

void NCSModelDistributionWidget::initialize(NCSCommandBridge * bridge)
{
    this->setEnabled(true);
    m_commandBridge = bridge;

}


void NCSModelDistributionWidget::m_modelLoadPressed()
{
    QString modelFile = QFileDialog::getOpenFileName(this,"Open Model Files",m_projectDir,tr("NCS Model Input (*.in)"));
    if (modelFile != "")
        m_modelFileEdit->setText(modelFile);
}

void NCSModelDistributionWidget:: m_modelAppendPressed()
{
    QStringList list = QFileDialog::getOpenFileNames(this,"Open Model Files",m_projectDir,tr("NCS Model Include (*.inc)"));
    for (int i = 0;i < list.size(); i ++)
        m_modelDependencyWidget->addItem(list.at(i));
}

void NCSModelDistributionWidget:: m_modelRemovePressed()
{
    QList<QListWidgetItem*> itemsSelected = m_modelDependencyWidget->selectedItems();
    for (int i = 0;i < itemsSelected.size(); i ++)
    {
        m_modelDependencyWidget->removeItemWidget(itemsSelected[i]);
        delete itemsSelected[i];
    }

}

void NCSModelDistributionWidget:: m_clusterEditChanged(QString newText)
{
    //m_commandBridge->checkForFile(newText);
    //connect(m_commandBridge,SIGNAL(fileFound(filepath)),this,SLOT(m_clusterEditValid(QString)));
    //connect(m_commandBridge,SIGNAL(fileNotFound(filepath)),this,SLOT(m_clusterEditInvalid(QString)));
}

void NCSModelDistributionWidget:: m_clusterEditValid(QString newText)
{
    m_clusterValid = true;
    disconnect(m_commandBridge,SIGNAL(fileFound(filepath)),this,SLOT(m_clusterEditValid(QString)));
    disconnect(m_commandBridge,SIGNAL(fileNotFound(filepath)),this,SLOT(m_clusterEditInvalid(QString)));
}
void NCSModelDistributionWidget:: m_clusterEditInvalid(QString newText)
{
    m_clusterValid = false;
    disconnect(m_commandBridge,SIGNAL(fileFound(filepath)),this,SLOT(m_clusterEditValid(QString)));
    disconnect(m_commandBridge,SIGNAL(fileNotFound(filepath)),this,SLOT(m_clusterEditInvalid(QString)));
}

void NCSModelDistributionWidget::m_browseClusterPressed()
{
    QString clusterPath = QFileDialog::getOpenFileName(this,"Find Cluster File",m_projectDir);
    if (clusterPath != "")
        m_clusterFileEdit->setText(clusterPath);
}
void NCSModelDistributionWidget::m_launchSimulationPressed()
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


    NCSCommandArguments distArgs;
    distArgs << NCSCommandFileArgument(modelFile,m_modelFileEdit->text(),NCSCommandFileArgument::UploadBeforeExecution,modelFileDependencies);
    distArgs << NCSCommandFileArgument(clusterFile,m_clusterFileEdit->text(),NCSCommandFileArgument::UploadBeforeExecution);
    distArgs << NCSCommandFileArgument(m_distributionOutputDir) << "-topology" << NCSCommandFileArgument("topology",m_topologyFilename,NCSCommandFileArgument::DownloadAfterExecution);
    m_currentApplication = m_commandBridge->executeApplication("ncsDistributor",distArgs);
    connect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_distributionFailed(NCSApplicationBridge::ApplicationError)));
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_distributionFinished()));

}

void NCSModelDistributionWidget::m_distributionFailed(NCSApplicationBridge::ApplicationError err)
{
    QMessageBox msgBox;
    msgBox.addButton("Ok", QMessageBox::ActionRole);
    msgBox.setText("Distribution Failed." );
    msgBox.exec();
    m_launching = false;
    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_distributionFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_distributionFinished()));

    delete m_currentApplication;
    m_currentApplication= NULL;

}

void NCSModelDistributionWidget::m_timeUnitSelected(QString unit)
{
    if (unit == "Forever")
        m_timeSpinBox->setEnabled(false);
    else
        m_timeSpinBox->setEnabled(true);

}
void NCSModelDistributionWidget::m_distributionFinished()
{
    distributed(m_topologyFilename );
    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_distributionFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_distributionFinished()));

    m_readStandardOutput();
    m_readStandardError();
    delete m_currentApplication;
    m_currentApplication= NULL;

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
    simArgs << NCSCommandFileArgument(m_distributionOutputDir) << timeArg;
    m_currentApplication = m_commandBridge->executeApplication("simulator",simArgs);
    connect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_simulationFailed(NCSApplicationBridge::ApplicationError)));
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_simulationFinished()));
    connect(m_currentApplication,SIGNAL(readyReadStandardOutput()),this,SLOT(m_readStandardOutput()));
    connect(m_currentApplication,SIGNAL(readyReadStandardError()),this,SLOT(m_readStandardError()));

}

void NCSModelDistributionWidget::m_simulationFailed(NCSApplicationBridge::ApplicationError err)
{
    QMessageBox msgBox;
    msgBox.addButton("Ok", QMessageBox::ActionRole);
    msgBox.setText("Simulation Launching Failed." );
    msgBox.exec();
    m_launching = false;
    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_simulationFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_simulationFinished()));
    delete m_currentApplication;
    m_currentApplication= NULL;

}

void NCSModelDistributionWidget::m_readStandardError()
{
    qDebug() << m_currentApplication->readAllStandardError();
}
void NCSModelDistributionWidget::m_readStandardOutput()
{
    qDebug() << m_currentApplication->readAllStandardOutput();
}

void NCSModelDistributionWidget::m_simulationFinished()
{
    launched();
    m_launching = false;
    m_currentApplication->disconnect();
    delete m_currentApplication;
    m_currentApplication= NULL;

}

QIcon NCSModelDistributionWidget::icon()
{
    return QIcon(":/assets/ncsDistributor.png");
}

QString NCSModelDistributionWidget::title()
{
    return "NCS Distributor";
}