#ifndef NCSCOMMANDBRIDGEVALIDATION_H
#define NCSCOMMANDBRIDGEVALIDATION_H

/*! \brief The NCS installation could not be validated.*/
enum NCSCommandBridgeValidationError
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

#endif // NCSCOMMANDBRIDGEVALIDATION_H
