#include "distributewidget.h"

DistributeWidget::DistributeWidget(QWidget *parent) :
    QWidget(parent)
{
    m_model = "";
    m_layout = new QVBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);

    m_widgetStack = new SlidingStackedWidget();

	m_deploymentType = LocalComputer;


	m_deploymentWidget = new DeploymentWidget();
	connect(m_deploymentWidget,SIGNAL(clusterPicked()),this,SLOT(m_clusterSelected()));
	connect(m_deploymentWidget,SIGNAL(localComputerPicked()),this,SLOT(m_localComputerSelected()));
    m_widgetStack->addWidget(m_deploymentWidget);


    m_connectionWidget = new ConnectionWidget();
    connect(m_connectionWidget,SIGNAL(connectionAttempted()),this,SLOT(m_connectionAttempted()));
    connect(m_connectionWidget,SIGNAL(connected(SSHSocket*)),this,SLOT(m_validateNCSCapability(SSHSocket*)));
    connect(m_connectionWidget,SIGNAL(connectionFailed()),this,SLOT(m_onConnectionFailed()));

    m_clusterEditor = new NCSClusterEditor();

    m_clusterWidget = new QWidget();
    m_clusterLayout = new QVBoxLayout();
    m_promptWidget = new QWidget();

    m_promptLayout = new QHBoxLayout();
    m_machinesLabel = new QLabel("Number of machines to distribute across:");
    m_promptLayout->addWidget(m_machinesLabel);

    m_numComputersEntryBox = new QSpinBox();
    m_numComputersEntryBox->setMinimum(1);
    m_numComputersEntryBox->setMaximum(1000);
    m_promptLayout->addWidget(m_numComputersEntryBox);


    m_detectDevicesButton = new QPushButton( "Detect Devices");
    connect(m_detectDevicesButton,SIGNAL(clicked()),this,SLOT(m_detectDevices()));

    m_promptLayout->addWidget(m_detectDevicesButton);
    m_promptWidget->setLayout(m_promptLayout);

    m_clusterLayout->addWidget(m_promptWidget);

    m_clusterEditor = new NCSClusterEditor();
    m_clusterEditor->setEnabled(false);
    m_clusterLayout->addWidget(m_clusterEditor);
    m_clusterWidget->setLayout(m_clusterLayout);

    m_widgetStack->addWidget(m_clusterWidget);
    m_widgetStack->setSpeed(300);

    m_layout->addWidget(m_widgetStack);

    m_buttonWidget = new QWidget();
    m_buttonLayout = new QHBoxLayout();

    m_previousButton = new ColorButton();
    m_previousButton->setColor(QColor(221,72,30),QColor(255,255,255));
    m_previousButton->hide();
    m_buttonLayout->addWidget(m_previousButton);


    m_validConnection = false;

    m_nextButton = new ColorButton();
	m_nextButton->setText("Connect >>>");
	connect(m_nextButton,SIGNAL(pressed()),this,SLOT(m_deploymentPicked()));
    m_nextButton->setColor(QColor(221,72,30),QColor(255,255,255));
    m_buttonLayout->addWidget(m_nextButton);
    m_buttonWidget->setLayout(m_buttonLayout);

    m_modelWidget = new ModelSelector();
    connect(m_modelWidget,SIGNAL(modelLoaded(QString)),this,SLOT(m_modelLoaded(QString)));
    m_widgetStack->addWidget(m_modelWidget);
    connect(m_widgetStack,SIGNAL(currentChanged(int)),this,SLOT(m_mainWidgetChanged(int)));
    m_stepProgress = new QProgressBar();
    m_stepProgress->setMinimum(0);
    m_stepProgress->setMaximum(100);
    m_stepProgress->setValue(0);
    m_stepProgress->hide();
    m_layout->addWidget(m_stepProgress);

    m_layout->addWidget(m_buttonWidget);
    m_clusterConnection = NULL;
    this->setLayout(m_layout);


}

void DistributeWidget::m_onConnectionFailed()
{

    QMessageBox msgBox;
    QPushButton *ok = msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
    msgBox.setText("Cannot connect to the remote cluster. Could be due to lack of internet connection or user account not being set up.");
    msgBox.exec();
    m_stepProgress->hide();
}

QString DistributeWidget:: m_executeLocalCommand(QString cmd)
{
#ifdef _WIN32
    FILE* pipe = _popen(cmd.toAscii().data(), "r");
#else
    FILE* pipe = popen(cmd.toAscii().data(), "r");
#endif
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe))
    {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }

#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif

    QString response = QString::fromStdString(result);
    response.replace("\n","");
    return response;
}

void DistributeWidget::m_localComputerSelected()
{

	m_nextButton->setText("Select Devices >>>" );
}

void DistributeWidget::m_clusterSelected()
{

	m_nextButton->setText("Connect >>>" );
}
void DistributeWidget::m_deploymentPicked()
{
	if (m_deploymentWidget->typeSelected() == Cluster)
	{
		if (m_widgetStack->widget(1) != m_connectionWidget)
			m_widgetStack->insertWidget(1,m_connectionWidget);
		m_widgetStack->slideInNext();

		if (!m_promptWidget->isVisible())
		{
            m_clusterLayout->insertWidget(0,m_promptWidget);
			m_promptWidget->show();
		}
		m_deploymentType = Cluster;
	}
	else
    {
        m_stepProgress->setValue(33);


#ifdef _WIN32
		QString mpiStatus =  m_executeLocalCommand("mpirun");

		QString brainslugStatus = m_executeLocalCommand("IF exist brainslug\ ( echo exists )" );

		if (mpiStatus == "")
			m_showErrorPopup("<font color='red'>Local computer does not have mpirun in the system path. Cannot continue until resolved. </font>");
#else
        QString mpiStatus =  m_executeLocalCommand("type -P mpirun &>/dev/null && echo 'exists'");
        QString brainslugStatus = m_executeLocalCommand("[ -d ./brainslug ] && echo 'exists' ");

		if (mpiStatus != "exists")
			m_showErrorPopup("<font color='red'>Local computer does not have mpirun in the system path. Cannot continue until resolved. </font>");
#endif


		else if (brainslugStatus != "exists")
			m_showErrorPopup("<font color='red'>Local computer does not have brainslug installed in supplied directory. Cannot continue until resolved. </font>");

		else
		{
			if (m_widgetStack->widget(1) == m_connectionWidget)
				m_widgetStack->removeWidget(m_connectionWidget);

			m_widgetStack->slideInNext();

			if (m_promptWidget->isVisible())
			{
				m_clusterLayout->removeWidget(m_promptWidget);
				m_promptWidget->hide();
            }
            m_clusterEditor->clear();
			m_clusterEditor->setEnabled(true);
			m_deploymentType = LocalComputer;

			if (m_validConnection)
                m_clusterConnection->disconnectFromHost();
            m_validConnection = false;

		}

	}
}



void DistributeWidget::m_connectionAttempted()
{

    m_stepProgress->show();
    m_stepProgress->setValue(0);
}

void DistributeWidget::m_detectDevices()
{

    m_stepProgress->show();
    m_stepProgress->setValue(0);
    m_clusterConnection->executeCommand("mpirun --np " + QString::number(m_numComputersEntryBox->value()) + " brainslug/applications/clusterSpecifier/clusterSpecifier ./cluster");

}

void DistributeWidget::m_showErrorPopup(QString text, QString detailedText)
{

    QMessageBox msgBox;
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
    msgBox.setText(text);
    if (detailedText != "")
        msgBox.setDetailedText(detailedText);
    msgBox.exec();

    m_stepProgress->hide();
    m_stepProgress->setValue(0);
}

void DistributeWidget::m_onCommandExecuted(QString command,QString result)
{

    if (command == "type -P mpirun &>/dev/null && echo 'exists'")
    {
        if (result == "exists")
        {

            m_stepProgress->setValue(66);
            m_clusterConnection->executeCommand("[ -d ./brainslug ] && echo 'exists' ");
        }
        else
        {
			m_showErrorPopup("<font color='red'>Unable to establish valid connection to cluster:  MPI not Installed</font>");
			m_nextButton->setEnabled(false);
            m_clusterConnection->disconnectFromHost();
        }

    }

    else if (command == "[ -d ./brainslug ] && echo 'exists' ")
    {
        if (result == "exists")
        {
            m_stepProgress->setValue(100);
            m_validConnection = true;
            m_nextButton->setEnabled(true);
        }
        else
        {
			m_showErrorPopup("<font color='red'>Unable to establish valid connection to cluster:  Brainslug not Installed</font>");
			m_nextButton->setEnabled(false);
            m_clusterConnection->disconnectFromHost();
        }

   }

    else if (command.contains("mpirun") && command.contains("hostfile"))
    {
        m_stepProgress->setValue(100);
        m_stepProgress->hide();

        finalized();
    }

    else if (command.contains("mpirun") )
    {

        m_stepProgress->setValue(50);
        m_clusterConnection->pullFile("./cluster","./cluster");
    }

}

void DistributeWidget::m_mainWidgetChanged(int index)
{

    QWidget * widget = m_widgetStack->widget(index);
    m_stepProgress->hide();
    m_stepProgress->setValue(0);
    if (widget == m_deploymentWidget)
    {
        m_previousButton->disconnect();
        m_previousButton->hide();

		m_nextButton->disconnect();
		m_nextButton->setEnabled(true);

		if (m_deploymentWidget->typeSelected() == Cluster)
			m_nextButton->setText("Connect >>>" );
		else
			m_nextButton->setText("Select Devices >>>" );

		connect(m_nextButton,SIGNAL(pressed()),this,SLOT(m_deploymentPicked()));

    }
    else if (widget == m_connectionWidget)
    {
        m_previousButton->disconnect();
        m_previousButton->show();
        m_previousButton->setText("<<< Choose Deployment Type");
        connect(m_previousButton,SIGNAL(pressed()),m_widgetStack,SLOT(slideInPrev()));

        m_nextButton->disconnect();

        m_nextButton->setText("Select Hardware >>>");
        m_nextButton->setEnabled(m_validConnection);
        m_nextButton->setColor(QColor(221,72,30),QColor(255,255,255));
        connect(m_nextButton,SIGNAL(pressed()),m_widgetStack,SLOT(slideInNext()));
    }
    else if (widget == m_clusterWidget)
    {
        m_previousButton->disconnect();
        m_previousButton->show();
        if ( m_deploymentType == LocalComputer)
            m_previousButton->setText("<<< Choose Deployment Type");
        else
            m_previousButton->setText("<<< Connect to Cluster");

        connect(m_previousButton,SIGNAL(pressed()),m_widgetStack,SLOT(slideInPrev()));

        m_nextButton->disconnect();
        m_nextButton->setText("Select Model >>>");
        m_nextButton->setEnabled(m_clusterEditor->isEnabled());
        m_nextButton->setColor(QColor(221,72,30),QColor(255,255,255));

        connect(m_nextButton,SIGNAL(pressed()),m_widgetStack,SLOT(slideInNext()));
    }
    else if (widget == m_modelWidget)
    {
        m_previousButton->disconnect();
        m_previousButton->show();
        m_previousButton->setText("<<< Select Hardware");

        connect(m_previousButton,SIGNAL(pressed()),m_widgetStack,SLOT(slideInPrev()));

        m_nextButton->disconnect();
        m_nextButton->setColor(QColor(0,255,0),QColor(255,255,255));
        m_nextButton->setText("Distribute Simulation >>>");
        m_nextButton->setEnabled(m_model != "");

        connect(m_nextButton,SIGNAL(pressed()),this,SLOT(m_distribute()));
    }
}


void DistributeWidget::m_validateNCSCapability(SSHSocket * connection)
{
    m_clusterEditor->clear();
    m_stepProgress->setValue(33);

    m_clusterEditor->setEnabled(false);

    if (m_clusterConnection != NULL)
        m_clusterConnection->disconnect(this);

    m_clusterConnection = connection;

    connect(m_clusterConnection,SIGNAL(error(SSHSocket::SSHSocketError)),this,SLOT(m_error(SSHSocket::SSHSocketError)));
    connect(m_clusterConnection,SIGNAL(commandExecuted(QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString)));
    connect(m_clusterConnection,SIGNAL(pullSuccessfull(QString,QString)),this,SLOT(m_onFilePulled(QString,QString)));
    connect(m_clusterConnection,SIGNAL(pushSuccessfull(QString,QString)),this,SLOT(m_onFilePushed(QString,QString)));

    m_clusterConnection->executeCommand("type -P mpirun &>/dev/null && echo 'exists'");
}


void  DistributeWidget::m_error(SSHSocket::SSHSocketError err)
{
    m_validConnection = false;
}


void DistributeWidget::m_modelLoaded(QString model)
{
    m_model = model;
    m_nextButton->setEnabled(true);
}

void DistributeWidget::m_onFilePushed(QString localFile,QString remoteFile)
{

    if (localFile == "./cluster")
    {
        // push hostfile

        m_stepProgress->setValue(33);

        m_clusterEditor->saveHostFile("./hostfile");
        m_clusterConnection->pushFile("./hostfile","./hostfile");
    }
    else if (localFile == "./hostfile")
    {

        m_stepProgress->setValue(66);
        m_clusterConnection->executeCommand("mpirun --np " + QString::number(m_clusterEditor->enabledMachines()) + "--hostfile ./hostfile.txt ./brainslug/applications/clusterSpecifier/clusterSpecifier ./cluster");


    }


}

void DistributeWidget::m_onFilePulled(QString localFile,QString remoteFile)
{

    if (localFile == "./cluster")
    {
        m_stepProgress->setValue(100);
        m_clusterEditor->loadClusterFile("./cluster");
        m_nextButton->setEnabled(true);
        m_clusterEditor->setEnabled(true);
    }

}

void  DistributeWidget::m_distribute()
{
    QMessageBox msgBox;
    QPushButton *cont = msgBox.addButton(tr("Continue"), QMessageBox::ActionRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);
    msgBox.setText("Do you want to continue? Once you do you cannot edit your distribution settings without stopping the simulation.");
    msgBox.exec();

    if (msgBox.clickedButton() == cont)
    {

        // push model.


        // push cluster desc.

        m_stepProgress->show();
        m_stepProgress->setValue(0);


		if (m_deploymentType == Cluster)
        {
            m_clusterEditor->saveClusterFile("./cluster");
			m_clusterConnection->pushFile("./cluster","./cluster");
		}
    }
}
