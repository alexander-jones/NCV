#include "ncsremotecommandbridge.h"

NCSRemoteCommandBridge::NCSRemoteCommandBridge( QSshSocket * socket,QObject *parent ):NCSInternalCommandBridge(parent)
{
    m_socket = NULL;
    m_clearProjectContext();
    if (m_socket != NULL)
        m_socket->disconnect();
    m_socket = socket;

    connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
    connect(m_socket,SIGNAL(workingDirectorySet(QString)),this,SLOT(m_socketDirectorySet(QString)));
    connect(m_socket,SIGNAL(cloned(QSshSocket*)),this,SLOT(m_onSocketCloned(QSshSocket*)));

}

void NCSRemoteCommandBridge::validate(QString path)
{
    m_remoteRootPath = path;
    m_remoteBuildPath = m_remoteRootPath + "/build";
    m_remoteProjectPath = "tmp-project";
    m_socket->executeCommand("type -P mpirun &>/dev/null && echo 'exists'");
}

void NCSRemoteCommandBridge::launchApplicationBridge(QString application, NCSCommandArguments arguments)
{
    if (!m_valid)
        return ;

    ApplicationContext context;
    context.applicationPath =" applications/" + application  + "/" + application;
    context.arguments = arguments;
    context.application = new NCSRemoteApplicationBridge(application,m_remoteProjectPath,this);
    m_launchingApplications.append(context);
    m_socket->clone();

}
void NCSRemoteCommandBridge::launchApplicationBridge(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile )
{
    if (!m_valid)
        return;

    arguments.insert(0,"--np");
    arguments.insert(1,QString::number(numProcesses));
    arguments.insert(2,"--hostfile");
    arguments.insert(3,NCSCommandFileArgument("temp_hostfile.ncv",hostFile,NCSCommandFileArgument::UploadBeforeExecution));
    arguments.insert(4,"applications/" + application  + "/" + application);

    ApplicationContext context;
    context.applicationPath = "mpirun";
    context.arguments = arguments;
    context.application = new NCSRemoteApplicationBridge(application,m_remoteProjectPath,this);
    m_launchingApplications.append(context);
    m_socket->clone();

}

void NCSRemoteCommandBridge::queryApplication(QString applicationName)
{
    m_applicationToquery =  queryContext(applicationName);
    if (m_socket != NULL)
    {
        disconnect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
        connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onApplicationQueried(QString,QString,QString)));
        m_socket->executeCommand("[ -f brainslug/build/applications/" + applicationName +"/"+ applicationName + " ] && echo 'exists' ");
    }
}

void NCSRemoteCommandBridge::queryPlugin(NCSCommandBridge::PluginType type,QString pluginName)
{

    m_pluginToquery =  queryContext(pluginName);
    QString pluginSubDir;
    if (type == NeuronPlugin)
        pluginSubDir = "neurons";
    else if (type == SynapsePlugin)
        pluginSubDir = "synapses";
    else
        pluginSubDir = "inputs";

    if (m_socket != NULL)
    {
        disconnect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
        connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onPluginQueried(QString,QString,QString)));
        m_socket->executeCommand("find brainslug/build/plugins/" + pluginSubDir + "/" + pluginName  + " -name '" + pluginName + "*.o");
    }
}

void NCSRemoteCommandBridge::queryReader(QString readerName)
{
    m_readerToquery =  queryContext(readerName);
    if (m_socket != NULL)
    {
        disconnect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
        connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onReaderQueried(QString,QString,QString)));
        m_socket->executeCommand("find brainslug/build/readers/" + readerName + " -name '" + readerName + "*.o");
    }
}


bool NCSRemoteCommandBridge::valid()
{
    return m_valid;
}


void NCSRemoteCommandBridge::m_onSocketCloned(QSshSocket * applicationSocket)
{
    qDebug() << "cloned!!!";
    NCSRemoteApplicationBridge * application = m_launchingApplications[0].application;
    application->setSocket(applicationSocket);
    applicationBridgeLaunched(application);
    application->start(m_launchingApplications[0].applicationPath,m_launchingApplications[0].arguments);
    m_launchingApplications.pop_front();
}

void NCSRemoteCommandBridge::m_onApplicationQueried(QString command,QString stdOut,QString stdError)
{

    if (!m_applicationToquery.builtChecked)
    {
        m_applicationToquery.builtChecked = true;
        if ( stdOut.contains("exists"))
        {
            disconnect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onApplicationQueried(QString,QString,QString)));
            connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
            applicationQueried(m_applicationToquery.name,Ready);
        }
        else
            m_socket->executeCommand(command.replace("applications/","../applications/"));
    }
    else
    {
        disconnect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onApplicationQueried(QString,QString,QString)));
        connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));      
        if ( stdOut.contains("exists"))
            applicationQueried(m_applicationToquery.name,Unbuilt);
        else
            applicationQueried(m_applicationToquery.name,Missing);

    }

}

void NCSRemoteCommandBridge::m_onPluginQueried(QString command,QString stdOut,QString stdError)
{
    if (!m_pluginToquery.builtChecked)
    {
        m_pluginToquery.builtChecked = true;
        if ( stdOut.size() > 0)
        {
            disconnect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onPluginQueried(QString,QString,QString)));
            connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
            pluginQueried(m_pluginToquery.name,Ready);
        }
        else
            m_socket->executeCommand(command.replace("plugins/","../plugins/"));
    }
    else
    {
        disconnect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onPluginQueried(QString,QString,QString)));
        connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
        if ( stdOut.size() > 0)
            pluginQueried(m_pluginToquery.name,Unbuilt);
        else
            pluginQueried(m_pluginToquery.name,Missing);

    }


}

void NCSRemoteCommandBridge::m_onReaderQueried(QString command,QString stdOut,QString stdError)
{

    if (!m_readerToquery.builtChecked)
    {
        m_readerToquery.builtChecked = true;
        if ( stdOut.size() > 0)
        {
            disconnect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onReaderQueried(QString,QString,QString)));
            connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
            readerQueried(m_readerToquery.name,Ready);
        }

        else
            m_socket->executeCommand(command.replace("readers/","../readers/"));
    }
    else
    {
        disconnect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onReaderQueried(QString,QString,QString)));
        connect(m_socket,SIGNAL(commandExecuted(QString,QString,QString)),this,SLOT(m_onCommandExecuted(QString,QString,QString)));
        if ( stdOut.size() > 0)
            readerQueried(m_readerToquery.name,Unbuilt);
        else
            readerQueried(m_readerToquery.name,Missing);

    }

}


void NCSRemoteCommandBridge::m_onCommandExecuted(QString command,QString stdOut,QString stdError)
{
    qDebug() << command << " "<< stdOut << " " << stdError;

    stdOut.replace('\n',"");
    if (command == "type -P mpirun &>/dev/null && echo 'exists'")
    {
        if (stdOut == "exists")
            m_socket->executeCommand("[ -d " + m_remoteRootPath +" ] && echo 'exists'" );
        else
            validationError(NCSInternalCommandBridge::MissingMPI);
    }
    else if (command == "[ -d " + m_remoteRootPath +" ] && echo 'exists'")
    {
        if (stdOut == "exists")
            m_socket->executeCommand("[ -d " + m_remoteBuildPath + " ] && echo 'exists'");
        else
            validationError(NCSInternalCommandBridge::MissingRootDirectory);
    }
    else if (command == "[ -d " + m_remoteBuildPath + " ] && echo 'exists'")
    {
        if (stdOut == "exists")
            m_socket->executeCommand("[ -d " + m_remoteBuildPath + "/applications ] && echo 'exists'");
        else
            validationError(NCSInternalCommandBridge::MissingBuildDirectory);
    }
    else if (command == "[ -d " + m_remoteBuildPath + "/applications ] && echo 'exists'")
    {
        if (stdOut == "exists")
            m_socket->executeCommand("[ -d " + m_remoteBuildPath +"/plugins ] && echo 'exists'");
        else
            validationError(NCSInternalCommandBridge::MissingApplicationDirectory);
    }
    else if (command == "[ -d " + m_remoteBuildPath +"/plugins ] && echo 'exists'")
    {
        if (stdOut == "exists")
            m_socket->setWorkingDirectory(m_remoteBuildPath);

        else
            validationError(NCSInternalCommandBridge::MissingPluginDirectory);
    }
    else if (command == "[ -d ./tmp-project ] && echo 'exists'")
    {
        if (stdOut == "exists")
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


void NCSRemoteCommandBridge::m_clearProjectContext()
{
    m_remoteRootPath = "";
    m_remoteBuildPath = "";
    m_remoteProjectPath = "";
    m_valid = false;
}

void NCSRemoteCommandBridge::m_onSocketError(QSshSocket::SshError err)
{
    if (err == QSshSocket::CloneError)
    {
        qDebug() << "not cloned!!!";
        NCSRemoteApplicationBridge * application = m_launchingApplications[0].application;
        application->setSocket(m_socket);
        applicationBridgeLaunched(application);
        application->start(m_launchingApplications[0].applicationPath,m_launchingApplications[0].arguments);
        m_launchingApplications.pop_front();
    }
    else
        m_clearProjectContext();

}

QString NCSRemoteCommandBridge::hostname()
{
    return m_socket->host();
}
