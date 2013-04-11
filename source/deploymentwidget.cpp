#include "deploymentwidget.h"

QString localComputerDesc = QString("Distribute a simulation across the hardware available on this computer.") +
	QString(" Best suited for small scale simulations; with mid-range laptops capable of handling models of around 1,000 neurons and 1,000 synapses, ") +
	QString("and desktops capable of handling models of around 10,000 neurons and 10,000 synapses. However, this deployment specification offers") +
	QString("less data latency as no networking communication is needed.");


QString clusterDesc = QString("Distribute a simulation across the hardware available on a remote cluster.") +
	QString(" Best suited for large scale simulations; with cpu-dense clusters capable of handling models around 100,000 neurons ") +
	QString("and 100,000 synapses, and gpu-dense clusters capable of handling models of around 1,000,000 neurons and 1,000,000  ") +
	QString("synapses . However, this deployment specification requires network communication and thus implies more data latency ");


DeploymentWidget::DeploymentWidget(QWidget *parent)
	: QWidget(parent)
{
	m_deploymentLayout = new QVBoxLayout();

	m_localComputerDescriptionWidget = new QGroupBox();
	m_localComputerDescriptionWidget->setTitle("Local Computer");
	connect(m_localComputerDescriptionWidget,SIGNAL(clicked(bool)),this,SLOT(m_onLocalComputerChecked(bool)));
	m_localComputerDescriptionWidget->setCheckable(true);
	m_localComputerDescriptionWidget->setChecked(false);
	m_localComputerDescriptionLayout = new QHBoxLayout();
	m_localComputerDescriptionImage = new ImageContainer();
	m_localComputerDescriptionImage->setPixmap(QPixmap(":/assets/localComputer.png"));
	m_localComputerDescriptionLayout->addWidget(m_localComputerDescriptionImage);
	m_localComputerDescriptionLabel = new QWidget();
	m_localComputerDescriptionLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	m_localComputerDescriptionLayout->addWidget(m_localComputerDescriptionLabel);
	m_localComputerDescriptionLayout->setAlignment(Qt::AlignLeft);
	m_localComputerDescriptionWidget->setLayout(m_localComputerDescriptionLayout);

	m_deploymentLayout->addWidget(m_localComputerDescriptionWidget);

	m_clusterDescriptionWidget = new QGroupBox();
	m_clusterDescriptionWidget->setTitle("Cluster");
	m_clusterDescriptionWidget->setCheckable(true);
	connect(m_clusterDescriptionWidget,SIGNAL(clicked(bool)),this,SLOT(m_onClusterChecked(bool)));
	m_clusterDescriptionLayout = new QHBoxLayout();
	m_clusterDescriptionImage = new ImageContainer();
	m_clusterDescriptionImage->setPixmap(QPixmap(":/assets/cluster.png"));
	m_clusterDescriptionLayout->addWidget(m_clusterDescriptionImage);
	m_clusterDescriptionLabel = new QWidget();
	m_clusterDescriptionLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	m_clusterDescriptionLayout->addWidget(m_clusterDescriptionLabel);
	m_clusterDescriptionLayout->setAlignment(Qt::AlignLeft);
	m_clusterDescriptionWidget->setLayout(m_clusterDescriptionLayout);

	m_deploymentLayout->addWidget(m_clusterDescriptionWidget);

	this->setLayout(m_deploymentLayout);
	m_typeSelected = Cluster;
}



void DeploymentWidget::paintEvent(QPaintEvent * e)
{
	QWidget::paintEvent(e);
	QPainter painter(this);
	painter.translate(QPoint(m_localComputerDescriptionImage->width() + 25,m_localComputerDescriptionImage->height()/2));
	QTextDocument localComputerDescriptionText;
	if (!m_localComputerDescriptionWidget->isChecked())
		localComputerDescriptionText.setHtml("<font color='grey'>" +  localComputerDesc + "</font>");
	else
		localComputerDescriptionText.setPlainText(localComputerDesc);
	localComputerDescriptionText.setTextWidth(m_localComputerDescriptionLabel->size().width());
	localComputerDescriptionText.setDefaultStyleSheet("* { color: #FFEFEF }");
	localComputerDescriptionText.drawContents(&painter,m_localComputerDescriptionLabel->rect());


	painter.translate(QPoint(25,m_clusterDescriptionImage->height()));
	QTextDocument clusterDescriptionText;
	if (!m_clusterDescriptionWidget->isChecked())
		clusterDescriptionText.setHtml("<font color='grey'>" +  clusterDesc + "</font>");
	else
		clusterDescriptionText.setPlainText(clusterDesc);
	clusterDescriptionText.setTextWidth(m_clusterDescriptionLabel->size().width());
	clusterDescriptionText.setDefaultStyleSheet("* { color: #FFEFEF }");
	clusterDescriptionText.drawContents(&painter,m_clusterDescriptionLabel->rect());
}



void DeploymentWidget::m_onLocalComputerChecked(bool checked)
{ 
	if (!checked)
		m_localComputerDescriptionWidget->setChecked(true);
	else
	{
		m_clusterDescriptionWidget->setChecked(false);
		m_typeSelected = LocalComputer;
		localComputerPicked();
	}
}



void DeploymentWidget::m_onClusterChecked(bool checked)
{
	if (!checked)
		m_clusterDescriptionWidget->setChecked(true);
	else
	{
		m_localComputerDescriptionWidget->setChecked(false);
		m_typeSelected = Cluster;
		clusterPicked();
	}
}



DeploymentType DeploymentWidget::typeSelected()
{
	return m_typeSelected;
}



DeploymentWidget::~DeploymentWidget()
{

}
