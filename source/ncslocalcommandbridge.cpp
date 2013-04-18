#include "ncslocalcommandbridge.h"

NCSLocalApplicationBridge::NCSLocalApplicationBridge(QString workingDirectory, QObject *parent):NCSApplicationBridge(parent)
{
    m_process = new QProcess(this);
    if (workingDirectory != "")
        m_process->setWorkingDirectory(workingDirectory);
    connect(m_process,SIGNAL(readyReadStandardOutput()),this,SIGNAL(readyReadStandardOutput()));
    connect(m_process,SIGNAL(readyReadStandardError()),this,SIGNAL(readyReadStandardError()));
    connect(m_process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(m_processErrorOccured(QProcess::ProcessError)));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(m_processFinished()));
}
NCSLocalApplicationBridge::~NCSLocalApplicationBridge()
{
    m_process->disconnect();
    m_process = NULL;
}


void NCSLocalApplicationBridge::start(QString application,QStringList arguments)
{
    m_process->start(application,arguments);
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


NCSApplicationBridge * NCSLocalCommandBridge::executeApplication(QString application, NCSCommandArguments arguments)
{
    if (!m_valid)
    {
        return NULL;
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

    m_applicationBridge = new NCSLocalApplicationBridge(m_buildPath);
    m_applicationBridge->start("applications/" + application  + "/" + application,argLiterals);
    return m_applicationBridge;
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

NCSApplicationBridge * NCSLocalCommandBridge::executeApplication(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile )
{

    if (!m_valid)
    {
        return NULL;
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

    m_applicationBridge = new NCSLocalApplicationBridge(m_buildPath);
    m_applicationBridge->start("mpirun",completeArgs);
    return m_applicationBridge;
}



void NCSLocalCommandBridge::m_invalidate(NCSCommandBridge::ValidationError err)
{
    m_valid = false;
    validationError(err);
}
