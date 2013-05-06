#include "ncscommandfileargument.h"

NCSCommandFileArgument::NCSCommandFileArgument()
{
    m_argument = "";
    m_localFile = "";
    m_operation = NoSync;
}

NCSCommandFileArgument::NCSCommandFileArgument(QString argument)
{
    m_argument = argument;
    m_localFile = "";
    m_operation = NoSync;
}
NCSCommandFileArgument::NCSCommandFileArgument(QString argument,QString localFile,SyncOperation op)
{
    m_argument = argument;
    m_localFile = localFile;
    m_operation = op;
}

NCSCommandFileArgument::NCSCommandFileArgument(QString argument,QString localFile,SyncOperation op, QVector<NCSCommandFileArgument> dependencies)
{
    m_argument = argument;
    m_localFile = localFile;
    m_operation = op;
    m_dependencies = dependencies;
}

QString NCSCommandFileArgument::argument()
{
    return m_argument;
}

NCSCommandFileArgument::SyncOperation NCSCommandFileArgument::syncOperation()
{
    return m_operation;
}

void NCSCommandFileArgument::syncWith(QString localFile, SyncOperation op)
{
    m_localFile = localFile;
    m_operation = op;
}
void NCSCommandFileArgument::addDependency(NCSCommandFileArgument dependency)
{
    m_dependencies.append(dependency);
}


void NCSCommandFileArgument::setDependencies(QVector<NCSCommandFileArgument> dependencies)
{
    m_dependencies = dependencies;
}

void NCSCommandFileArgument::setArgument(QString argument)
{
    m_argument = argument;
}

QString NCSCommandFileArgument::localSyncFile()
{
    return m_localFile;
}

QVector<NCSCommandFileArgument> NCSCommandFileArgument::fileDependencies()
{
    return m_dependencies;
}
