#ifndef NCSInternalCommandBridge_H
#define NCSInternalCommandBridge_H
#include "core/ncscommandbridge.h"

class NCSInternalCommandBridge : public NCSCommandBridge
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

    explicit NCSInternalCommandBridge(QObject *parent = 0):NCSCommandBridge(parent){}

    /*!
        \param path The path that contains the NCS installation.
        \brief Validates whether or not a proper NCS installation is contained at path.
    */
    virtual void validate(QString path) = 0;

    /*!
        \brief Returns whether or not the command bridge has been validated.
    */
    virtual bool valid() = 0;
    
signals:


    /*!
        \brief Emitted when the NCS installation path supplied has been validated.
    */
    void validated();

    /*!
        \param err The name of the application to launch.
        \brief Emitted when the NCS installation path supplied could not be validated.
    */
    void validationError(NCSInternalCommandBridge::ValidationError err);

};

#endif // NCSInternalCommandBridge_H
