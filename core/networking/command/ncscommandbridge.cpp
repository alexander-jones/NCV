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

bool NCSCommandFileArgument::operator == (const NCSCommandFileArgument & arg)
{
    return literal == arg.literal && dependencies == arg.dependencies;
}

NCSCommandArguments::NCSCommandArguments(){}

void NCSCommandArguments::append(const NCSCommandFileArgument & arg)
{
    insert(count(),arg);
}

void NCSCommandArguments::append(const QString & literal)
{
    m_literals.append(literal);
}

int NCSCommandArguments::count()
{
    return m_literals.count();
}

void NCSCommandArguments::insert(int index,const NCSCommandFileArgument & arg )
{
    m_literals.insert(index,arg.literal);
    int fileArgIndex = m_fileArguments.count();
    if (index != 0)
        for(int i = 0; i < m_fileArguments.size(); i++)
        {
            if (m_fileArguments[i].literal == m_literals[index -1]);
            fileArgIndex = i;
            break;
        }
    else
        fileArgIndex = 0;

    m_insertToFileArguments(fileArgIndex,arg);
}

void NCSCommandArguments::insert(int index,const QString & literal)
{
    m_literals.insert(index,literal);
}

int NCSCommandArguments::indexOf(const QString & literal)
{
    return m_literals.indexOf(literal);
}

int NCSCommandArguments::indexOf(const NCSCommandFileArgument & arg )
{
    return m_literals.indexOf(arg.literal);
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

void NCSCommandArguments::m_insertToFileArguments(int index, NCSCommandFileArgument arg)
{
    for (int i = 0; i < arg.dependencies.size(); i ++)
        m_insertToFileArguments(index + i, arg.dependencies[i]);
    m_fileArguments.insert(index + arg.dependencies.size(), arg);
}
