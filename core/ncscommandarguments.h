#ifndef NCSCOMMANDARGUMENTS_H
#define NCSCOMMANDARGUMENTS_H
#include <QString>
#include <QProcess>
#include <QVector>
#include <QDir>
#include <QTimer>
#include <QHostAddress>

/*!
    \struct NCSCommandFileArgument
    \author Alex Jones
    \brief A file argument which needs to be synced with a local file before or after interacting with an NCS application.
*/
struct NCSCommandFileArgument
{
    /*! \brief This enum describes how this file argument should be synced with a local copy.*/
    enum SyncOperation
    {
        /*! \brief Sync the local copy with the file argument before execution of an NCS application.*/
        UploadBeforeExecution,
        /*! \brief Sync the local copy with the file argument after execution of an NCS application.*/
        DownloadAfterExecution
    };

    /*!
        \brief A constructor. That is all.
    */
    NCSCommandFileArgument();

    /*!
        \param literal The literal used to refer to this file when executing an NCS application.
        \param filename The filename of a local copy to sync with the literal.
        \param operation The fashion in which in the file named filename will be synced with literal.
        \brief A constructor. That is all.
    */
    NCSCommandFileArgument(QString literal, QString filename, SyncOperation operation);

    /*!
        \param literal The literal used to refer to this file when executing an NCS application.
        \param filename The filename of a local copy to sync with the literal.
        \param operation The fashion in which in the file named filename will be synced with literal.
        \param dependencies Any other file dependencies that this file argument may need. These dependencies
        will not be passed to the NCS application but simply indicate that this file argument references these dependencies internally.
        \brief A constructor. That is all.
    */
    NCSCommandFileArgument(QString literal, QString filename, SyncOperation operation, QVector<NCSCommandFileArgument> dependencies);

    /*!
        \param arg The command file argument to test against.
        \brief Returns whether or not this file argument has the same literal and same dependencies as arg.
    */
    bool operator == (const NCSCommandFileArgument & arg);

    /*! \brief The literal used to refer to this file when executing an NCS application.*/
    QString literal;
    /*! \brief The filename of a local copy to sync with the literal.*/
    QString filename;
    /*! \brief The fashion in which in the file named filename will be synced with literal.*/
    SyncOperation operation;
    /*!
        \brief Any other file dependencies that this file argument may need. These dependencies
        will not be passed to the NCS application but simply indicate that this file argument references these dependencies internally.
    */
    QVector<NCSCommandFileArgument> dependencies;
};

/*!
    \class NCSCommandArguments
    \author Alex Jones
    \brief A set of arguments given to an NCS application.
*/
class NCSCommandArguments
{
public:

    /*!
        \brief A constructor. That is all.
    */
    NCSCommandArguments();

    /*!
        \param arg A command file argument to be appended.
        \brief Appends the file argument to the list of file arguments and appends the file argument's literal to the list of literals .
    */
    void append(const NCSCommandFileArgument & arg);
    /*!
        \param arg A literal command argument to be appended.
        \brief Appends literal to the list of literals .
    */
    void append(const QString & literal);

    /*!
        \brief Returns the number of argument literals contained.
    */
    int count();

    /*!
        \param index The literal index to insert the command argument into.
        \param arg The command file argument to insert.
        \brief Insert a command file argument into the list of literals and command file arguments.
    */
    void insert(int index,const NCSCommandFileArgument & arg );

    /*!
        \param index The literal index to insert the argument literal into.
        \param literal The argument literal to insert.
        \brief Insert a argument literal into the list of literals.
    */
    void insert(int index,const QString & literal);

    /*!
        \param literal The argument literal to check.
        \brief Returns the index of the argument literal in the list of literals.
    */
    int indexOf(const QString & literal);

    /*!
        \param arg The command file argument to check.
        \brief Returns the index of the argument in the list of literals.
    */
    int indexOf(const NCSCommandFileArgument & arg );

    /*!
        \param arg A command file argument to be appended.
        \brief Appends the file argument to the list of file arguments and appends the file argument's literal to the list of literals .
    */
    NCSCommandArguments& operator<<(const NCSCommandFileArgument & arg);

    /*!
        \param arg A literal command argument to be appended.
        \brief Appends literal to the list of literals .
    */
    NCSCommandArguments& operator<<(const QString & literal);

    /*!
        \brief Returns all the literals in a string list.
    */
    QStringList literals();

    /*!
        \brief Returns all the file arguments a vector.
    */
    QVector<NCSCommandFileArgument> fileArguments();

private:
    void m_insertToFileArguments(int index, NCSCommandFileArgument arg);
    void m_removeFromFileArguments(NCSCommandFileArgument arg);
    QStringList m_literals;
    QVector<NCSCommandFileArgument> m_fileArguments;

};
#endif // NCSCOMMANDARGUMENTS_H
