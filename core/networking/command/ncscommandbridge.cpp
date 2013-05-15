#include "ncscommandbridge.h"

NCSCommandFileArgument::NCSCommandFileArgument(){}

NCSCommandFileArgument::NCSCommandFileArgument(QString literal, QString filename, NCSCommandFileArgument::SyncOperation operation)
{
    this->literal = literal;
    this->filename = filename;
    this->operation = operation;
}

NCSCommandFileArgument::NCSCommandFileArgument(QString literal, QString filename, NCSCommandFileArgument::SyncOperation operation, QVector<NCSCommandFileArgument> dependencies)
{
    this->literal = literal;
    this->filename = filename;
    this->operation = operation;
    this->dependencies = dependencies;
}

NCSCommandArguments::NCSCommandArguments(){}

void NCSCommandArguments::append(NCSCommandFileArgument arg)
{
    m_literals.append(arg.literal);
    m_addToFileArguments(arg);
}

void NCSCommandArguments::append(const QString & literal)
{
    m_literals.append(literal);
}

NCSCommandArguments& NCSCommandArguments::operator<<(const NCSCommandFileArgument & arg)
{
    append(arg);
    return *this;
}

NCSCommandArguments& NCSCommandArguments::operator<<(const QString & literal)
{
    append(literal);
    return *this;
}


QStringList NCSCommandArguments::literals()
{
    return m_literals;
}

QVector<NCSCommandFileArgument> NCSCommandArguments::fileArguments()
{
    return m_fileArguments;
}

void NCSCommandArguments::m_addToFileArguments(NCSCommandFileArgument arg)
{
    for (int i = 0; i < arg.dependencies.size(); i ++)
        m_addToFileArguments(arg.dependencies[i]);
    m_fileArguments.append(arg);
}
