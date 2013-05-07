#include "ncslocalcommandbridge.h"
#include <QMessageBox>

NCSLocalApplicationBridge::NCSLocalApplicationBridge(QString name,QString workingDirectory, QObject *parent):NCSApplicationBridge(parent)
{
    m_destroyProcess = true;
    m_name = name;
    m_process = new QProcess();
    m_runProcessName = "";
    if (workingDirectory != "")
        m_process->setWorkingDirectory(workingDirectory);
    connect(m_process,SIGNAL(readyReadStandardOutput()),this,SIGNAL(readyReadStandardOutput()));
    connect(m_process,SIGNAL(readyReadStandardError()),this,SIGNAL(readyReadStandardError()));
    connect(m_process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(m_processErrorOccured(QProcess::ProcessError)));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(m_processFinished()));
}

void NCSLocalApplicationBridge::scheduleDestruction(bool destroy)
{
    m_destroyProcess = destroy;
    executionFinished();
}

QString NCSLocalApplicationBridge::applicationName()
{
   return m_name;
}

NCSLocalApplicationBridge::~NCSLocalApplicationBridge()
{
    executionFinished();
    m_process->disconnect();
    if (m_destroyProcess)
    {
        m_process->kill();
        m_process->waitForFinished();

        QProcess checker;
        QStringList checkParams;
        checkParams << m_name;
        #ifdef Q_OS_LINUX
            checker.start("pidof",checkParams,QIODevice::ReadOnly);
            checker.waitForFinished(-1);

        #elif Q_OS_WINDOWS
            params.insert(0,"/v");
            params.insert(1,"/fo");
            params.insert(2,"csv");
            params.insert(3,"|");
            params.insert(4,"findstr");
            params.insert(5,"/i");
            checker.start("tasklist",checkParams,QIODevice::ReadOnly);
            checker.waitForFinished(-1);
        #endif

        QStringList pids = QString(checker.readAllStandardOutput()).split( " ");
        QString pidString = pids.first();
        pidString.replace("\r\n","");
        pidString.replace('\n',"");
        if (pidString != "")
        {
            QProcess killer;
            QStringList killParams;
            killParams << pidString;
            #ifdef Q_OS_LINUX
                killer.start("kill",killParams,QIODevice::ReadOnly);
                killer.waitForFinished(-1);

            #elif Q_OS_WINDOWS
                killParams.insert(0,"-f");
                killParams.insert(1,"/PID");
                killer.start("taskkill",killParams,QIODevice::ReadOnly);
                killer.waitForFinished(-1);
            #endif
        }


    }
}

void NCSLocalApplicationBridge::start(QString applicationPath,QStringList arguments)
{
    m_runProcessName = applicationPath;
    m_process->start(applicationPath,arguments);
}

QString NCSLocalApplicationBridge::readAllStandardError()
{
    return m_process->readAllStandardError();
}

QString NCSLocalApplicationBridge::readAllStandardOutput()
{
    return m_process->readAllStandardOutput();
}

void NCSLocalApplicationBridge::m_processErrorOccured(QProcess::ProcessError err)
{
    if (err == QProcess::UnknownError)
        executionError(NCSApplicationBridge::UnknownError);
    else
        executionError((NCSApplicationBridge::ApplicationError)err);
}

void NCSLocalApplicationBridge::m_processFinished()
{
    executionFinished();
}

NCSLocalCommandBridge::NCSLocalCommandBridge(QObject *parent):NCSCommandBridge(parent)
{
    m_rootPath = "";
    m_valid = false;
    m_applicationBridge = NULL;
}
void NCSLocalCommandBridge::initialize(QString projectPath)
{
    m_projectPath = projectPath;
}

void NCSLocalCommandBridge::validate(QString path)
{
    m_rootPath = path;
    m_buildPath = m_rootPath + "/build";
    m_valid = true;

    // attempt to find mpi
    QProcess mpi;
    mpi.start("mpirun");
    if (!mpi.waitForStarted())
        m_invalidate(NCSCommandBridge::MissingMPI);
    mpi.close();

    // make sure directory supplied exists
    QDir ncsDirectory(m_rootPath);
    if (!ncsDirectory.exists())
        m_invalidate(NCSCommandBridge::MissingRootDirectory);
    else
    {
        // make sure ncs has build directory and application directory
        if (!QDir(m_buildPath).exists())
            m_invalidate(NCSCommandBridge::MissingBuildDirectory);
        else if (!QDir(m_buildPath + "/applications").exists())
            m_invalidate(NCSCommandBridge::MissingApplicationDirectory);
        else if (!QDir(m_buildPath + "/plugins").exists())
            m_invalidate(NCSCommandBridge::MissingPluginDirectory);

    }
    if (m_valid == true)
        validated();
}

bool NCSLocalCommandBridge::valid()
{
    return m_valid;
}


void NCSLocalCommandBridge::executeApplication(QString application, NCSCommandArguments arguments)
{
    if (!m_valid)
    {
        return;
    }

    QVector<NCSCommandFileArgument> simFileArgs = arguments.fileArguments();
    QStringList argLiterals = arguments.literals();
    for (int i = 0; i <simFileArgs.size();i++)
    {
        int literalIndex = argLiterals.indexOf(simFileArgs[i].argument());
        if (literalIndex != -1)
        {
            if (simFileArgs[i].localSyncFile() != "")
                argLiterals.replace(literalIndex, simFileArgs[i].localSyncFile());
            else
                argLiterals.replace(literalIndex, simFileArgs[i].argument());
        }
    }

    m_applicationBridge = new NCSLocalApplicationBridge(application,m_buildPath,this);
    m_applicationBridge->start("applications/" + application  + "/" + application,argLiterals);
    applicationStarted(m_applicationBridge);
}

bool NCSLocalCommandBridge::m_transfer(QString sourcePath, QString destPath)
{
    QFile sourceFile(sourcePath);
    QFile destFile(destPath);
    if (sourceFile.exists())
    {
        if (destFile.exists())
            if (!destFile.remove())
                return false;

        if (sourceFile.copy(destPath))
            return true;
        else
            return false;
    }
    else
        return false;
}

void NCSLocalCommandBridge::executeApplication(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile )
{

    if (!m_valid)
    {
        return ;
    }

    QVector<NCSCommandFileArgument> simFileArgs = arguments.fileArguments();
    QStringList argLiterals = arguments.literals();
    for (int i = 0; i <simFileArgs.size();i++)
    {
        int literalIndex = argLiterals.indexOf(simFileArgs[i].argument());
        if (literalIndex != -1)
        {
            if (simFileArgs[i].localSyncFile() != "")
                argLiterals.replace(literalIndex, simFileArgs[i].localSyncFile());
            else
                argLiterals.replace(literalIndex, simFileArgs[i].argument());
        }
    }

    QStringList completeArgs;
    completeArgs << "--np" << QString::number(numProcesses);
    if (hostFile != "")
        completeArgs <<"--hostfile" << hostFile ;
    completeArgs << ("applications/" + application  + "/" + application)<< argLiterals;

    m_applicationBridge = new NCSLocalApplicationBridge(application,m_buildPath,this);
    m_applicationBridge->start("mpirun",completeArgs);
    applicationStarted(m_applicationBridge);
}

QString NCSLocalCommandBridge::hostname()
{
    return "localhost";
}

void NCSLocalCommandBridge::m_invalidate(NCSCommandBridge::ValidationError err)
{
    m_valid = false;
    validationError(err);
}
