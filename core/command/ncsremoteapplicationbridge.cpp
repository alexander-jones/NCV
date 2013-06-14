#include "ncsremoteapplicationbridge.h"

NCSRemoteApplicationBridge::NCSRemoteApplicationBridge(QString name,QString projectDirectory, QObject *parent)
    :NCSApplicationBridge(parent)
{
    m_name = name;
    m_projectDir = projectDirectory;
    m_alive = false;
    m_pidString = -1;
    m_destroyProcess = true;
    m_socket = NULL;
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(m_checkIfAlive()));
}
NCSRemoteApplicationBridge::~NCSRemoteApplicationBridge()
{
    m_timer->stop();
    disconnect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
    disconnect(m_socket,SIGNAL(error(QSshSocket::SshError)),this,SLOT(m_onSocketError(QSshSocket::SshError)));
    disconnect(m_socket,SIGNAL(pullSuccessful(QString,QString)),this,SLOT(m_executeNextPull()));
    disconnect(m_socket,SIGNAL(pushSuccessful(QString,QString)),this,SLOT(m_executeNextPush()));

    if (m_alive && m_destroyProcess)
    {
        m_socket->executeCommand("kill " + m_pidString);
    }
}
void NCSRemoteApplicationBridge::setSocket(QSshSocket * socket, bool own)
{
    Q_UNUSED(own);
    m_socket = socket;
    connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
    connect(m_socket,SIGNAL(error(QSshSocket::SshError)),this,SLOT(m_onSocketError(QSshSocket::SshError)));
    connect(m_socket,SIGNAL(pullSuccessful(QString,QString)),this,SLOT(m_executeNextPull()));
    connect(m_socket,SIGNAL(pushSuccessful(QString,QString)),this,SLOT(m_executeNextPush()));
}

void NCSRemoteApplicationBridge::stopExecution(bool destroy)
{
    m_destroyProcess = destroy;
    executionFinished();
}

QString NCSRemoteApplicationBridge::applicationName()
{
    return m_name;
}
void NCSRemoteApplicationBridge::start(QString application, NCSCommandArguments arguments)
{
    QVector<NCSCommandFileArgument> fileArgs = arguments.fileArguments();
    QStringList argLiterals = arguments.literals();
    for (int i = 0; i <fileArgs.size();i++)
    {
        int literalIndex = argLiterals.indexOf(fileArgs[i].literal);
        if (literalIndex != -1)
            argLiterals.replace(literalIndex, m_projectDir + "/"+ fileArgs[i].literal);
        fileArgs[i].literal = m_projectDir + "/"+fileArgs[i].literal;

        if (fileArgs[i].operation == NCSCommandFileArgument::UploadBeforeExecution)
            m_uploadArguments.append(fileArgs[i]);
        if (fileArgs[i].operation == NCSCommandFileArgument::DownloadAfterExecution)
            m_downloadArguments.append(fileArgs[i]);

    }

    m_wholeCommand = application;
    for (int i=0; i < argLiterals.size();i++)
        m_wholeCommand +=" " + argLiterals[i];

    m_alive = true;
    m_executeNextPush();
}

QString NCSRemoteApplicationBridge::readAllStandardError()
{
    QString retString = m_stdErr;
    m_stdErr = "";
    return retString;
}

QString NCSRemoteApplicationBridge::readAllStandardOutput()
{
    QString retString = m_stdOut;
    m_stdOut = "";
    return retString;
}

void NCSRemoteApplicationBridge::m_onCommandExecuted(QString command,QString stdOut, QString stdError)
{
    if (command == "pidof " + m_name)
    {
        if (stdOut == "")
        {
            m_alive = false;
            executionFinished();
        }
        else
        {
            QStringList pids = stdOut.split( " ");
            m_pidString = pids.first();
            m_pidString.replace("\r\n","");
            m_pidString.replace("\n","");
            m_timer->start(10000);
        }
    }
    else
    {
        m_stdOut += stdOut;
        m_stdErr += stdError;
        if (stdOut.size() > 0)
            readyReadStandardOutput();
        if (stdError.size() > 0)
            readyReadStandardError();
        m_executeNextPull();
    }
}

void NCSRemoteApplicationBridge::m_checkIfAlive()
{
    m_socket->executeCommand("pidof " + m_name);
}

void NCSRemoteApplicationBridge::m_executeNextPush()
{
    if (m_uploadArguments.size() > 0)
    {
        NCSCommandFileArgument arg = m_uploadArguments[0];
        m_uploadArguments.pop_front();
        m_socket->pushFile(arg.filename,arg.literal);
    }
    else
    {
        m_socket->executeCommand(m_wholeCommand);
    }
}

void NCSRemoteApplicationBridge::m_executeNextPull()
{
    if (m_downloadArguments.size() > 0)
    {
        NCSCommandFileArgument arg = m_downloadArguments[0];
        m_downloadArguments.pop_front();
        m_socket->pullFile(arg.filename,arg.literal);
    }
    else
    {
        disconnect(m_socket,SIGNAL(pullSuccessful(QString,QString)),this,SLOT(m_executeNextPull()));
        readyReadStandardOutput();
        readyReadStandardError();
        m_checkIfAlive();
    }


}
void NCSRemoteApplicationBridge::m_onSocketError(QSshSocket::SshError err)
{
    if (m_uploadArguments.count() > 0 )
    {
        if (err == QSshSocket::ScpPushRequestError || err == QSshSocket::ScpWriteError)
            executionError(NCSApplicationBridge::SyncUploadError);
    }
    else if(m_downloadArguments.count() > 0 )
    {
        if (err == QSshSocket::ScpPullRequestError || err == QSshSocket::ScpReadError)
            executionError(NCSApplicationBridge::SyncDownloadError);
    }
    else
        executionError(NCSApplicationBridge::UnknownError);
}
