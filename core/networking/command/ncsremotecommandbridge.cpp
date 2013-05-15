#include "ncsremotecommandbridge.h"

NCSRemoteApplicationBridge::NCSRemoteApplicationBridge(QString name, QObject *parent)
    :NCSApplicationBridge(parent)
{
    m_name = name;
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
    disconnect(m_socket,SIGNAL(commandExecuted(QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString)));
    disconnect(m_socket,SIGNAL(error(QSshSocket::SshError)),this,SLOT(m_onSocketError(QSshSocket::SshError)));
    disconnect(m_socket,SIGNAL(pullSuccessful(QString,QString)),this,SLOT(m_executeNextPull()));

    if (m_alive)
        if (m_destroyProcess)
            m_socket->executeCommand("kill " + m_pidString);
}
void NCSRemoteApplicationBridge::setSocket(QSshSocket * socket, bool own)
{
    m_socket = socket;
    connect(m_socket,SIGNAL(commandExecuted(QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString)));
    connect(m_socket,SIGNAL(error(QSshSocket::SshError)),this,SLOT(m_onSocketError(QSshSocket::SshError)));
    connect(m_socket,SIGNAL(pullSuccessful(QString,QString)),this,SLOT(m_executeNextPull()));
}

void NCSRemoteApplicationBridge::scheduleDestruction(bool destroy)
{
    m_destroyProcess = destroy;
    executionFinished();
}

QString NCSRemoteApplicationBridge::applicationName()
{
    return m_name;
}
void NCSRemoteApplicationBridge::start(QString application, QStringList arguments, QVector<NCSCommandFileArgument> downloadArgs)
{
    m_downloadArguments = downloadArgs;
    for (int i=0; i < arguments.size();i++)
        application +=" " + arguments[i];
    m_alive = true;
    m_socket->executeCommand(application);
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

void NCSRemoteApplicationBridge::m_onCommandExecuted(QString command,QString response)
{
    if (command == "pidof " + m_name)
    {
        if (response == "")
        {
            m_alive = false;
            executionFinished();
        }
        else
        {
            QStringList pids = response.split( " ");
            m_pidString = pids.first();
            m_pidString.replace("\r\n","");
            m_pidString.replace("\n","");
            m_timer->start(10000);
        }
    }
    else
    {
        m_stdOut = response;
        m_stdErr = "";
        m_executeNextPull();
    }
}

void NCSRemoteApplicationBridge::m_checkIfAlive()
{
    m_socket->executeCommand("pidof " + m_name);
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
    executionError(NCSApplicationBridge::UnknownError);
}

NCSRemoteCommandBridge::NCSRemoteCommandBridge( QObject *parent ):NCSCommandBridge(parent)
{
    m_socket = NULL;
    m_currentApplication = NULL;
    m_clearProjectContext();
}

void NCSRemoteCommandBridge::initialize(QString projectPath, QSshSocket * socket)
{
    m_projectPath = projectPath;
    if (m_socket != NULL)
        m_socket->disconnect();
    m_socket = socket;

    connect(m_socket,SIGNAL(commandExecuted(QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString)));
    connect(m_socket,SIGNAL(workingDirectorySet(QString)),this,SLOT(m_socketDirectorySet(QString)));
    connect(m_socket,SIGNAL(pushSuccessful(QString,QString)),this,SLOT(m_executeNextPush()));
    connect(m_socket,SIGNAL(cloned(QSshSocket*)),this,SLOT(m_onSocketCloned(QSshSocket*)));

}


void NCSRemoteCommandBridge::validate(QString path)
{
    m_remoteRootPath = path;
    m_remoteBuildPath = m_remoteRootPath + "/build";
    m_remoteProjectPath = "tmp-project";
    m_socket->executeCommand("type -P mpirun &>/dev/null && echo 'exists'");
}

void NCSRemoteCommandBridge::launchApplication(QString application, NCSCommandArguments arguments)
{
    if (!m_valid)
        return ;

    QVector<NCSCommandFileArgument> fileArgs = arguments.fileArguments();
    QStringList argLiterals = arguments.literals();
    for (int i = 0; i <fileArgs.size();i++)
    {
        int literalIndex = argLiterals.indexOf(fileArgs[i].literal);
        if (literalIndex != -1)
            argLiterals.replace(literalIndex, m_remoteProjectPath + "/"+ fileArgs[i].literal);
        fileArgs[i].literal = m_remoteProjectPath + "/"+fileArgs[i].literal;

        if (fileArgs[i].operation == NCSCommandFileArgument::UploadBeforeExecution)
            m_uploadArguments.append(fileArgs[i]);
        if (fileArgs[i].operation == NCSCommandFileArgument::DownloadAfterExecution)
            m_downloadArguments.append(fileArgs[i]);

    }

    m_application = " applications/" + application  + "/" + application;

    m_applicationArguments = argLiterals;

    m_currentApplication = new NCSRemoteApplicationBridge(application);
    m_executeNextPush();

}
void NCSRemoteCommandBridge::launchApplication(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile )
{
    if (!m_valid)
        return;

    QVector<NCSCommandFileArgument> fileArgs = arguments.fileArguments();
    QStringList argLiterals = arguments.literals();
    m_downloadArguments.clear();
    m_uploadArguments.clear();
    QString remoteHostfile = m_remoteProjectPath + "/temp_hostfile.ncv";
    m_uploadArguments.append(NCSCommandFileArgument(remoteHostfile,hostFile,NCSCommandFileArgument::UploadBeforeExecution));

    for (int i = 0; i <fileArgs.size();i++)
    {
        int literalIndex = argLiterals.indexOf(fileArgs[i].literal);
        if (literalIndex != -1)
            argLiterals.replace(literalIndex, m_remoteProjectPath + "/"+ fileArgs[i].literal);
        fileArgs[i].literal = m_remoteProjectPath + "/"+fileArgs[i].literal;

        if (fileArgs[i].operation == NCSCommandFileArgument::UploadBeforeExecution)
            m_uploadArguments.append(fileArgs[i]);
        if (fileArgs[i].operation == NCSCommandFileArgument::DownloadAfterExecution)
            m_downloadArguments.append(fileArgs[i]);

    }

    m_application = "mpirun";
    m_applicationArguments.clear();
    m_applicationArguments << "--np" << QString::number(numProcesses);
    if (hostFile != "")
        m_applicationArguments << "--hostfile" << remoteHostfile ;
    m_applicationArguments << " applications/" + application  + "/" + application;

    for (int i=0; i < argLiterals.size();i++)
        m_applicationArguments.append(argLiterals[i]);

    m_currentApplication = new NCSRemoteApplicationBridge(application);
    connect(m_currentApplication,SIGNAL(executionFinished()),this,SLOT(m_clearApplicationContext()));
    m_executeNextPush();

}
bool NCSRemoteCommandBridge::valid()
{
    return m_valid;
}


void NCSRemoteCommandBridge::m_onSocketCloned(QSshSocket * applicationSocket)
{
    qDebug() << "cloned!!!";
    m_currentApplication->setSocket(applicationSocket);
    m_currentApplication->start(m_application,m_applicationArguments,m_downloadArguments);
    applicationStarted(m_currentApplication);
}

void NCSRemoteCommandBridge::m_onCommandExecuted(QString command,QString response)
{
    qDebug() << command << " "<<response;
    if (command == "type -P mpirun &>/dev/null && echo 'exists'")
    {
        if (response == "exists")
            m_socket->executeCommand("[ -d " + m_remoteRootPath +" ] && echo 'exists'" );
        else
            validationError(NCSCommandBridge::MissingMPI);
    }
    else if (command == "[ -d " + m_remoteRootPath +" ] && echo 'exists'")
    {
        if (response == "exists")
            m_socket->executeCommand("[ -d " + m_remoteBuildPath + " ] && echo 'exists'");
        else
            validationError(NCSCommandBridge::MissingRootDirectory);
    }
    else if (command == "[ -d " + m_remoteBuildPath + " ] && echo 'exists'")
    {
        if (response == "exists")
            m_socket->executeCommand("[ -d " + m_remoteBuildPath + "/applications ] && echo 'exists'");
        else
            validationError(NCSCommandBridge::MissingBuildDirectory);
    }
    else if (command == "[ -d " + m_remoteBuildPath + "/applications ] && echo 'exists'")
    {
        if (response == "exists")
            m_socket->executeCommand("[ -d " + m_remoteBuildPath +"/plugins ] && echo 'exists'");
        else
            validationError(NCSCommandBridge::MissingApplicationDirectory);
    }
    else if (command == "[ -d " + m_remoteBuildPath +"/plugins ] && echo 'exists'")
    {
        if (response == "exists")
            m_socket->setWorkingDirectory(m_remoteBuildPath);

        else
            validationError(NCSCommandBridge::MissingPluginDirectory);
    }
    else if (command == "[ -d ./tmp-project ] && echo 'exists'")
    {
        if (response == "exists")
            m_socket->executeCommand("rm -r tmp-project");
        else
            m_socket->executeCommand("mkdir tmp-project");
    }
    else if (command == "rm -r tmp-project")
    {
        m_socket->executeCommand("mkdir tmp-project");
    }
    else if (command == "mkdir tmp-project")
    {
        m_valid = true;
        validated();
    }
}

void NCSRemoteCommandBridge::m_socketDirectorySet(QString dir)
{
    if (dir == m_remoteBuildPath)
        m_socket->executeCommand("[ -d ./tmp-project ] && echo 'exists'");
}

void NCSRemoteCommandBridge::m_clearApplicationContext()
{
    m_application = "";
    m_applicationArguments.clear();
    m_downloadArguments.clear();
    m_uploadArguments.clear();
}

void NCSRemoteCommandBridge::m_clearProjectContext()
{

    m_projectPath = "";
    m_remoteRootPath = "";
    m_remoteBuildPath = "";
    m_remoteProjectPath = "";
    m_valid = false;
    m_clearApplicationContext();
}

void NCSRemoteCommandBridge::m_onSocketError(QSshSocket::SshError err)
{
    if (err == QSshSocket::CloneError)
    {
        qDebug() << "not cloned!!!";
        m_currentApplication->setSocket(m_socket);
        m_currentApplication->start(m_application,m_applicationArguments,m_downloadArguments);
        applicationStarted(m_currentApplication);
    }
    else
        m_clearProjectContext();

}
void NCSRemoteCommandBridge::m_executeNextPush()
{
    if (m_uploadArguments.size() > 0)
    {
        NCSCommandFileArgument arg = m_uploadArguments[0];
        m_uploadArguments.pop_front();
        m_socket->pushFile(arg.filename,arg.literal);
    }
    else
    {
        m_socket->clone();
    }
}

QString NCSRemoteCommandBridge::hostname()
{
    return m_socket->host();
}
