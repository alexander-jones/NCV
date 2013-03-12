
#include "ncsclustereditor.h"
#include <QByteArray>

NCSClusterEditor::NCSClusterEditor()
    :QWidget()
{

    m_layout = new QVBoxLayout();



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
    connect(m_machineList,
	        SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), 
			this, 
			SLOT(machineSelected(QListWidgetItem*, QListWidgetItem*)));
    connect(m_machineList,
	        SIGNAL(itemChanged(QListWidgetItem*)),
			this,
			SLOT(machineEnabled(QListWidgetItem*)));
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
    connect(m_deviceList,
	        SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
			this,
			SLOT(deviceSelected(QListWidgetItem*, QListWidgetItem*)));
    connect(m_deviceList,
	        SIGNAL(itemChanged(QListWidgetItem*)),
			this,
			SLOT(deviceEnabled(QListWidgetItem*)));
	
    m_deviceBox = new QGroupBox(tr("Device Info"));
    deviceLayout->addWidget(m_deviceBox);


    m_machineWidgdet->setLayout(m_mainLayout);
    m_layout->addWidget(m_machineWidgdet);

    this->setLayout(m_layout);
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
    Machine machine = m_cluster.machines[current->data(Qt::UserRole).toInt()];
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
    m_machineSelection = machine;
}

void NCSClusterEditor::machineEnabled(QListWidgetItem* item)
{
    m_cluster.machines[item->data(Qt::UserRole).toInt()].enabled =item->checkState() & Qt::Checked;
}

void NCSClusterEditor::deviceSelected(QListWidgetItem* current, QListWidgetItem* prev)
{
    Device device = m_machineSelection.devices[current->data(Qt::UserRole).toInt()];
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
    m_machineSelection.devices[item->data(Qt::UserRole).toInt()].enabled =item->checkState() & Qt::Checked;
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

void NCSClusterEditor::loadClusterFile(QString filename)
{

    clear();
    unsigned int numMachines,numDevices;

    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream dataStream(&file);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    dataStream >> numMachines;

    for (int i = 0;i < numMachines; i ++)
    {
        Machine machine;
        machine.devices.clear();
        QByteArray utfString;
        dataStream >>  utfString;
        machine.name = QString::fromUtf8(utfString.data());
        dataStream >> machine.enabled;
        dataStream >> numDevices;

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
            machine.devices.append(device);
        }
        m_cluster.machines.append(machine);
    }


    file.close();

    m_deviceList->clear();
    m_machineList->clear();
    int id = 0;
    for (int i = 0; i < m_cluster.machines.count(); i ++)
    {
        Machine machine = m_cluster.machines[i];
        QListWidgetItem * item = new QListWidgetItem(machine.name);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(machine.enabled? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole, id);
        m_machineList->addItem(item);
        ++id;
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
    file.open(QIODevice::ReadOnly);

    // setup data streaming object
    QDataStream dataStream(&file);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    for (int i = 0; i < m_cluster.machines.count(); i ++)
    {
        // for each machine write name, machine, and type and number of devices
        Machine machine = m_cluster.machines[i];
        int numDevices = machine.devices.size();

        QByteArray utfString = machine.name.toUtf8();
        dataStream << utfString;
        dataStream << machine.enabled;
        dataStream << numDevices;

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

