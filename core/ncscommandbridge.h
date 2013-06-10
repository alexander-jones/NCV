#ifndef NCSCOMMANDBRIDGE_H
#define NCSCOMMANDBRIDGE_H

#include "ncscommandarguments.h"

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

    /*! \brief The type of NCS plugin being probed .*/
    enum PluginType
    {
        /*! \brief A NCS plugin for emulating neuron behavior.*/
        NeuronPlugin,
        /*! \brief A NCS plugin for emulating synapse behavior.*/
        SynapsePlugin,
        /*! \brief A NCS plugin for injecting stimulus.*/
        InputPlugin,
    };

    /*! \brief The status of a target being probed.*/
    enum ProbeStatus
    {
        /*! \brief The target of the probe was missing.*/
        Missing,
        /*! \brief The source code for the probe's target was found, but it was not built.*/
        Unbuilt,
        /*! \brief The target of the probe was found and built.*/
        Ready
    };

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
        \param applicationName The name of the application to check.
        \brief This function probes the status of an application in the NCS installation.
        When the status is aquired, the signal applicationProbed is emitted.
    */
    virtual void probeApplication(QString applicationName) = 0;

    /*!
        \param pluginName The name of the plugin to check.
        \brief This function probes the status of an plugin in the NCS installation.
        When the status is aquired, the signal pluginProbed is emitted.
    */
    virtual void probePlugin(NCSCommandBridge::PluginType type,QString pluginName) = 0;

    /*!
        \param readerName The name of the reader to check.
        \brief This function probes the status of an reader in the NCS installation.
        When the status is aquired, the signal readerProbed is emitted.
    */
    virtual void probeReader(QString readerName) = 0;

    /*!
        \param application The name of the application to launch.
        Applications are launched at <installation root>/applications/<application name>/<application name>
        \param arguments to pass to the application executable.
        \brief Attempts to launch an NCS application with the specified arguments.
        When the application has started the signal applicationBridgeLaunched will be emitted.
    */
    virtual void launchApplication(QString application, NCSCommandArguments arguments) = 0;

    /*!
        \param application The name of the application to launch.
        Applications are launched at <installation root>/applications/<application name>/<application name>
        \param arguments to pass to the application executable.
        \param numProcesses The number of processes this application executable is distributed accross.
        \param hostFile The MPI hostfile describing what machines the application executable is distributed across.
        \brief Attempts to launch an NCS application through MPI with the specified arguments.
        When the application has started the signal applicationBridgeLaunched will be emitted.
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
    void applicationBridgeLaunched(NCSApplicationBridge * app);

    /*!
        \brief Emitted when the NCS installation path supplied has been validated.
    */
    void validated();

    /*!
        \param applicationName The name of the application to check.
        \param status The status of the application being probed.
        \brief This signal is emmitted when the status of an NCS application has been probed.
    */
    void applicationProbed(QString applicationName,NCSCommandBridge::ProbeStatus status);

    /*!
        \param applicationName The name of the plugin to check.
        \param status The status of the plugin being probed.
        \brief This signal is emmitted when the status of an NCS plugin has been probed.
    */
    void pluginProbed(QString pluginName,NCSCommandBridge::ProbeStatus status);

    /*!
        \param readerName The name of the reader to check.
        \param status The status of the reader being probed.
        \brief This signal is emmitted when the status of an NCS reader has been probed.
    */
    void readerProbed(QString readerName,NCSCommandBridge::ProbeStatus status);

    /*!
        \param err The name of the application to launch.
        \brief Emitted when the NCS installation path supplied could not be validated.
    */
    void validationError(NCSCommandBridge::ValidationError err);
};
#endif // NCSCOMMANDBRIDGE_H
