#include "ncsconnectionwizardpage.h"


NCSConnectionWizardPage::NCSConnectionWizardPage(QWidget *parent) :
    QWizardPage(parent)
{
    this->setTitle("Connect to NCS");
    this->setSubTitle("Establish a connection to a built installation of NCS by specifying a path on a remote or local host. ");
    m_complete = false;
    m_remoteCommandBridge = NULL;

    m_socket = NULL;
    m_localCommandBridge = NULL;

    m_layout = new QVBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);

    m_connectionGroupVector = new QGroupVector();
    connect(m_connectionGroupVector,SIGNAL(groupChecked(QLayout *)),this,SLOT(m_connectionGroupChecked(QLayout *)));
    m_connectionGroupVector->setUncheckedBehavior(QGroupVector::DisableUnchecked);
    m_connectionGroupVector->setCheckedBehavior(QGroupVector::SingleChecked);

    m_localLayout = new QHBoxLayout();
    m_localNCSDirectoryLabel = new QLabel("NCS 6 Directory:");
    m_localLayout->addWidget(m_localNCSDirectoryLabel);
    m_ncsDirectoryEdit = new QLineEdit();
    connect(m_ncsDirectoryEdit,SIGNAL(textChanged(QString)),this,SLOT(m_localNCSDirectoryChanged(QString)));
    m_localLayout->addWidget(m_ncsDirectoryEdit);
    m_ncsBrowseButton = new QPushButton("Browse");
    connect(m_ncsBrowseButton,SIGNAL(clicked()),this,SLOT(m_browseForNCS()));
    m_localLayout->addWidget(m_ncsBrowseButton);
    m_localValidateButton = new QPushButton("Validate");
    m_localValidateButton->setEnabled(false);
    connect(m_localValidateButton,SIGNAL(clicked()),this,SLOT(m_validateLocalConnection()));
    m_localLayout->addWidget(m_localValidateButton);

    m_connectionGroupVector->addGroup(m_localLayout,"Use Local Machine as Host");
    m_connectionGroupVector->setGroupChecked(m_localLayout,true);

    m_remoteLayout = new QVBoxLayout();
    m_remoteConnectionWidget = new RemoteConnectionWidget();
    m_remoteConnectionWidget->setEnabled(false);
    connect(m_remoteConnectionWidget,SIGNAL(connected(QSshSocket*)),this,SLOT(m_remoteConnectionEstablished(QSshSocket*)));
    connect(m_remoteConnectionWidget,SIGNAL(connectionFailed()),this,SLOT(m_remoteConnectionFailed()));
    connect(m_remoteConnectionWidget,SIGNAL(connectionAttempted()),this,SLOT(m_clearRemoteContext()));
    m_remoteLayout->addWidget(m_remoteConnectionWidget);

    m_remoteNCSDirectoryVector = new QWidgetVector();
    m_remoteNCSDirectoryVector->setEnabled(false);
    m_remoteNCSDirectoryLabel = new QLabel("NCS 6 Directory:");
    m_remoteNCSDirectoryVector->addWidget(m_remoteNCSDirectoryLabel);
    m_remoteNCSDirectoryEdit = new QLineEdit();
    m_remoteNCSDirectoryVector->addWidget(m_remoteNCSDirectoryEdit);
    m_remoteValidateButton = new QPushButton("Validate");
    connect(m_remoteValidateButton,SIGNAL(clicked()),this,SLOT(m_validateRemoteConnection()));
    m_remoteNCSDirectoryVector->addWidget(m_remoteValidateButton);

    m_remoteLayout->addWidget(m_remoteNCSDirectoryVector);
    m_connectionGroupVector->addGroup(m_remoteLayout,"Use Remote Machine as Host");
    m_layout->addWidget(m_connectionGroupVector);

    this->setPixmap(QWizard::LogoPixmap,QPixmap(":/media/ncvWizardLogo.png"));
    this->setPixmap(QWizard::WatermarkPixmap,QPixmap(":/media/ncsBannerVertical.png"));
    this->setPixmap(QWizard::BackgroundPixmap,QPixmap(":/media/ncsBannerVertical.png"));
    this->setLayout(m_layout);
}
void NCSConnectionWizardPage::loadProject(QString projectDir)
{
    m_projectPath = projectDir;
    m_localCommandBridge->initialize(m_projectPath);
    m_ncsDirectoryEdit->setText(m_projectPath);
}

void NCSConnectionWizardPage::m_localNCSDirectoryChanged(QString newText)
{
    if (newText != "" && QDir(newText).exists())
        m_localValidateButton->setEnabled(true);
    else
        m_localValidateButton->setEnabled(false);
}

void NCSConnectionWizardPage::m_connectionGroupChecked(QLayout * layout)
{
    if (layout == m_remoteLayout)
    {
        if(m_socket == NULL)
            m_remoteNCSDirectoryVector->setEnabled(false);
        else
            m_remoteNCSDirectoryVector->setEnabled(true);
    }
}


void NCSConnectionWizardPage::m_browseForNCS()
{
    QString dirPath = QFileDialog::getExistingDirectory(this,"Find NCS 6 Directory",m_ncsDirectoryEdit->text());
    if (dirPath != "")
        m_ncsDirectoryEdit->setText(dirPath);


}

void NCSConnectionWizardPage::m_clearRemoteContext( )
{
    if (m_socket != NULL)
    {
        m_socket->disconnect();
        m_socket->deleteLater();
        if (m_remoteCommandBridge != NULL)
        {
            m_remoteCommandBridge->disconnect();
            m_socket->deleteLater();
        }
    }
    m_socket = NULL;
    m_remoteCommandBridge = NULL;
    m_remoteNCSDirectoryEdit->clear();
    m_remoteNCSDirectoryVector->setEnabled(false);
}
void NCSConnectionWizardPage::m_remoteConnectionFailed()
{
    QMessageBox msgBox;
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
    msgBox.setText("Cannot connect to the remote host. Could be due to lack of internet connection or user account not being set up.");
    msgBox.exec();
}


void NCSConnectionWizardPage::m_remoteConnectionError(QSshSocket::SshError err)
{

    QMessageBox msgBox;
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
    msgBox.setText("An unexpected error has occured over the remote socket. This may be due to loss of internet connection.");
    msgBox.exec();
}

void NCSConnectionWizardPage::m_remoteConnectionEstablished(QSshSocket * socket)
{
    m_complete = false;
    completeChanged();

    if (m_socket != NULL)
    {
        m_socket->disconnect();
        m_socket->deleteLater();
        if (m_remoteCommandBridge != NULL)
        {
            m_remoteCommandBridge->disconnect();
            m_remoteCommandBridge->deleteLater();
        }
    }
    m_socket = socket;
    m_remoteCommandBridge = new NCSRemoteCommandBridge(this);
    connect(m_remoteCommandBridge,SIGNAL(validationError(NCSCommandBridge::ValidationError)),this,SLOT(m_connectionInvalidated(NCSCommandBridge::ValidationError)));
    connect(m_remoteCommandBridge,SIGNAL(validated()),this,SLOT(m_remoteConnectionValidated()));
    m_remoteCommandBridge->initialize(m_projectPath,socket);
    m_remoteNCSDirectoryVector->setEnabled(true);
}
bool NCSConnectionWizardPage::validatePage()
{
    if (!m_complete)
    {
        QMessageBox msgBox;
        msgBox.setText("An NCS installation has not been validated. Enter a local or remote NCS installation path then press 'Validate' to continue." );
        msgBox.addButton("Ok", QMessageBox::ActionRole);
        msgBox.exec();
    }
    return m_complete;
}

void NCSConnectionWizardPage::m_validateRemoteConnection()
{
    m_complete = false;
    m_remoteCommandBridge->validate(m_remoteNCSDirectoryEdit->text());

}
void NCSConnectionWizardPage::m_remoteConnectionValidated()
{
    m_complete = true;
    bridgeEstablished(m_remoteCommandBridge);
    m_remoteCommandBridge = NULL;
}
void NCSConnectionWizardPage::m_validateLocalConnection()
{
    m_complete = false;
    m_localCommandBridge = new NCSLocalCommandBridge(this);
    connect(m_localCommandBridge,SIGNAL(validationError(NCSCommandBridge::ValidationError)),this,SLOT(m_connectionInvalidated(NCSCommandBridge::ValidationError)));
    connect(m_localCommandBridge,SIGNAL(validated()),this,SLOT(m_localConnectionValidated()));
    m_localCommandBridge->validate(m_ncsDirectoryEdit->text());
}

void NCSConnectionWizardPage::m_localConnectionValidated()
{
    m_socket = NULL;
    m_remoteNCSDirectoryEdit->clear();
    m_remoteNCSDirectoryVector->setEnabled(false);
    m_complete = true;
    bridgeEstablished(m_localCommandBridge);
    m_localCommandBridge = NULL;
}
void NCSConnectionWizardPage::m_connectionInvalidated(NCSCommandBridge::ValidationError err)
{
    QMessageBox msgBox;
    msgBox.setText("NCS installation could not be validated:");
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);

    if (err == NCSCommandBridge::MissingMPI)
        msgBox.setDetailedText("MPI could not be found in the host's system path.");
    else if (err == NCSCommandBridge::MissingRootDirectory)
        msgBox.setDetailedText("The path specified could not be found.");
    else if (err == NCSCommandBridge::MissingBuildDirectory)
        msgBox.setDetailedText("The path specified does not contain a build directory.");
    else if (err == NCSCommandBridge::MissingApplicationDirectory)
        msgBox.setDetailedText("The build directory at the path specified does not contain applications.");
    else if (err == NCSCommandBridge::MissingPluginDirectory)
        msgBox.setDetailedText("The build directory at the path specified does not contain plugins.");

    msgBox.exec();
}

void NCSConnectionWizardPage::initialize()
{
}

void NCSConnectionWizardPage::cleanup()
{
}
