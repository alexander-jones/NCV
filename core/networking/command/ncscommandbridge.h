#ifndef NCSCOMMANDBRIDGE_H
#define NCSCOMMANDBRIDGE_H
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

/*!
    \class NCSApplicationBridge
    \author Alex Jones
    \brief An abstract bridge for communicating with an NCS application.
    This class can not be directly instantiated, but will be used as an interface to abstract interaction with derived classes.
    Derived classes must guarantee that multiple application bridges can be executed concurrently.
    NCSApplicationBridges cannot be started directly but are launched by a NCSCommanBridge derivation.
*/
class NCSApplicationBridge:public QObject
{
    Q_OBJECT
public:

    /*! \brief This enum describes potential errors that can occur when interacting with a NCS application.*/
    enum ApplicationError
    {
        /*! \brief The application failed to start.*/
        FailedToStart,
        /*! \brief The communication with the application timed out.*/
        Timedout,
        /*! \brief The application crashed.*/
        Crashed,
        /*! \brief There was an error reading output from the application.*/
        ReadError,
        /*! \brief There was an error writing data to the application.*/
        WriteError,
        /*! \brief There was an error writing data to the application.*/
        ContextInvalid,
        /*! \brief There was another instance of this application already running.
        Usually generated when the application can only be executed as a singleton.*/
        OtherApplicationRunning,
        /*! \brief There was an error uploading a file argument copy to the application installation.*/
        SyncUploadError,
        /*! \brief There was an error downloading a file argument copy from the application installation.*/
        SyncDownloadError,
        /*! \brief There was an unkown error that caused communication with the application to cease.*/
        UnknownError
    };

    /*!
        \param parent The QObject parent to tie this class instance to.
        \brief A constructor.
    */
    explicit NCSApplicationBridge(QObject * parent):QObject(parent){}

    /*!
        \brief Returns all error output recieved from this application.
        Monitor the readyReadStandardError signal to tell when this data is ready.
    */
    virtual QString readAllStandardError() = 0;

    /*!
        \brief Returns all standard output recieved from this application.
        Monitor the readyReadStandardOutput signal to tell when this data is ready.
    */
    virtual QString readAllStandardOutput() = 0;


    /*!
        \brief Returns the name of the NCS application associated with this class instance.
    */
    virtual QString applicationName() = 0;

signals:

    /*!
        \brief Emitted when the NCS application has started executing.
    */
    void executionStarted();

    /*!
        \param err The error that occured when interacting with the NCS application.
        \brief Emitted when there is an error interacting with the NCS application.
    */
    void executionError(NCSApplicationBridge::ApplicationError err);

    /*!
        \brief Emitted when the NCS application has finished executing.
    */
    void executionFinished();

    /*!
        \brief Emitted when the NCS application has returned standard output
    */
    void readyReadStandardOutput();

    /*!
        \brief Emitted when the NCS application has returned standard error
    */
    void readyReadStandardError();

private:

    /*!
        \param destroy Whether or not to destroy the NCS application process when this class instance is destructed.
        \brief This function exists so that the main application can terminate applications launched by plugins at exit.
        It will emit the executionFinished signal so that plugins can terminate the application instance.
    */
    virtual void stopExecution(bool destroy) = 0;
    friend class MainWindow;
};

/*!
    \class NCSCommandBridge
    \author Alex Jones
    \brief An abstract bridge for communicating with an NCS installation.
    This class can not be directly instantiated, but will be used as an interface to abstract interaction with derived classes.
*/
class NCSCommandBridge: public QObject
{
    Q_OBJECT
public:

    /*! \brief The NCS installation could not be validated.*/
    enum ValidationError
    {
        /*! \brief The NCS installation's host system does not contain a visible MPI installation.*/
        MissingMPI,
        /*! \brief The NCS installation path supplied can not be found.*/
        MissingRootDirectory,
        /*! \brief The NCS installation path supplied does not contain a build directory.*/
        MissingBuildDirectory,
        /*! \brief The NCS installation path supplied does not contain an application directory.*/
        MissingApplicationDirectory,
        /*! \brief The NCS installation path supplied does not contain a plugin directory.*/
        MissingPluginDirectory
    };

    /*!
        \param parent The QObject parent to tie this class instance to.
        \brief A constructor.
    */
    explicit NCSCommandBridge(QObject * parent):QObject(parent){}

    /*!
        \param path The path that contains the NCS installation.
        \brief Validates whether or not a proper NCS installation is contained at path.
    */
    virtual void validate(QString path) = 0;

    /*!
        \param application The name of the application to launch.
        Applications are launched at <installation root>/applications/<application name>/<application name>
        \param arguments to pass to the application executable.
        \brief Attempts to launch an NCS application with the specified arguments.
        When the application has started the signal applicationStarted will be emitted.
    */
    virtual void launchApplication(QString application, NCSCommandArguments arguments) = 0;

    /*!
        \param application The name of the application to launch.
        Applications are launched at <installation root>/applications/<application name>/<application name>
        \param arguments to pass to the application executable.
        \param numProcesses The number of processes this application executable is distributed accross.
        \param hostFile The MPI hostfile describing what machines the application executable is distributed across.
        \brief Attempts to launch an NCS application through MPI with the specified arguments.
        When the application has started the signal applicationStarted will be emitted.
    */
    virtual void launchApplication(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile = "" ) = 0;

    /*!
        \brief Returns whether or not the command bridge has been validated.
    */
    virtual bool valid() = 0;

    /*!
        \brief Returns the hostname of the host that contains the NCS installation.
    */
    virtual QString hostname() = 0;


signals:
    /*!
        \param app The application that was launched.
        \brief Emitted when a new application has been launched.
    */
    void applicationStarted(NCSApplicationBridge * app);

    /*!
        \brief Emitted when the NCS installation path supplied has been validated.
    */
    void validated();

    /*!
        \param err The name of the application to launch.
        \brief Emitted when the NCS installation path supplied could not be validated.
    */
    void validationError(NCSCommandBridge::ValidationError err);
};
#endif // NCSCOMMANDBRIDGE_H
