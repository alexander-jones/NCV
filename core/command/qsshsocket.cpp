#include "qsshsocket.h"
#include <QFileInfo>

QSshSocket::QSshSocket(QObject * parent )
    :QThread(parent)
{
    m_host = "";
    m_user = "";
    m_password = "";
    m_port = -1;
    m_loggedIn = false;
    m_session  = NULL;
    m_clone = NULL;
    m_workingDirectory = ".";

    qRegisterMetaType<QSshSocket::SshError>("QSshSocket::SshError");
    m_currentOperation.executed = true;

    m_run = true;
    start();
}

QSshSocket::~QSshSocket()
{
    if (m_session != NULL)
        ssh_free(m_session);
    m_run = false;
    this->wait();
}

void QSshSocket::run()
{

    while(m_run)
    {
        if (m_session == NULL)
        {
            if (m_host != "")
                m_attemptConnect();
        }


        // if we have a vaild ssh connection, authenticate connection with credentials
        else if (!m_loggedIn)
        {
            // check to see if a username and a password have been given
            if (m_user != "" && m_password !="")
                m_attemptLogin();
        }
        // if all ssh setup has been completed, check to see if we have any commands to execute
        else if (!m_currentOperation.executed)
        {

            if (m_currentOperation.type == Command || m_currentOperation.type == WorkingDirectoryTest)
                m_attemptCommand();
            // if all ssh setup has been completed, check to see if we have any file transfers to execute
            else if (m_currentOperation.type == Pull)
                m_attemptPull();
            else if (m_currentOperation.type == Push)
                m_attemptPush();


            m_currentOperation.executed = true;
        }
        else
        {
            msleep(100);
        }

    }

}

void QSshSocket::clone()
{
    m_clone = new QSshSocket(this->parent());
    connect(m_clone,SIGNAL(connected()),this,SLOT(m_onCloneConnect()));
    connect(m_clone,SIGNAL(loginSuccessful()),this,SLOT(m_onCloneLoggedIn()));
    connect(m_clone,SIGNAL(workingDirectorySet(QString)),this,SLOT(m_onCloneCWDSet(QString)));
    connect(m_clone,SIGNAL(error(QSshSocket::SshError)),this,SLOT(m_onCloneError(QSshSocket::SshError)));

    if (m_connected)
        m_clone->connectToHost(m_host,m_port,m_loggingProtocol);
    else
    {
        m_releaseClone();
        cloned(m_clone);
        m_clone = NULL;
    }
}
void QSshSocket::m_releaseClone()
{
    disconnect(m_clone,SIGNAL(connected()),this,SLOT(m_onCloneConnect()));
    disconnect(m_clone,SIGNAL(loginSuccessful()),this,SLOT(m_onCloneLoggedIn()));
    disconnect(m_clone,SIGNAL(workingDirectorySet(QString)),this,SLOT(m_onCloneCWDSet(QString)));
    disconnect(m_clone,SIGNAL(error(QSshSocket::SshError)),this,SLOT(m_onCloneError(QSshSocket::SshError)));
}

void QSshSocket::m_onCloneConnect()
{
    if (m_loggedIn)
        m_clone->login(m_user,m_password);
    else
    {
        m_releaseClone();
        cloned(m_clone);
        m_clone = NULL;
    }
}

void QSshSocket::m_onCloneLoggedIn()
{
    if (m_workingDirectory != ".")
        m_clone->setWorkingDirectory(m_workingDirectory);
    else
    {
        m_releaseClone();
        cloned(m_clone);
        m_clone = NULL;
    }
}

void QSshSocket::m_onCloneCWDSet(QString cwd)
{
    Q_UNUSED(cwd);
    m_releaseClone();
    cloned(m_clone);
    m_clone = NULL;
}

void QSshSocket::m_onCloneError(QSshSocket::SshError)
{
    m_releaseClone();
    error(QSshSocket::CloneError);
    m_clone = NULL;
}

void QSshSocket::disconnectFromHost()
{
    m_host = "";
    m_user = "";
    m_password = "";
    m_port = -1;
    m_loggedIn = false;
    m_clone = NULL;
    if (m_session != NULL)
    {
        ssh_disconnect(m_session);
        ssh_free(m_session);
    }
    m_session = NULL;
}

void QSshSocket::connectToHost(QString host, int port,LoggingProtocol logging)
{
    m_host = host;
    m_port = port;
    m_loggingProtocol = logging;
}
void QSshSocket::login(QString user, QString password)
{
    m_user = user;
    m_password = password;
}
void QSshSocket::executeCommand(QString command)
{
    m_currentOperation.type = Command;
    if (m_workingDirectory != ".")
        m_currentOperation.adminCommand = "cd " + m_workingDirectory + "; "  + command;
    else
        m_currentOperation.adminCommand = command ;

    m_currentOperation.command =command;
    m_currentOperation.executed = false;
}

void QSshSocket::pullFile(QString localPath, QString remotePath)
{
    m_currentOperation.localPath = localPath;
    if (QFileInfo(remotePath).isAbsolute())
        m_currentOperation.remotePath = remotePath;
    else
        m_currentOperation.remotePath = m_workingDirectory + "/" + remotePath;
    m_currentOperation.type = Pull;
    m_currentOperation.executed = false;
}

void QSshSocket::pushFile(QString localPath, QString remotePath)
{
    m_currentOperation.localPath = localPath;
    if (QFileInfo(remotePath).isAbsolute())
        m_currentOperation.remotePath = remotePath;
    else
        m_currentOperation.remotePath = m_workingDirectory + "/" + remotePath;
    m_currentOperation.type = Push;
    m_currentOperation.executed = false;
}

void QSshSocket::setWorkingDirectory(QString path)
{
    m_nextWorkingDir = path;
    m_currentOperation.type = WorkingDirectoryTest;
    m_currentOperation.adminCommand = "[ -d " + m_nextWorkingDir +" ] && echo 'exists'";
    m_currentOperation.executed = false;
}

bool QSshSocket::isConnected()
{
    return m_session != NULL;
}

bool QSshSocket::isLoggedIn()
{
    return m_loggedIn;
}

QString QSshSocket::user(){return m_user;}
QString QSshSocket::password(){return m_password;}
QString QSshSocket::host(){return m_host;}
int QSshSocket::port(){return m_port;}

void QSshSocket::m_attemptConnect()
{
    m_session = ssh_new();

    // set the pertinant ssh session options
    ssh_options_set(m_session, SSH_OPTIONS_HOST, m_host.toAscii().data());
    ssh_options_set(m_session, SSH_OPTIONS_LOG_VERBOSITY, &m_loggingProtocol);
    ssh_options_set(m_session, SSH_OPTIONS_PORT, &m_port);

    // try to connect given host, user, port
    int connectionResponse = ssh_connect(m_session);

    // if connection is Successful keep track of connection info.
    if (connectionResponse == SSH_OK)
    {
        m_connected = true;
        connected();
    }

    else
        error(SessionCreationError);
}

void QSshSocket::m_attemptLogin()
{

    // try authenticating current user at remote host
    int worked = ssh_userauth_password(m_session, m_user.toAscii().data(), m_password.toAscii().data());


    // if successful, store user password.
    if (worked == SSH_OK)
    {
        loginSuccessful();
        m_loggedIn = true;
    }
    else
    {
        m_user = "";
        m_password = "";
        error(PasswordAuthenticationFailedError);
    }


}

void QSshSocket::m_attemptCommand()
{
    if (m_loggingProtocol == LogAll || m_loggingProtocol == LogOperations)
        qDebug() << "!!! Command '" << m_currentOperation.command << "'' executing on "<< m_user << "@" << m_host <<" at working directory " << m_workingDirectory << " ...";
    // attempt to open ssh shell channel
    ssh_channel channel = ssh_channel_new(m_session);

    // if attempt fails,return
    if (ssh_channel_open_session(channel) != SSH_OK)
    {
        error(ChannelCreationError);
        return;
    }


    int requestResponse = SSH_AGAIN;

    // attempt to execute shell command
    while (requestResponse == SSH_AGAIN)
        requestResponse = ssh_channel_request_exec(channel, m_currentOperation.adminCommand.toAscii().data());

    // if attempt not executed, close connection then return
    if (requestResponse != SSH_OK)
    {
        error(ChannelCreationError);
        return;
    }


    QByteArray buffer;
    buffer.resize(1000);

    // read in std out
    int totalBytes = 0, newBytes = 0;
    do
    {
        newBytes = ssh_channel_read(channel, &(buffer.data()[totalBytes]), buffer.size() - totalBytes, 0);
        if (newBytes > 0)
            totalBytes += newBytes;
    }while ( newBytes > 0);


    QString stdOut = QString(buffer).mid(0,totalBytes);

    // read in std err
    totalBytes = 0, newBytes = 0;
    do
    {
        newBytes = ssh_channel_read(channel, &(buffer.data()[totalBytes]), buffer.size() - totalBytes, 1);
        if (newBytes > 0)
            totalBytes += newBytes;
    }while ( newBytes > 0);

    QString stdErr = QString(buffer).mid(0,totalBytes);

    if (totalBytes >0)
    {
        qDebug() << ssh_get_error(m_session);

    }

    // close channel
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    if (m_currentOperation.type == WorkingDirectoryTest)
    {
        if (stdOut.contains("exists"))
            m_workingDirectory = m_nextWorkingDir;
        m_nextWorkingDir = ".";
        workingDirectorySet(m_workingDirectory);
    }
    else
        commandExecuted( m_currentOperation.command, stdOut,stdErr) ;
}

void QSshSocket::m_attemptPull()
{
    if (m_loggingProtocol == LogAll || m_loggingProtocol == LogOperations)
        qDebug() << "!!! Remote file '"<< m_currentOperation.remotePath << "'' being pulled to local file '"<< m_currentOperation.localPath <<"'' ..." ;

    ssh_scp scpSession = ssh_scp_new(m_session,SSH_SCP_READ, m_currentOperation.remotePath.toAscii().data());
    if (scpSession == NULL)
        error(ScpChannelCreationError);

    // attempt to initialize new scp session.
    int scpInitialized = ssh_scp_init(scpSession);
    if(scpInitialized != SSH_OK)
    {
        ssh_scp_close(scpSession);
        ssh_scp_free(scpSession);
        error(ScpChannelCreationError);
        return;
    }


    // attempt to authorize new scp pull
    if (ssh_scp_pull_request(scpSession) != SSH_SCP_REQUEST_NEWFILE)
    {
        ssh_scp_close(scpSession);
        ssh_scp_free(scpSession);
        error(ScpPullRequestError);
        return;
    }

    // accept authorization
    ssh_scp_accept_request(scpSession);


    // get remote file size
    int size = ssh_scp_request_get_size(scpSession);

    // resize buffer, read remote file into buffer
    QByteArray buffer;
    buffer.resize(size);

    // if an error happens while reading, close the scp session and return
    if (ssh_scp_read(scpSession, buffer.data() , size) == SSH_ERROR)
    {
        ssh_scp_close(scpSession);
        ssh_scp_free(scpSession);
        error(ScpReadError);
        return;
    }

    // loop until eof flag
    if  (ssh_scp_pull_request(scpSession)  != SSH_SCP_REQUEST_EOF)
    {
        ssh_scp_close(scpSession);
        ssh_scp_free(scpSession);
        error(ScpReadError);
        return;
    }

    //close scp session
    ssh_scp_close(scpSession);
    ssh_scp_free(scpSession);

    // open up local file and write contents of buffer to it.
    QFile file(m_currentOperation.localPath);
    file.open(QIODevice::WriteOnly);
    file.write(buffer);
    file.close();

    pullSuccessful(m_currentOperation.localPath,m_currentOperation.remotePath);
}

void QSshSocket::m_attemptPush()
{
    if (m_loggingProtocol == LogAll || m_loggingProtocol == LogOperations)
        qDebug() << "!!! Local file '" << m_currentOperation.localPath <<"'' being pushed to remote file '"<< m_currentOperation.remotePath << "'' ..." ;

    // attempt to create new scp from ssh session.
    ssh_scp scpSession = ssh_scp_new(m_session,SSH_SCP_WRITE, m_currentOperation.remotePath.toAscii().data());

    // if creation failed, return
    if (scpSession == NULL)
        error(SocketError);


    // attempt to initialize new scp session.
    int scpInitialized = ssh_scp_init(scpSession);


    // if failed, close scp session and return.
    if(scpInitialized != SSH_OK)
    {
        ssh_scp_close(scpSession);
        ssh_scp_free(scpSession);
        error(ScpChannelCreationError);
        return;
    }


    // open the local file and check to make sure it exists
    // if not, close scp session and return.
    QFile file(m_currentOperation.localPath);
    if (!file.exists())
    {
        ssh_scp_close(scpSession);
        ssh_scp_free(scpSession);
        error(ScpFileNotCreatedError);
        return;
    }

    // if the file does exist, read all contents as bytes
    file.open(QIODevice::ReadOnly);
    QByteArray buffer =file.readAll();
    file.close();

    // attempt to authorize pushing bytes over scp socket
    // if this fails, close scp session and return.
    if (ssh_scp_push_file(scpSession, m_currentOperation.remotePath.toAscii().data(), buffer.size(), S_IRUSR | S_IWUSR) != SSH_OK)
    {
        ssh_scp_close(scpSession);
        ssh_scp_free(scpSession);
        error(ScpPushRequestError);
        return;
    }


    // once authorized to push bytes over scp socket, start writing
    // if an error is returned,  close scp session and return.
    if ( ssh_scp_write(scpSession,buffer.data(), buffer.size()) != SSH_OK)
    {

        ssh_scp_close(scpSession);
        ssh_scp_free(scpSession);
        error(ScpWriteError);
        return;
    }


    // close scp session and return.
    ssh_scp_close(scpSession);
    ssh_scp_free(scpSession);

    pushSuccessful(m_currentOperation.localPath,m_currentOperation.remotePath);
}
