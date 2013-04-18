
#include "ncsclustereditor.h"
#include <QByteArray>

NCSClusterEditor::NCSClusterEditor(QString projectDir,QWidget * parent)
    :NCSApplicationWidgetPlugin(projectDir,parent)
{
    m_projectDir = projectDir;
    m_layout = new QVBoxLayout();

    /*m_ipRangeDetectionVector = new QWidgetVector();
    m_machinesLabel = new QLabel("Search IP Range:");
    m_ipRangeDetectionVector->addWidget(m_machinesLabel);
    m_startIP = new QLineEdit();
    m_ipRangeDetectionVector->addWidget(m_startIP);
    m_endIP = new QLineEdit();
    m_ipRangeDetectionVector->addWidget(m_endIP);
    m_ipDetectDevicesButton = new QPushButton( "Detect Devices");
    m_ipDetectDevicesButton->setEnabled(false);
    connect(m_ipDetectDevicesButton,SIGNAL(clicked()),this,SLOT(m_detectDevicesOverIPRange()));
    m_ipRangeDetectionVector->addWidget(m_ipDetectDevicesButton);
    m_layout->addWidget(m_ipRangeDetectionVector);*/

    m_hostfileDetectionVector = new QWidgetVector();
    m_hostfileLabel = new QLabel("Detect With Hostfile:");
    m_hostfileDetectionVector->addWidget(m_hostfileLabel);
    m_hostFileEdit = new QLineEdit();
    connect(m_hostFileEdit,SIGNAL(textChanged(QString)),this,SLOT(m_hostFileEditChanged(QString)));
    m_hostfileDetectionVector->addWidget(m_hostFileEdit);
    m_hostfileBrowseButton = new QPushButton( "Browse");
    m_hostfileBrowseButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    connect(m_hostfileBrowseButton,SIGNAL(clicked()),this,SLOT(m_browseForHostFile()));
    m_hostfileDetectionVector->addWidget(m_hostfileBrowseButton);
    m_hostfileDetectAndLoadButton = new QPushButton( "Load Devices");
    m_hostfileDetectAndLoadButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    m_hostfileDetectAndLoadButton->setEnabled(false);
    connect(m_hostfileDetectAndLoadButton,SIGNAL(clicked()),this,SLOT(m_hostfileDetectAndLoad()));
    m_hostfileDetectionVector->addWidget(m_hostfileDetectAndLoadButton);
    m_hostfileDetectAndAppendButton = new QPushButton( "Append Devices");
    m_hostfileDetectAndAppendButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    m_hostfileDetectAndAppendButton->setEnabled(false);
    connect(m_hostfileDetectAndAppendButton,SIGNAL(clicked()),this,SLOT(m_hostfileDetectAndAppend()));
    m_hostfileDetectionVector->addWidget(m_hostfileDetectAndAppendButton);
    m_layout->addWidget(m_hostfileDetectionVector);


    m_optionVector = new QWidgetVector();
    m_optionVector->setAlignment(Qt::AlignRight);
    m_allCPUCheckBox = new QCheckBox();
    m_allCPUCheckBox->setText("All CPU");
    connect(m_allCPUCheckBox,SIGNAL(clicked(bool)),this,SLOT(m_allCPUSelected(bool)));
    m_optionVector->addWidget(m_allCPUCheckBox);

    m_allGPUCheckBox = new QCheckBox();
    m_allGPUCheckBox->setText("All GPU");
    connect(m_allGPUCheckBox,SIGNAL(clicked(bool)),this,SLOT(m_allGPUSelected(bool)));
    m_optionVector->addWidget(m_allGPUCheckBox);

    m_onlyCPUCheckBox = new QCheckBox();
    m_onlyCPUCheckBox->setText("Only CPU");
    connect(m_onlyCPUCheckBox,SIGNAL(clicked(bool)),this,SLOT(m_onlyCPUSelected(bool)));
    m_optionVector->addWidget(m_onlyCPUCheckBox);
    m_onlyGPUCheckBox = new QCheckBox();
    m_onlyGPUCheckBox->setText("Only GPU");
    connect(m_onlyGPUCheckBox,SIGNAL(clicked(bool)),this,SLOT(m_onlyGPUSelected(bool)));
    m_optionVector->addWidget(m_onlyGPUCheckBox);
    m_layout->addWidget(m_optionVector);

    m_machineWidgdet = new QWidget();
    m_mainLayout = new QHBoxLayout();

	
    QVBoxLayout * machineLayout = new QVBoxLayout();
    m_mainLayout->addLayout(machineLayout);

    QGroupBox * machineListBox = new QGroupBox(tr("Machines"));

    m_machineList = new QListWidget();

    QVBoxLayout * machineListLayout = new QVBoxLayout();
    machineListLayout->addWidget(m_machineList);
	machineListBox->setLayout(machineListLayout);
	machineLayout->addWidget(machineListBox);
    connect(m_machineList,SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(machineSelected(QListWidgetItem*, QListWidgetItem*)));
    connect(m_machineList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(machineEnabled(QListWidgetItem*)));
    m_machineBox = new QGroupBox(tr("Machine Info"));
    machineLayout->addWidget(m_machineBox);

    QVBoxLayout * deviceLayout = new QVBoxLayout();
    m_mainLayout->addLayout(deviceLayout);

    m_deviceListBox = new QGroupBox(tr("Devices"));
    m_deviceList = new QListWidget();
    QVBoxLayout * deviceListLayout = new QVBoxLayout();
    deviceListLayout->addWidget(m_deviceList);
    m_deviceListBox->setLayout(deviceListLayout);
    deviceLayout->addWidget(m_deviceListBox);

    connect(m_deviceList,SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),this,SLOT(deviceSelected(QListWidgetItem*, QListWidgetItem*)));    
    connect(m_deviceList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(deviceEnabled(QListWidgetItem*)));
	
    m_deviceBox = new QGroupBox(tr("Device Info"));
    deviceLayout->addWidget(m_deviceBox);


    m_machineWidgdet->setLayout(m_mainLayout);
    m_layout->addWidget(m_machineWidgdet);

    m_clusterFileOptionVector = new QWidgetVector();
    m_layout->addWidget(m_clusterFileOptionVector);

    m_loadClusterFileButton = new QPushButton("Load Cluster from File");
    connect(m_loadClusterFileButton,SIGNAL(clicked()),this,SLOT(m_loadClusterFilePressed()));
    m_clusterFileOptionVector->addWidget(m_loadClusterFileButton);

    m_appendClusterFileButton= new QPushButton("Append Cluster from File");
    connect(m_appendClusterFileButton,SIGNAL(clicked()),this,SLOT(m_appendClusterFilePressed()));
    m_clusterFileOptionVector->addWidget(m_appendClusterFileButton);

    m_saveClusterFileButton = new QPushButton("Save Cluster to File");
    connect(m_saveClusterFileButton,SIGNAL(clicked()),this,SLOT(m_saveClusterFilePressed()));
    m_clusterFileOptionVector->addWidget(m_saveClusterFileButton);

    m_clearButton = new QPushButton("Clear All Machines");
    connect(m_clearButton,SIGNAL(clicked()),this,SLOT(clear()));
    m_layout->addWidget(m_clearButton);

    this->setLayout(m_layout);
    this->setEnabled(false);
}

QIcon NCSClusterEditor::icon()
{
    return QIcon(":/assets/hardwareIcon.png");
}

QString NCSClusterEditor::title()
{
    return "Cluster Editor";
}

void NCSClusterEditor::initialize(NCSCommandBridge *bridge)
{
    m_commandBridge = bridge;
    this->setEnabled(true);
}

void NCSClusterEditor::m_hostFileEditChanged(QString newText)
{
    if (QFile(newText).exists())
    {
        m_hostfileDetectAndLoadButton->setEnabled(true);
        m_hostfileDetectAndAppendButton->setEnabled(true);
    }
    else
    {
        m_hostfileDetectAndLoadButton->setEnabled(false);
        m_hostfileDetectAndAppendButton->setEnabled(false);
    }
}

void NCSClusterEditor::m_browseForHostFile()
{
    QString hostFile = QFileDialog::getOpenFileName(this,"Open MPI Hostfile",m_projectDir);
    if (hostFile != "")
        m_hostFileEdit->setText(hostFile);
}

void NCSClusterEditor::m_hostfileDetectAndAppend()
{
    m_appendDetectedHosts = true;

    int numMachines = 0;
    QFile file(m_hostFileEdit->text());
    file.open(QFile::ReadOnly);
    while (!file.atEnd())
    {
        file.readLine();
        numMachines++;
    }
    file.close();


    NCSCommandArguments clusterArgs;
    clusterArgs << NCSCommandFileArgument("cluster",m_projectDir + "/tmp/cluster",NCSCommandFileArgument::DownloadAfterExecution);

    m_currentApplication  = m_commandBridge->executeApplication("clusterSpecifier",clusterArgs,numMachines,m_hostFileEdit->text());
    connect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_clusterCompilationFailed(NCSApplicationBridge::ApplicationError)));
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_clusterCompilationFinished()));

}

void NCSClusterEditor::m_hostfileDetectAndLoad()
{
    m_appendDetectedHosts = false;
    int numMachines = 0;
    QFile file(m_hostFileEdit->text());
    file.open(QFile::ReadOnly);
    while (!file.atEnd())
    {
        file.readLine();
        numMachines++;
    }
    file.close();

    NCSCommandArguments clusterArgs;
    clusterArgs << NCSCommandFileArgument("cluster",m_projectDir + "/tmp/cluster",NCSCommandFileArgument::DownloadAfterExecution);

    m_currentApplication  = m_commandBridge->executeApplication("clusterSpecifier",clusterArgs,numMachines,m_hostFileEdit->text());
    connect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_clusterCompilationFailed(NCSApplicationBridge::ApplicationError)));
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_clusterCompilationFinished()));

}

void NCSClusterEditor::m_detectDevicesOverIPRange()
{
    int devicesDetected = 1;

}
void NCSClusterEditor::m_clusterCompilationFailed(NCSApplicationBridge::ApplicationError err)
{
    QMessageBox msgBox;
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
    if (err == NCSApplicationBridge::Timedout)
        msgBox.setText("<font color = 'red'> Error. MPI timed out when processing cluster file.</font>");

    else if (err == NCSApplicationBridge::FailedToStart)
        msgBox.setText("<font color = 'red'> Error. Cluster file could not be started. Is it present in the host's build/application directory?.</font>");
    msgBox.exec();

    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_clusterCompilationFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_clusterCompilationFinished()));

}

void NCSClusterEditor::m_clusterCompilationFinished()
{


    loadClusterFile(m_projectDir + "/tmp/cluster",m_appendDetectedHosts);
    disconnect(m_currentApplication,SIGNAL(executionError(NCSApplicationBridge::ApplicationError)),this,SLOT(m_clusterCompilationFailed(NCSApplicationBridge::ApplicationError)));
    disconnect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_clusterCompilationFinished()));

}

void NCSClusterEditor::m_loadClusterFilePressed()
{
    QString filepath = QFileDialog::getOpenFileName(this,"Open Cluster File",m_projectDir);
    if (filepath != "")
        loadClusterFile(filepath);

}

void NCSClusterEditor::m_saveClusterFilePressed()
{

    QString filepath = QFileDialog::getSaveFileName(this,"Save Cluster File",m_projectDir);
    if (filepath != "")
        saveClusterFile(filepath);
}

void NCSClusterEditor::m_appendClusterFilePressed()
{
    QString filepath = QFileDialog::getOpenFileName(this,"Open Cluster File",m_projectDir);
    if (filepath != "")
        loadClusterFile(filepath,true);
}

NCSClusterEditor::LabelSet NCSClusterEditor::m_infoParam(const QString & key, const QString & value)
{
    QLabel * keyLabel = new QLabel(key);
    keyLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QLabel * valueLabel = new QLabel(value);
    valueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    return LabelSet(keyLabel, valueLabel);
}


void NCSClusterEditor::machineSelected(QListWidgetItem* current, QListWidgetItem* prev)
{
    if (current == NULL)
        return;

    m_machineSelectionIndex = current->data(Qt::UserRole).toInt();
    Machine machine = m_cluster.machines[m_machineSelectionIndex];
    m_machineBox->setTitle(machine.name);
    if (m_machineBox->layout())
	{
        QLayout * layout = m_machineBox->layout();
		QLayoutItem* item;
        while ((item = layout->takeAt(0)) != NULL)
		{
			delete item->widget();
			delete item;
		}
		delete layout;
	}
    QGridLayout * layout = new QGridLayout();
    LabelSet hostInfo = m_infoParam("Hostname:", machine.name);
	layout->addWidget(hostInfo.first, 0, 0);
	layout->addWidget(hostInfo.second, 0, 1);
    m_machineBox->setLayout(layout);
    m_deviceList->clear();

    int id = 0;
    for (int i = 0; i < machine.devices.count(); i ++)
    {
        Device device = machine.devices[i];
        QListWidgetItem * item = new QListWidgetItem(device.type);
		item->setData(Qt::UserRole, id);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(device.enabled? Qt::Checked : Qt::Unchecked);
        m_deviceList->addItem(item);
		++id;
    }

}

void NCSClusterEditor::m_enableAll(QString type)
{
    QList<QListWidgetItem * > selectedItems =m_machineList->selectedItems();
    QListWidgetItem * machineSelected = NULL;
    if (selectedItems.count() > 0)
        machineSelected = selectedItems[0];
    for (int i = 0; i < m_cluster.machines.count(); i ++)
    {
        bool machineEnabled = false;

        bool selectedMachine = false;
        if (machineSelected != NULL)
            selectedMachine = machineSelected->data(Qt::UserRole).toInt() == i;


        for (int j = 0; j <m_cluster.machines[i].devices.count(); j ++)
        {
            if (m_cluster.machines[i].devices[j].type != type)
            {
                if (m_cluster.machines[i].devices[j].enabled)
                    machineEnabled = true;

            }
            else
            {
                machineEnabled = true;
                m_cluster.machines[i].devices[j].enabled = true;
            }
            if (selectedMachine)
                m_deviceList->item(j)->setCheckState(m_cluster.machines[i].devices[j].enabled? Qt::Checked : Qt::Unchecked);

        }
        m_cluster.machines[i].enabled = machineEnabled;

        QListWidgetItem * item = m_machineList->item(i);
        item->setCheckState(m_cluster.machines[i].enabled? Qt::Checked : Qt::Unchecked);

    }
}

void NCSClusterEditor::m_enableOnly(QString type)
{
    QList<QListWidgetItem * > selectedItems =m_machineList->selectedItems();
    QListWidgetItem * machineSelected = NULL;
    if (selectedItems.count() > 0)
        machineSelected = selectedItems[0];
    for (int i = 0; i < m_cluster.machines.count(); i ++)
    {
        bool machineEnabled = false;

        bool selectedMachine = false;
        if (machineSelected != NULL)
            selectedMachine = machineSelected->data(Qt::UserRole).toInt() == i;


        for (int j = 0; j <m_cluster.machines[i].devices.count(); j ++)
        {
            if (m_cluster.machines[i].devices[j].type != type)
               m_cluster.machines[i].devices[j].enabled = false;

            else if (m_cluster.machines[i].devices[j].enabled)
                machineEnabled = true;

            if (selectedMachine)
                m_deviceList->item(j)->setCheckState(m_cluster.machines[i].devices[j].enabled? Qt::Checked : Qt::Unchecked);

        }
        if (!(machineEnabled))
            m_cluster.machines[i].enabled = machineEnabled;

        QListWidgetItem * item = m_machineList->item(i);
        item->setCheckState(m_cluster.machines[i].enabled? Qt::Checked : Qt::Unchecked);

    }
}
void NCSClusterEditor::m_onlyCPUSelected(bool set)
{
    if (set)
    {
        m_allGPUCheckBox->setChecked(false);
        m_onlyGPUCheckBox->setChecked(false);
        m_enableOnly("CPU");
    }

}

void NCSClusterEditor::m_onlyGPUSelected(bool set)
{
    if (set)
    {
        m_allCPUCheckBox->setChecked(false);
        m_onlyCPUCheckBox->setChecked(false);
        m_enableOnly("CUDA");
    }
}

void NCSClusterEditor::m_allGPUSelected(bool set)
{

    if (set)
    {
        m_onlyCPUCheckBox->setChecked(false);
        m_enableAll("CUDA");
    }

}

void NCSClusterEditor::m_allCPUSelected(bool set)
{

    if (set)
    {
        m_onlyGPUCheckBox->setChecked(false);
        m_enableAll("CPU");
    }

}



void NCSClusterEditor::machineEnabled(QListWidgetItem* item)
{

    int machineIndex = item->data(Qt::UserRole).toInt();

    QMessageBox msgBox;
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);

    if (item->checkState() == Qt::Checked)
    {
        if (m_onlyGPUCheckBox->checkState() == Qt::Checked)
        {
            if (!m_cluster.machines[machineIndex].hasCUDA)
            {
                item->setCheckState(Qt::Unchecked);
                msgBox.setText("Machine cannot be enabled as it does not contain a GPU and 'Only GPU' check box is set. To allow this machine to be enabled, deselect 'Only GPU'.");
                msgBox.exec();
            }
        }

        else if (m_onlyCPUCheckBox->checkState() == Qt::Checked)
        {

            if (!m_cluster.machines[machineIndex].hasCPU)
            {
                item->setCheckState(Qt::Unchecked);

                msgBox.setText("Machine cannot be enabled as it does not contain a CPU and 'Only CPU' check box is set. To allow this machine to be enabled, deselect 'Only CPU'.");
                msgBox.exec();
            }
        }


    }
    else if (item->checkState() == Qt::Unchecked)
    {

        if (m_allGPUCheckBox->checkState() == Qt::Checked)
        {
            if (m_cluster.machines[machineIndex].hasCUDA)
            {
                item->setCheckState(Qt::Checked);

                msgBox.setText("Machine cannot be disabled as it contains a GPU and 'All GPU' check box is set. To allow this machine to be disabled, deselect 'All GPU'.");
                msgBox.exec();
            }
        }

        if (m_allCPUCheckBox->checkState() == Qt::Checked)
        {
            if (m_cluster.machines[machineIndex].hasCPU)
            {
                item->setCheckState(Qt::Checked);
                msgBox.setText("Machine cannot be disabled as it contains a CPU and 'All CPU' check box is set. To allow this machine to be disabled, deselect 'All CPU'.");
                msgBox.exec();
            }
        }
    }
    m_cluster.machines[machineIndex].enabled =item->checkState() & Qt::Checked;

}

void NCSClusterEditor::deviceSelected(QListWidgetItem* current, QListWidgetItem* prev)
{
    if (current == NULL)
        return;
    Device device = m_cluster.machines[m_machineSelectionIndex].devices[current->data(Qt::UserRole).toInt()];
    m_deviceBox->setTitle(device.type);
    if (m_deviceBox->layout())
	{
        QLayout * layout = m_deviceBox->layout();
		QLayoutItem* item;
        while ((item = layout->takeAt(0)) != NULL)
		{
			delete item->widget();
			delete item;
		}
        delete layout;
	}

    QGridLayout * layout = new QGridLayout();
    LabelSet typeInfo = m_infoParam("Type:", device.type);
	layout->addWidget(typeInfo.first, 0, 0);
    layout->addWidget(typeInfo.second, 0, 1);
    LabelSet powerInfo = m_infoParam("Power:", QString::number(device.power));
	layout->addWidget(powerInfo.first, 1, 0);
	layout->addWidget(powerInfo.second, 1, 1);
    m_deviceBox->setLayout(layout);
}

void NCSClusterEditor::deviceEnabled(QListWidgetItem* item)
{
    int deviceIndex = item->data(Qt::UserRole).toInt();

    QMessageBox msgBox;
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);

    if (item->checkState() == Qt::Checked)
    {
        if (m_onlyGPUCheckBox->checkState() == Qt::Checked && m_cluster.machines[m_machineSelectionIndex].devices[deviceIndex].type != "CUDA")
        {
            item->setCheckState(Qt::Unchecked);
            msgBox.setText("CPU Device cannot be enabled as 'Only GPU' check box is set. To allow this device to be enabled, deselect 'Only GPU'.");
            msgBox.exec();
        }

        else if (m_onlyCPUCheckBox->checkState() == Qt::Checked && m_cluster.machines[m_machineSelectionIndex].devices[deviceIndex].type != "CPU")
        {
            item->setCheckState(Qt::Unchecked);
            msgBox.setText("GPU Device cannot be enabled as 'Only CPU' check box is set. To allow this device to be enabled, deselect 'Only CPU'.");
            msgBox.exec();
        }
        if (item->checkState() == Qt::Checked)
        {
            m_cluster.machines[m_machineSelectionIndex].devices[deviceIndex].enabled = true;
            m_cluster.machines[m_machineSelectionIndex].enabled = true;
            m_machineList->item(m_cluster.machines[m_machineSelectionIndex].listIndex)->setCheckState(Qt::Checked);
        }
    }

    if (item->checkState() == Qt::Unchecked)
    {

        if (m_allGPUCheckBox->checkState() == Qt::Checked && m_cluster.machines[m_machineSelectionIndex].devices[deviceIndex].type == "CUDA")
        {
            item->setCheckState(Qt::Checked);
            msgBox.setText("GPU Device cannot be disabled as 'All GPU' check box is set. To allow this device to be disabled, deselect 'All GPU'.");
            msgBox.exec();
        }

        if (m_allCPUCheckBox->checkState() == Qt::Checked && m_cluster.machines[m_machineSelectionIndex].devices[deviceIndex].type == "CPU")
        {
            item->setCheckState(Qt::Checked);
            msgBox.setText("CPU Device cannot be disabled as 'All CPU' check box is set. To allow this device to be disabled, deselect 'All CPU'.");
            msgBox.exec();
        }

        if (item->checkState() == Qt::Unchecked)
        {
            m_cluster.machines[m_machineSelectionIndex].devices[deviceIndex].enabled = false;
            bool otherDevicesSelected = false;
            for (int i = 0; i < m_cluster.machines[m_machineSelectionIndex].devices.count(); i ++)
            {
               if (i != deviceIndex)
                   if (m_cluster.machines[m_machineSelectionIndex].devices[i].enabled)
                       otherDevicesSelected = true;
            }
            if (!otherDevicesSelected)
            {
                m_cluster.machines[m_machineSelectionIndex].enabled = false;
                m_machineList->item(m_cluster.machines[m_machineSelectionIndex].listIndex)->setCheckState(Qt::Unchecked);
            }
        }
    }

    m_cluster.machines[m_machineSelectionIndex].devices[deviceIndex].enabled =item->checkState() & Qt::Checked;
}


void NCSClusterEditor::saveHostFile( QString filename)
{

    QFile file(filename);
    file.open(QIODevice::ReadWrite);

    for (int i = 0; i < m_cluster.machines.count(); i ++)
    {
        Machine machine = m_cluster.machines[i];
        if (machine.enabled)
            file.write((machine.name  + " slots=1 \n").toAscii());
    }
    file.close();


}
void NCSClusterEditor::clear()
{
    m_cluster.machines.clear();
    m_machineList->clear();
    m_deviceList->clear();
}

void NCSClusterEditor::loadClusterFile(QString filename, bool append)
{

    if (!append)
        clear();
    int numMachines,numDevices;

    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream dataStream(&file);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    dataStream >> numMachines;

    bool duplicateMachineDetected = false;
    int id = 0;
    for (int i = 0;i < numMachines; i ++)
    {
        Machine machine;
        QByteArray utfString;
        dataStream >>  utfString;
        machine.name = QString::fromUtf8(utfString.data());
        dataStream >> machine.enabled;
        dataStream >> numDevices;
        machine.hasCPU = false;
        machine.hasCUDA = false;
        bool anyDeviceEnabled = false;

        for (int j = 0; j < numDevices; j ++)
        {
            Device device;

            QByteArray typeUtfString;
            dataStream >> typeUtfString;
            device.type = QString::fromUtf8(typeUtfString.data());

            if (device.type == "CUDA")
                dataStream >> device.number;
            dataStream >> device.power;
            dataStream >> device.enabled;

            if (device.type == "CUDA")
            {
                if (m_allGPUCheckBox->checkState() == Qt::Checked)
                    device.enabled = true;
                else if (m_onlyCPUCheckBox->checkState() == Qt::Checked)
                    device.enabled = false;

                machine.hasCUDA = true;

            }
            if (device.type == "CPU")
            {
                if (m_allCPUCheckBox->checkState() == Qt::Checked)
                    device.enabled = true;
                else if (m_onlyGPUCheckBox->checkState() == Qt::Checked)
                    device.enabled = false;

                machine.hasCPU = true;
            }
            if (device.enabled)
                anyDeviceEnabled = true;

            machine.devices.append(device);

        }

        machine.enabled = anyDeviceEnabled;

        if (m_cluster.machines.count(machine) == 0)
        {
            m_cluster.machines.append(machine);
            QListWidgetItem * item = new QListWidgetItem(machine.name);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(machine.enabled? Qt::Checked : Qt::Unchecked);
            item->setData(Qt::UserRole, id);
            m_machineList->addItem(item);
            m_cluster.machines[i].listIndex = m_machineList->count() -1;
            ++id;
        }
        else
            duplicateMachineDetected = true;
    }


    file.close();
    if (duplicateMachineDetected)
    {
        QMessageBox msgBox;
        msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
        msgBox.setText("Duplicate machine declaration(s) were detected. The duplicates have been removed.");
        msgBox.exec();
    }

}

int  NCSClusterEditor::enabledMachines()
{
    int count = 0;
    for (int i = 0; i < m_cluster.machines.count(); i ++)
    {
        Machine machine = m_cluster.machines[i];
        if (machine.enabled)
            count++;
    }
    return count;

}

void NCSClusterEditor::saveClusterFile(QString filename)
{

    // open file to house binary distribution file
    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    // setup data streaming object
    QDataStream dataStream(&file);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    dataStream << m_cluster.machines.count();
    for (int i = 0; i < m_cluster.machines.count(); i ++)
    {
        // for each machine write name, machine, and type and number of devices
        Machine machine = m_cluster.machines[i];
        QByteArray utfString = machine.name.toUtf8();
        dataStream << utfString;
        dataStream << machine.enabled;
        dataStream << machine.devices.count();


        for (int j = 0; j < machine.devices.count(); j ++)
        {
            // for each device write type
            Device device = machine.devices[j];

            QByteArray typeUtfString = device.type.toUtf8();
            dataStream <<  typeUtfString ;

            // for each device write number of devices
            if (device.type == "CUDA")
                dataStream << device.number;

            // for each device write power, whether it's enabled
            dataStream << device.power;
            dataStream << device.enabled;

        }
    }
    // close file
    file.close();
}

