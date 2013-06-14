#ifndef NCSCOMMANDBRIDGE_H
#define NCSCOMMANDBRIDGE_H

#include "ncscommandarguments.h"
#include "ncsapplicationbridge.h"

class NCSCommandBridge: public QObject
{
    Q_OBJECT
public:

    /*! \brief The type of NCS plugin being Queried .*/
    enum PluginType
    {
        /*! \brief A NCS plugin for emulating neuron behavior.*/
        NeuronPlugin,
        /*! \brief A NCS plugin for emulating synapse behavior.*/
        SynapsePlugin,
        /*! \brief A NCS plugin for injecting stimulus.*/
        InputPlugin,
    };

    /*! \brief The status of a target being Queried.*/
    enum QueryResult
    {
        /*! \brief The target of the query was missing.*/
        Missing,
        /*! \brief The source code for the query's target was found, but it was not built.*/
        Unbuilt,
        /*! \brief The target of the query was found and built.*/
        Ready
    };


    /*!
        \param parent The QObject parent to tie this class instance to.
        \brief A constructor.
    */
    explicit NCSCommandBridge(QObject * parent):QObject(parent){}


    /*!
        \param applicationName The name of the application to check.
        \brief This function querys the status of an application in the NCS installation.
        When the status is aquired, the signal applicationQueried is emitted.
    */
    virtual void queryApplication(QString applicationName) = 0;


    /*!
        \param pluginName The name of the plugin to check.
        \brief This function querys the status of an plugin in the NCS installation.
        When the status is aquired, the signal pluginQueried is emitted.
    */
    virtual void queryPlugin(NCSCommandBridge::PluginType type,QString pluginName) = 0;

    /*!
        \param readerName The name of the reader to check.
        \brief This function querys the status of an reader in the NCS installation.
        When the status is aquired, the signal readerQueried is emitted.
    */
    virtual void queryReader(QString readerName) = 0;

    /*!
        \param application The name of the application to launch.
        Applications are launched at <installation root>/applications/<application name>/<application name>
        \param arguments to pass to the application executable.
        \brief Attempts to launch an NCS application with the specified arguments.
        When the application has started the signal applicationBridgeLaunched will be emitted.
    */
    virtual void launchApplicationBridge(QString application, NCSCommandArguments arguments) = 0;

    /*!
        \param application The name of the application to launch.
        Applications are launched at <installation root>/applications/<application name>/<application name>
        \param arguments to pass to the application executable.
        \param numProcesses The number of processes this application executable is distributed accross.
        \param hostFile The MPI hostfile describing what machines the application executable is distributed across.
        \brief Attempts to launch an NCS application through MPI with the specified arguments.
        When the application has started the signal applicationBridgeLaunched will be emitted.
    */
    virtual void launchApplicationBridge(QString application, NCSCommandArguments arguments,int numProcesses, QString hostFile = "" ) = 0;

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
        \param applicationName The name of the application to check.
        \param status The status of the application being Queried.
        \brief This signal is emmitted when the status of an NCS application has been queried.
    */
    void applicationQueried(QString applicationName,NCSCommandBridge::QueryResult status);

    /*!
        \param pluginName The name of the plugin to check.
        \param status The status of the plugin being Queried.
        \brief This signal is emmitted when the status of an NCS plugin has been queried.
    */
    void pluginQueried(QString pluginName,NCSCommandBridge::QueryResult status);

    /*!
        \param readerName The name of the reader to check.
        \param status The status of the reader being Queried.
        \brief This signal is emmitted when the status of an NCS reader has been queried.
    */
    void readerQueried(QString readerName,NCSCommandBridge::QueryResult status);


};
#endif // NCSCOMMANDBRIDGE_H
