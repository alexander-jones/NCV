#include "ncsinstallationdialog.h"

NCSInstallationDialog::NCSInstallationDialog(QWidget *parent) :
    NCSDialog(parent)
{
    m_installationFilename = "./user/installations.xml";
    m_local = true;
    m_socket = NULL;
    m_remoteCommandBridge = NULL;
    m_localCommandBridge = NULL;

    m_mainVector = new QWidgetVector();
    m_mainVector->setDirection(QWidgetVector::TopToBottom);
    m_groupBoxVector = new QGroupVector();
    m_groupBoxVector->setCheckedBehavior(QGroupVector::SingleChecked);
    m_groupBoxVector->setUncheckedBehavior(QGroupVector::DisableUnchecked);
    connect(m_groupBoxVector,SIGNAL(groupChecked(QLayout*)),this,SLOT(m_installationTypeSwitched(QLayout*)));
    m_buildLocalWidget();
    m_buildRemoteWidget();
    m_groupBoxVector->setGroupChecked(m_localLayout,true);
    m_mainVector->addWidget(m_groupBoxVector);


    QFrame * sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    m_mainVector->addWidget(sep);


    m_bottomVector = new QWidgetVector();
    m_bottomVector->setEnabled(false);

    m_saveCheckBox = new QCheckBox("Save this installation");
    connect(m_saveCheckBox,SIGNAL(clicked(bool)),this,SLOT(m_enableDefaultCheckBox(bool)));
    m_bottomVector->addWidget(m_saveCheckBox);

    m_defaultCheckBox = new QCheckBox("Use this installation by default");
    connect(m_defaultCheckBox,SIGNAL(clicked(bool)),this,SLOT(m_defaultChecked(bool)));
    m_defaultCheckBox->setEnabled(false);
    m_bottomVector->addWidget(m_defaultCheckBox);


    m_validateButton =  new QPushButton("Connect");
    connect(m_validateButton,SIGNAL(clicked()),this,SLOT(m_validateLocalConnection()));
    m_bottomVector->addWidget(m_validateButton);
    m_mainVector->addWidget(m_bottomVector);

    this->resize(715,638);
    this->setTitle("<font color = '#dd4814'><b>Connect to a installation of NCS 6:</b></font>");
    this->setWindowTitle("Connect to NCS");
    this->setDescription("Specify a remote or local installation path to NCS 6.");
    this->setWidget(m_mainVector);

    m_loadSavedInstallations();

}
void NCSInstallationDialog::show()
{
    m_localPathEdit->clear();
    m_remoteHostEdit->clear();
    m_remoteUserEdit->clear();
    m_remotePasswordEdit->clear();
    m_remotePathEdit->clear();
    m_localContexts.clear();
    m_remoteContexts.clear();
    m_savedRemotesWidget->clear();
    m_savedLocalsWidget->clear();
    m_savedInstallationsDocument.clear();
    m_loadSavedInstallations();
    NCSDialog::show();
}

void NCSInstallationDialog::m_loadSavedInstallations()
{
    QFile file(m_installationFilename);
    if (file.exists())
    {
        file.open( QIODevice::ReadOnly | QIODevice::Text );
        m_savedInstallationsDocument.setContent(&file);
        file.close();
        m_rootDocumentElement = m_savedInstallationsDocument.firstChildElement("ncs-installations");


        // check to see if saved installation document contains this installation already.
        QDomNodeList installationNodes = m_rootDocumentElement.childNodes();
        for (int i =0; i< installationNodes.count(); i ++)
        {
            QDomElement installationElement = installationNodes.at(i).toElement();
            InstallationContext installationContext;
            installationContext.host = installationElement.tagName();
            installationContext.path =  installationElement.attribute("path");
            if (installationContext.host == "localhost")
            {
                m_savedLocalsWidget->addItem(installationContext.path + " @ " + installationContext.host );
                m_localContexts.append(installationContext);
            }
            else
            {
                installationContext.user = installationElement.attribute("user");
                QByteArray encryptPass = QByteArray::fromHex(installationElement.attribute("password").toAscii());
                SimpleCrypt crypto(Q_UINT64_C(0x0c2ad4a4acb9f023)); //same random number: key should match encryption key
                crypto.setIntegrityProtectionMode(SimpleCrypt::ProtectionHash);  //properly protect the integrity of the data
                installationContext.password = crypto.decryptToByteArray(encryptPass);

                m_savedRemotesWidget->addItem(installationContext.path + " @ " + installationContext.host );
                m_remoteContexts.append(installationContext);
            }
            if (!hasDefaultInstallation())
                m_defaultContext = installationContext;
        }
    }
    else
    {
        m_rootDocumentElement = m_savedInstallationsDocument.createElement("ncs-installations");
        m_rootDocumentElement.setAttribute("version", 1.0);
        m_savedInstallationsDocument.appendChild(m_rootDocumentElement);
    }

}

void NCSInstallationDialog::m_buildLocalWidget()
{

    m_localLayout = new QHBoxLayout();

    m_localEntryVector = new QWidgetVector();
    m_localEntryVector->setDirection(QWidgetVector::TopToBottom);

    m_localPathVector = new QWidgetVector();
    m_localPathVector->addWidget(new QLabel("Path:"));
    m_localPathEdit = new QLineEdit();
    connect(m_localPathEdit,SIGNAL(textChanged(QString)),this,SLOT(m_onTextEditChanged()));
    m_localPathVector->addWidget(m_localPathEdit);
    m_localEntryVector->addWidget(m_localPathVector);

    m_localPathBrowseButton =  new QPushButton("Browse");
    connect(m_localPathBrowseButton,SIGNAL(clicked()),this,SLOT(m_browseForLocalPath()));
    m_localEntryVector->addWidget(m_localPathBrowseButton);

    m_localLayout->addWidget(m_localEntryVector);

    m_savedLocalsVector = new QWidgetVector();
    m_savedLocalsVector->setDirection(QWidgetVector::TopToBottom);
    m_savedLocalsVector->addWidget(new QLabel("Saved Installations:"));
    m_savedLocalsWidget = new QListWidget();
    connect(m_savedLocalsWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(m_loadLocalInstallation(QModelIndex)));
    m_savedLocalsVector->addWidget(m_savedLocalsWidget);

    m_localLayout->addWidget(m_savedLocalsVector);

    m_groupBoxVector->addGroup(m_localLayout,"Connect to Local Installation of NCS");
}

void NCSInstallationDialog::m_buildRemoteWidget()
{

    m_remoteLayout = new QHBoxLayout();

    m_remoteEntryVector = new QWidgetVector();
    m_remoteEntryVector->setDirection(QWidgetVector::TopToBottom);
    m_remoteEntryWidget = new QWidget();
    m_remoteEntryLayout = new QFormLayout();

    m_remoteHostEdit = new QLineEdit();
    connect(m_remoteHostEdit,SIGNAL(textChanged(QString)),this,SLOT(m_onTextEditChanged()));
    m_remoteEntryLayout->addRow("Host:",m_remoteHostEdit);

    m_remoteUserEdit = new QLineEdit();
    connect(m_remoteUserEdit,SIGNAL(textChanged(QString)),this,SLOT(m_onTextEditChanged()));
    m_remoteEntryLayout->addRow("User:",m_remoteUserEdit);

    m_remotePasswordEdit = new QLineEdit();
    m_remotePasswordEdit->setEchoMode(QLineEdit::Password);
    connect(m_remotePasswordEdit,SIGNAL(textChanged(QString)),this,SLOT(m_onTextEditChanged()));
    m_remoteEntryLayout->addRow("Password:",m_remotePasswordEdit);

    m_remotePathEdit = new QLineEdit();
    connect(m_remotePathEdit,SIGNAL(textChanged(QString)),this,SLOT(m_onTextEditChanged()));
    m_remoteEntryLayout->addRow("Path:",m_remotePathEdit);


    m_remoteEntryWidget->setLayout(m_remoteEntryLayout);
    m_remoteEntryVector->addWidget(m_remoteEntryWidget);

    m_remotePathBrowseButton =  new QPushButton("Browse");
    m_remotePathBrowseButton->setEnabled(false);
    m_remoteEntryVector->addWidget(m_remotePathBrowseButton);

    m_remoteLayout->addWidget(m_remoteEntryVector);

    m_savedRemotesVector = new QWidgetVector();
    m_savedRemotesVector->setDirection(QWidgetVector::TopToBottom);
    m_savedRemotesVector->addWidget(new QLabel("Saved Installations:"));
    m_savedRemotesWidget = new QListWidget();
    connect(m_savedRemotesWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(m_loadRemoteInstallation(QModelIndex)));
    m_savedRemotesVector->addWidget(m_savedRemotesWidget);

    m_remoteLayout->addWidget(m_savedRemotesVector);

    m_groupBoxVector->addGroup(m_remoteLayout,"Connect to Remote Installation of NCS");
}


bool NCSInstallationDialog::hasDefaultInstallation()
{
    return m_defaultContext.path != "";
}

bool NCSInstallationDialog::attemptDefault()
{
    if (hasDefaultInstallation())
    {
        if (m_defaultContext.host == "localhost")
            return attemptLocal(m_defaultContext.path);
        else
            return attemptRemote(m_defaultContext.host,m_defaultContext.user,m_defaultContext.password,m_defaultContext.path);

    }
    return false;
}
bool NCSInstallationDialog::attemptLocal(QString path)
{
    if (attemptingConnection())
        return false;

    m_currentContext.host = "localhost";
    m_currentContext.path = path;

    m_localCommandBridge = new NCSLocalCommandBridge(this);
    connect(m_localCommandBridge,SIGNAL(validationError(NCSInternalCommandBridge::ValidationError)),this,SLOT(m_localConnectionInvalidated(NCSInternalCommandBridge::ValidationError)));
    connect(m_localCommandBridge,SIGNAL(validated()),this,SLOT(m_localConnectionValidated()));
    m_localCommandBridge->validate(path);

    return true;
}

bool NCSInstallationDialog::attemptRemote(QString host, QString user, QString password, QString path)
{
    if (attemptingConnection())
        return false;

    m_currentContext.host = host;
    m_currentContext.user = user;
    m_currentContext.password = password;
    m_currentContext.path = path;

    m_socket = new QSshSocket(this);
    connect(m_socket,SIGNAL(connected()),this,SLOT(m_onRemoteConnect()));
    connect(m_socket,SIGNAL(loginSuccessful()),this,SLOT(m_onRemoteAuthentication()));
    connect(m_socket,SIGNAL(error(QSshSocket::SshError)),this,SLOT(m_onRemoteError(QSshSocket::SshError)));
    m_socket->connectToHost(m_currentContext.host,22,QSshSocket::LogOperations);

    return true;
}

bool NCSInstallationDialog::attemptingConnection()
{
    return m_currentContext.path != "";
}
void NCSInstallationDialog::m_warnOfActiveConnection()
{
    QMessageBox msgBox;
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
    msgBox.setText("NCV is still attempting another connection. Please wait");
    msgBox.exec();
    return;
}


void NCSInstallationDialog::m_validateRemoteConnection()
{
    m_currentContext.guiTriggered = true;
    if (!attemptRemote(m_remoteHostEdit->text(),m_remoteUserEdit->text(),m_remotePasswordEdit->text(),m_remotePathEdit->text()))
        m_warnOfActiveConnection();

}
void NCSInstallationDialog::m_onRemoteAuthentication()
{
    m_remoteCommandBridge = new NCSRemoteCommandBridge(m_socket,this);
    connect(m_remoteCommandBridge,SIGNAL(validationError(NCSInternalCommandBridge::ValidationError)),this,SLOT(m_remoteConnectionInvalidated(NCSInternalCommandBridge::ValidationError)));
    connect(m_remoteCommandBridge,SIGNAL(validated()),this,SLOT(m_remoteConnectionValidated()));
    m_remoteCommandBridge->validate(m_currentContext.path);
}
void NCSInstallationDialog::m_onRemoteError(QSshSocket::SshError err)
{
    delete m_socket;

    if (m_currentContext.guiTriggered)
    {
        QMessageBox msgBox;
        msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
        msgBox.setText("Cannot connect to the remote host. Could be due to lack of internet connection or user account not being set up.");
        msgBox.exec();
    }

    m_currentContext = InstallationContext();
    attemptFailed(ConnectionError);
}
void NCSInstallationDialog::m_onRemoteConnect()
{
    m_socket->login(m_currentContext.user,m_currentContext.password);
}
void NCSInstallationDialog::m_remoteConnectionValidated()
{
    if (m_saveCheckBox->isChecked())
        m_saveCurrentInstallation();

    attemptSuccesful(m_remoteCommandBridge);
    m_remoteCommandBridge = NULL;
    m_currentContext = InstallationContext();
    close();
}
void NCSInstallationDialog::m_remoteConnectionInvalidated(NCSInternalCommandBridge::ValidationError err)
{

    if (m_currentContext.guiTriggered)
        m_reportError(err);

    if (m_remoteContexts.contains(m_currentContext))
        m_reportSavedContextFailed();

    delete m_socket;
    m_socket = NULL;
    delete m_remoteCommandBridge;
    m_remoteCommandBridge = NULL;
    m_currentContext = InstallationContext();

    attemptFailed((BridgeCreationError)err);
}


void NCSInstallationDialog::m_browseForLocalPath()
{
    QString dirPath = QFileDialog::getExistingDirectory(this,"Find NCS 6 Directory");
    if (dirPath != "")
        m_localPathEdit->setText(dirPath);
}
void NCSInstallationDialog::m_onTextEditChanged()
{
    m_checkValidationEnabled();
}

void NCSInstallationDialog::m_validateLocalConnection()
{
    m_currentContext.guiTriggered = true;
    if (!attemptLocal(m_localPathEdit->text()))
        m_warnOfActiveConnection();

}
void NCSInstallationDialog::m_localConnectionValidated()
{
    if (m_saveCheckBox->isChecked())
        m_saveCurrentInstallation();
    attemptSuccesful(m_localCommandBridge);
    m_localCommandBridge = NULL;
    m_currentContext = InstallationContext();
    close();
}
void NCSInstallationDialog::m_localConnectionInvalidated(NCSInternalCommandBridge::ValidationError err)
{
    if (m_currentContext.guiTriggered)
        m_reportError(err);

    if (m_localContexts.contains(m_currentContext))
        m_reportSavedContextFailed();

    delete m_localCommandBridge;
    m_localCommandBridge = NULL;
    m_currentContext = InstallationContext();

    attemptFailed((BridgeCreationError)err);
}

void NCSInstallationDialog::m_reportSavedContextFailed()
{
    int index;
    if (m_currentContext.host == "localhost")
        index = m_localContexts.indexOf(m_currentContext);
    else
        index = m_remoteContexts.indexOf(m_currentContext);

    if (index != -1)
    {
        QMessageBox msgBox;
        msgBox.setText("The installation that just failed was in your saved installtions. Do you wish to remove this installation?:");
        QPushButton * yesButton = msgBox.addButton(tr("Yes"), QMessageBox::ActionRole);
        msgBox.addButton(tr("No"), QMessageBox::ActionRole);
        msgBox.exec();

        if (msgBox.clickedButton() == yesButton)
        {
            m_removeSavedContext(m_currentContext);
            if (m_currentContext.host == "localhost")
            {
                delete m_savedLocalsWidget->takeItem(index);
                m_localContexts.remove(index);
            }
            else
            {
                delete m_savedRemotesWidget->takeItem(index);
                m_remoteContexts.remove(index);
            }
        }

    }


}

void NCSInstallationDialog::m_removeSavedContext(InstallationContext context)
{
    // check to see if saved installation document contains this installation already.
    QDomNodeList hostNodes = m_rootDocumentElement.elementsByTagName(context.host);
    for (int i =0; i< hostNodes.count(); i ++)
    {
        QDomElement hostElement = hostNodes.at(i).toElement();
        // if the installation is already contained, return
        if (hostElement.attribute("path") == m_currentContext.path)
            if (hostElement.attribute("user") == m_currentContext.user)
            {
                m_rootDocumentElement.removeChild(hostElement);
                m_save();
                return;
            }
    }
}


void NCSInstallationDialog::m_reportError(NCSInternalCommandBridge::ValidationError err)
{
    QMessageBox msgBox;
    msgBox.setText("The NCS installation could not be validated:");
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);

    if (err == NCSInternalCommandBridge::MissingMPI)
        msgBox.setDetailedText("MPI could not be found in the host's system path.");
    else if (err == NCSInternalCommandBridge::MissingRootDirectory)
        msgBox.setDetailedText("The path specified could not be found.");
    else if (err == NCSInternalCommandBridge::MissingBuildDirectory)
        msgBox.setDetailedText("The path specified does not contain a build directory.");
    else if (err == NCSInternalCommandBridge::MissingApplicationDirectory)
        msgBox.setDetailedText("The build directory at the path specified does not contain applications.");
    else if (err == NCSInternalCommandBridge::MissingPluginDirectory)
        msgBox.setDetailedText("The build directory at the path specified does not contain plugins.");

    msgBox.exec();
}


void NCSInstallationDialog::m_saveCurrentInstallation()
{
    // check to see if saved installation document contains this installation already.
    QDomNodeList hostNodes = m_rootDocumentElement.elementsByTagName(m_currentContext.host);
    for (int i =0; i< hostNodes.count(); i ++)
    {
        QDomElement hostElement = hostNodes.at(i).toElement();
        // if the installation is already contained, return
        if (hostElement.attribute("path") == m_currentContext.path)
            if (hostElement.attribute("user") == m_currentContext.user)
            {
                if (m_defaultCheckBox->isChecked() && hostElement !=  m_rootDocumentElement.firstChild())
                {
                    m_rootDocumentElement.insertBefore(hostElement,m_rootDocumentElement.firstChild());
                    m_save();
                }

                return;
            }
    }

    // create element and set attributes
    QDomElement newElement = m_savedInstallationsDocument.createElement(m_currentContext.host);
    newElement.setAttribute("path",m_currentContext.path);
    if (m_currentContext.host != "localhost")
    {
        newElement.setAttribute("user",m_currentContext.user);

        //encrypt password
        SimpleCrypt crypto(Q_UINT64_C(0x0c2ad4a4acb9f023)); //some random number
        crypto.setIntegrityProtectionMode(SimpleCrypt::ProtectionHash);  //properly protect the integrity of the data
        QByteArray encryptedPassword = crypto.encryptToByteArray(m_currentContext.password.toAscii());
        newElement.setAttribute("password",QString(encryptedPassword.toHex().data()));

    }


    // put element in file
    if (m_rootDocumentElement.hasChildNodes() && m_defaultCheckBox->isChecked())
        m_rootDocumentElement.insertBefore(newElement,m_rootDocumentElement.firstChild());
    else
        m_rootDocumentElement.appendChild(newElement);

    // save file
    m_save();
}
void NCSInstallationDialog::m_save()
{
    QFile file(m_installationFilename);
    file.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream stream( &file );
    stream << m_savedInstallationsDocument.toString();
    file.close();
}

void NCSInstallationDialog::m_loadRemoteInstallation(QModelIndex index)
{
    InstallationContext context = m_remoteContexts[index.row()];
    m_remoteHostEdit->setText(context.host);
    m_remoteUserEdit->setText(context.user);
    m_remotePasswordEdit->setText(context.password);
    m_remotePathEdit->setText(context.path);
}
void NCSInstallationDialog::m_loadLocalInstallation(QModelIndex index)
{
    InstallationContext context = m_localContexts[index.row()];
    m_localPathEdit->setText(context.path);
}

void NCSInstallationDialog::m_checkValidationEnabled()
{
    if (m_local)
    {
        if (m_localPathEdit->text() != "" && QDir(m_localPathEdit->text()).exists())
            m_bottomVector->setEnabled(true);
        else
            m_bottomVector->setEnabled(false);
    }
    else
    {
        if (m_remoteHostEdit->text() == "" || m_remoteUserEdit->text() == "" || m_remotePasswordEdit->text() == "" || m_remotePathEdit->text() == "")
            m_bottomVector->setEnabled(false);
        else
            m_bottomVector->setEnabled(true);
    }
}

void NCSInstallationDialog::m_enableDefaultCheckBox(bool enable)
{
    m_defaultCheckBox->setEnabled(enable);
    m_defaultChecked(enable);
}

void NCSInstallationDialog::m_defaultChecked(bool checked)
{
    InstallationContext entryContext;
    if (m_local)
        entryContext = InstallationContext(m_localPathEdit->text());
    else
        entryContext = InstallationContext(m_remoteHostEdit->text(),m_remoteUserEdit->text(),m_remotePasswordEdit->text(),m_remotePathEdit->text());

    int savedInstallations = m_localContexts.count() + m_remoteContexts.count();
    bool currentIsSavedInstallation = (m_localContexts.contains(entryContext) || m_remoteContexts.contains(entryContext));

    if (savedInstallations == 0 || (currentIsSavedInstallation && savedInstallations ==1))
    {
        m_defaultCheckBox->setToolTip("When only one installation is saved, this installation is always default.");
        m_defaultCheckBox->blockSignals(true);
        m_defaultCheckBox->setChecked(true);
        m_defaultCheckBox->blockSignals(false);
    }
}

void NCSInstallationDialog::m_installationTypeSwitched(QLayout* layout)
{
    m_local = (layout == m_localLayout);

    m_validateButton->disconnect();
    if (m_local)
        connect(m_validateButton,SIGNAL(clicked()),this,SLOT(m_validateLocalConnection()));
    else
        connect(m_validateButton,SIGNAL(clicked()),this,SLOT(m_validateRemoteConnection()));

    m_checkValidationEnabled();
}
