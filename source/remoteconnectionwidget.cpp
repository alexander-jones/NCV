#include "remoteconnectionwidget.h"
#include <QBuffer>

RemoteConnectionWidget::RemoteConnectionWidget(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QHBoxLayout();

    m_credentialEntryWidget = new QFrame();
    m_credentialEntryWidget->setFrameShadow(QFrame::Sunken);
    m_credentialEntryWidget->setFrameShape(QFrame::Box);
    m_credentialEntryLayout = new QVBoxLayout();
    m_credentialEntryLayout->setAlignment(Qt::AlignCenter);


    m_credentialEntryLabel = new QLabel("Cluster Credentials:");
    m_credentialEntryLabel->setAlignment(Qt::AlignCenter);
    m_credentialEntryLayout->addWidget(m_credentialEntryLabel);

    m_loginWidget = new QwwLoginBox();
    m_loginWidget->setFields( QwwLoginBox::HostField | QwwLoginBox::UserField	| QwwLoginBox::PasswordField);

    m_credentialEntryLayout->addWidget(m_loginWidget);

    m_buttonWidget = new QWidget();
    m_buttonLayout = new QGridLayout();

    m_loadCredentialsButton = new QPushButton("Browse For Credentials");
    connect(m_loadCredentialsButton,SIGNAL(clicked()),this,SLOT(m_loadCredentials()));
    m_buttonLayout->addWidget(m_loadCredentialsButton,0,0);

    m_saveCredentialsButton = new QPushButton("Save Credentials");
    connect(m_saveCredentialsButton,SIGNAL(clicked()),this,SLOT(m_saveCredentials()));
    m_buttonLayout->addWidget(m_saveCredentialsButton,0,1);

    m_connectButton = new QPushButton("Connect");
    m_buttonLayout->addWidget(m_connectButton,1,0);

    connect(m_connectButton,SIGNAL(clicked()),this,SLOT(m_tryConnect()));

    m_buttonWidget->setLayout(m_buttonLayout);

    m_credentialEntryLayout->addWidget(m_buttonWidget);

    m_credentialEntryWidget->setLayout(m_credentialEntryLayout);
    m_layout->addWidget(m_credentialEntryWidget);

    m_recentCredentialWidget = new QFrame();
    m_recentCredentialWidget->setFrameShadow(QFrame::Sunken);
    m_recentCredentialWidget->setFrameShape(QFrame::Box);
    m_recentCredentialLayout = new QVBoxLayout();

    m_recentCredentialsLabel = new QLabel("Recent Credentials:");
    m_recentCredentialsLabel->setAlignment(Qt::AlignCenter);
    m_recentCredentialLayout->addWidget(m_recentCredentialsLabel);

    m_recentCredentials = new QListWidget();

    QStringList allRecentCredentials;
    QFile recentCredentials("./user/recentCredentialFiles.txt");
    if (!recentCredentials.exists())
    {
        QFile file("./user/recentCredentialFiles.txt");
        file.open(QIODevice::ReadWrite);
        file.close();
    }
    else
    {
        if(!recentCredentials.open(QIODevice::ReadOnly)) {
            QMessageBox::information(0, "error", recentCredentials.errorString());
        }
        allRecentCredentials = QString(recentCredentials.readAll()).split(" ");
        recentCredentials.close();
    }
    QFile::resize("./user/recentCredentialFiles.txt",0);
    if(!recentCredentials.open(QIODevice::WriteOnly)) {
        QMessageBox::information(0, "error", recentCredentials.errorString());
    }
    for (int i = 0; i < allRecentCredentials.size(); i ++)
    {
        if (QFile(allRecentCredentials[i]).exists())
        {
            recentCredentials.write(QString(allRecentCredentials[i] + " ").toAscii());
            m_recentCredentials->addItem(new QListWidgetItem(QString(allRecentCredentials[i])));
        }
    }
    recentCredentials.close();
    connect(m_recentCredentials,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(m_loadCredentials(QListWidgetItem*)));
    m_recentCredentialLayout->addWidget(m_recentCredentials,0,0);

    m_recentCredentialWidget->setLayout(m_recentCredentialLayout);
    m_layout->addWidget(m_recentCredentialWidget);

    this->setLayout(m_layout);
    m_connection = new SSHSocket(this);
    connect(m_connection,SIGNAL(connected()),this,SLOT(m_onConnect()));
    connect(m_connection,SIGNAL(authenticated()),this,SLOT(m_onAuthentication()));
    connect(m_connection,SIGNAL(error(SSHSocket::SSHSocketError)),this,SLOT(m_onError(SSHSocket::SSHSocketError)));


}


void RemoteConnectionWidget::m_loadCredentials(QListWidgetItem* item)
{
    QString fileName = item->text().mid(0,item->text().size());
    if(loadCredentials(fileName))
        m_addToRecentCredentials(fileName);
}

void RemoteConnectionWidget::m_loadCredentials()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Load Credentials"));
    if(loadCredentials(fileName))
        m_addToRecentCredentials(fileName);
}
void RemoteConnectionWidget::m_onError(SSHSocket::SSHSocketError err)
{
    qDebug() << "ERROR: " << err;
    connectionFailed();

}

bool RemoteConnectionWidget::loadCredentials(QString filename)
{

    if (filename == "")
        return false;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray bytes = file.readAll();

    SimpleCrypt crypto(Q_UINT64_C(0x0c2ad4a4acb9f023)); //same random number: key should match encryption key
    crypto.setIntegrityProtectionMode(SimpleCrypt::ProtectionHash);  //properly protect the integrity of the data
    QByteArray allText = crypto.decryptToByteArray(bytes);


    if (!crypto.lastError() == SimpleCrypt::ErrorNoError)
    {
        QMessageBox msgBox;
        msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
        msgBox.setText("The file you selected is not a credential file. Select a file saved via this credential dialog.");
        msgBox.exec();
        return false;
    }

    QStringList items = QString(allText).split(" ");

    m_loginWidget->setUser( items[0], items[2]);
    m_loginWidget->setHost( items[1]);
    m_addToRecentCredentials(filename);
    return true;

}

void RemoteConnectionWidget::m_saveCredentials()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Credentials"));
    saveCredentials(fileName);


}

void RemoteConnectionWidget::saveCredentials(QString fileName)
{
    //setup our objects
    SimpleCrypt crypto(Q_UINT64_C(0x0c2ad4a4acb9f023)); //some random number
    crypto.setIntegrityProtectionMode(SimpleCrypt::ProtectionHash);  //properly protect the integrity of the data
    QString allText = m_loginWidget->user()  + " " + m_loginWidget->host() + " " + m_loginWidget->password();

    QByteArray credentialsEncrypted = crypto.encryptToByteArray(allText.toAscii());

    if (crypto.lastError() == SimpleCrypt::ErrorNoError) {

        if (fileName == "")
            return;

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
            return;
        file.write(credentialsEncrypted.data());
        file.close();
        qDebug() << fileName;
        m_addToRecentCredentials(fileName);
    }

}
void RemoteConnectionWidget::m_addToRecentCredentials(QString fileName)
{

    for (int i = 0; i < m_recentCredentials->count(); i ++)
    {
        if(m_recentCredentials->item(i)->text() == fileName)
            return;
    }

    QFile recentCredentials("./user/recentCredentialFiles.txt");
    if (!recentCredentials.open(QIODevice::Append))
        return;
    recentCredentials.write((fileName + " ").toAscii());
    recentCredentials.close();

    m_recentCredentials->addItem(new QListWidgetItem(fileName));
}

void RemoteConnectionWidget::m_tryConnect()
{
    QMessageBox msgBox;
    msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);

    if (m_loginWidget->user() == "" || m_loginWidget->host() == "" || m_loginWidget->password() == "" )
    {
        msgBox.setText("Fields were left empty. Fill all remaining fields and try again.");
        msgBox.exec();
        return;
    }

    connectionAttempted();
    if (m_connection->isConnected())
        m_connection->disconnectFromHost();
    m_connection->connectToHost(m_loginWidget->host());

}


void RemoteConnectionWidget::m_onConnect()
{
    m_connection->login(m_loginWidget->user(),m_loginWidget->password());
}


void RemoteConnectionWidget::m_onAuthentication()
{
    connected(m_connection);
    return;
}


