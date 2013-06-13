#ifndef NCSAPPLICATIONBRIDGE_H
#define NCSAPPLICATIONBRIDGE_H

#include <QObject>
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

#endif // NCSAPPLICATIONBRIDGE_H
