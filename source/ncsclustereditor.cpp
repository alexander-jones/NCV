
#include "ncsclustereditor.h"

NCSClusterEditor::LabelSet NCSClusterEditor::infoParam(const std::string& key, const std::string& value)
{
    auto keyLabel = new QLabel(tr(key.c_str()));
	keyLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    auto valueLabel = new QLabel(tr(value.c_str()));
	valueLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	return LabelSet(keyLabel, valueLabel);
}

NCSClusterEditor::NCSClusterEditor()
    :QWidget(),m_cluster(nullptr)
{
    m_openAction = new QAction(tr("&Open"), this);
    m_saveAction = new QAction(tr("&Save"), this);
    m_hostfileAction = new QAction(tr("Generate MPI &Hostfile"), this);

    connect(m_openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(m_saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(m_hostfileAction, SIGNAL(triggered()), this, SLOT(hostfile()));


    m_layout = new QVBoxLayout();

    /*m_loginWidget = new QwwLoginBox();
    m_loginWidget->setFields( QwwLoginBox::HostField | QwwLoginBox::PortField | QwwLoginBox::UserField	| QwwLoginBox::PasswordField
                             |QwwLoginBox::RepeatPasswordField| QwwLoginBox::RememberPasswordField |QwwLoginBox::ProxyField);
    m_layout->addWidget(m_loginWidget);*/

    m_toolbar = new QToolBar();
    m_toolbar->addAction(m_openAction);
    m_toolbar->addAction(m_saveAction);
    m_toolbar->addAction(m_hostfileAction);
    m_toolbar->addSeparator();
    m_layout->addWidget(m_toolbar);


    m_machineWidgdet = new QWidget();
    m_mainLayout = new QHBoxLayout();

	
	auto machineLayout = new QVBoxLayout();
    m_mainLayout->addLayout(machineLayout);

	auto machineListBox = new QGroupBox(tr("Machines"));
    m_machineList = new QListWidget();
	auto machineListLayout = new QVBoxLayout();
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

	auto deviceLayout = new QVBoxLayout();
    m_mainLayout->addLayout(deviceLayout);

    m_deviceListBox = new QGroupBox(tr("Devices"));
    m_deviceList = new QListWidget();
	auto deviceListLayout = new QVBoxLayout();
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

void NCSClusterEditor::open()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open File"));
	if (filename == "")
		return;
	slug::spec::Cluster* cluster =
		slug::spec::Cluster::build(filename.toStdString(), false);
    m_updateCluster(cluster);
}

void NCSClusterEditor::save()
{
    if (nullptr == m_cluster)
		return;
	QString filename = QFileDialog::getSaveFileName(this, tr("Save File"));
	if (filename == "")
		return;
    m_cluster->write(filename.toStdString());
}

void NCSClusterEditor::hostfile()
{
    if (nullptr == m_cluster)
		return;
	QString filename = QFileDialog::getSaveFileName(this, tr("Host File"));
	if (filename == "")
		return;
	std::ofstream os(filename.toStdString());
	if (os.fail())
		return;
    for (auto machine : m_cluster->machines)
	{
		os << machine->name << " slots=1" << std::endl;
	}
	os.close();
}

void NCSClusterEditor::machineSelected(QListWidgetItem* current, QListWidgetItem* prev)
{
    auto machine = m_cluster->machines[current->data(Qt::UserRole).toInt()];
    m_machineBox->setTitle(tr(machine->name.c_str()));
    if (m_machineBox->layout())
	{
        auto layout = m_machineBox->layout();
		QLayoutItem* item;
		while ((item = layout->takeAt(0)) != nullptr)
		{
			delete item->widget();
			delete item;
		}
		delete layout;
	}
	auto layout = new QGridLayout();
	auto hostInfo = infoParam("Hostname:", machine->name);
	layout->addWidget(hostInfo.first, 0, 0);
	layout->addWidget(hostInfo.second, 0, 1);
    m_machineBox->setLayout(layout);
    m_deviceList->clear();

	int id = 0;
	for (auto device : machine->devices)
	{
        auto item = new QListWidgetItem(tr(device->type().c_str()));
		item->setData(Qt::UserRole, id);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(device->enabled? Qt::Checked : Qt::Unchecked);
        m_deviceList->addItem(item);
		++id;
	}
    m_machine = machine;
}

void NCSClusterEditor::machineEnabled(QListWidgetItem* item)
{
    m_cluster->machines[item->data(Qt::UserRole).toInt()]->enabled =
		item->checkState() & Qt::Checked;
}

void NCSClusterEditor::deviceSelected(QListWidgetItem* current, QListWidgetItem* prev)
{
    auto device = m_machine->devices[current->data(Qt::UserRole).toInt()];
    m_deviceBox->setTitle(tr(device->type().c_str()));
    if (m_deviceBox->layout())
	{
        auto layout = m_deviceBox->layout();
		QLayoutItem* item;
		while ((item = layout->takeAt(0)) != nullptr)
		{
			delete item->widget();
			delete item;
		}
		delete layout;
	}

	auto layout = new QGridLayout();
	auto typeInfo = infoParam("Type:", device->type());
	layout->addWidget(typeInfo.first, 0, 0);
	layout->addWidget(typeInfo.second, 0, 1);
	auto powerInfo = infoParam("Power:", QString::number(device->power).toStdString());
	layout->addWidget(powerInfo.first, 1, 0);
	layout->addWidget(powerInfo.second, 1, 1);
    m_deviceBox->setLayout(layout);
}

void NCSClusterEditor::deviceEnabled(QListWidgetItem* item)
{
    m_machine->devices[item->data(Qt::UserRole).toInt()]->enabled =
		item->checkState() & Qt::Checked;
}

void NCSClusterEditor::m_updateCluster(slug::spec::Cluster* cluster)
{
    if (nullptr != m_cluster)
	{
        delete m_cluster;
	}
    m_cluster = cluster;
    m_deviceList->clear();
    m_machineList->clear();
	int id = 0;
	for (auto machine : cluster->machines)
	{
        auto item = new QListWidgetItem(tr(machine->name.c_str()));
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(machine->enabled? Qt::Checked : Qt::Unchecked);
		item->setData(Qt::UserRole, id);
        m_machineList->addItem(item);
		++id;
	}
}
