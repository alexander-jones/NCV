#include "ncslocalapplicationbridge.h"

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

void NCSLocalApplicationBridge::stopExecution(bool destroy)
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

void NCSLocalApplicationBridge::start(QString applicationPath,NCSCommandArguments arguments)
{
    QVector<NCSCommandFileArgument> fileArgs = arguments.fileArguments();
    QStringList argLiterals = arguments.literals();
    for (int i = 0; i <fileArgs.size();i++)
    {
        int literalIndex = argLiterals.indexOf(fileArgs[i].literal);
        if (literalIndex != -1)
        {
            if (fileArgs[i].filename != "")
            {
                if (!QFileInfo(fileArgs[i].filename).isAbsolute())
                    fileArgs[i].filename == QDir::currentPath() + "/" + fileArgs[i].filename;
                argLiterals.replace(literalIndex, fileArgs[i].filename);
            }
        }
    }

    m_runProcessName = applicationPath;
    m_process->start(applicationPath,argLiterals);
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
