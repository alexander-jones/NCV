#ifndef QGLXDynamicFrameBufferObject_H
#define QGLXDynamicFrameBufferObject_H

#include "qglxcore.h"
#include <QGLWidget>
#include <QImage>
#include <algorithm>
#include <QMap>
/*!
    \struct FrameBufferTarget
    \author Alex Jones
    \brief Container for all neccessary information used to manipulate this target in opengl.
*/
struct FrameBufferTarget
{
    /*! \brief The identifier used by opengl for this target.*/
    GLuint id;

    /*! \brief The per pixel data type used by this target. (GL_FLOAT, GL_INT, GL_UNSIGNED_INT,...) */

    GLenum pixelType;

    /*! \brief The per pixel format used by this target. (GL_RGB,GL_RGBA,GL_RED,GL_RED_INTEGER,...)*/
    GLenum pixelFormat;

    /*! \brief The per pixel precision format used by this target. (GL_RGB8,GL_RGBA12,GL_RED16,GL_RED32UI,...)*/
    GLenum internalFormat;

    /*! \brief The attachment slot this target can be bound to for rendering. (GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT15,GL_DEPTH_ATTACHMENT,...)*/
    GLenum attachment;

    /*! \brief The type of this target. (GL_RENDERBUFFER,GL_TEXTURE_2D)*/
    GLenum targetType ;

    /*! \brief The width of the target.*/
    int width;

    /*! \brief The height of the target.*/
    int height;
};

/*!
    \class QGLXDynamicFrameBufferObject
    \author Alex Jones
    \brief Enables rendering of multiple targets simultaneously.In addition, this can manage targets for multiple passes.
    \note Implemented as Qt 4.8 does not implement a multiple target frame buffer. See \htmlonly<a href="http://qt-project.org/doc/qt-4.8/qglframebufferobject.html"> QGLFrameBufferObject</a>  \endhtmlonly
*/
class QGLXDynamicFrameBufferObject
{
public:

    //! \brief The constructor.
    QGLXDynamicFrameBufferObject();


    /*!
        \param name The name of the target to bind. (Will be used as identifier for target)
        \param width The width of the target.
        \param height The height of the target.
        \param internalFormat An OpenGL constant defining the internal format of the target.
        (GL_RGB8, GL_RGBA16, GL_DEPTH_COMPONENT32,...)
        \param attachment The attachment slot of this target.
        (GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT15, GL_DEPTH_ATTACHMENT,...)
        \param targetType The type of target to create
        (GL_RENDERBUFFER, GL_TEXTURE_2D)

        \brief This function creates a target and adds it to the list of targets.
    */
    void addTarget(QString name, int width, int height, GLenum internalFormat, GLenum attachment, GLenum targetType);


    /*!
        \param num The number of targets to bind
        \param names A pointer to an rray of target names;

        \brief This function binds up to 16 color targets in addition to one depth target.
        All targets being bound must have been created previously with AddTarget.
        In addition, no two targets being bound may occupy the same attachment slot.
        It is assumed that a shader program is already bound.
        \note Once created, the depth target does not have to be bound.
    */
    void bindTargets(int num, QString * names);

    /*!
        \param name The name of the target to get info about.
        \warning NOT WORKING
        \brief This returns a QImage containing a target's contents.
        This will only work if the target associated with name was created with
        a target type 'GL_TEXTURE_2D'
    */
    QImage getTargetAsImage(QString name);

    /*!
        \param num The number of targets to get info about.
        \param names The names of the targets to get info about.
        \brief This returns an array of FrameBufferTargets that describe target properties.
    */
    FrameBufferTarget * getTargets(int num, QString * names);

    /*!
        \param name The indentifier of the target to read pixels from.
        \param rect The area of pixels to pull from indicated target.
        \brief This function returns a section of pixel values from the specified target.
    */
    void getTargetPixels(QString name,QRect area, void * data);

    /*!
        \brief This function initializes the frame buffer.
        Must be called after OpenGL Context is created.
    */
    void init();

    /*!
        \brief This function releases all targets bound previously by BindTargets.
    */
    void releaseTargets();

    /*!
        \param name The name of the target to delete.

        \brief If present in the list of targets, this function deletes the target associated with name.
    */
    void removeTarget(QString name);

    /*!
        \param num The number of targets to size
        \param names The identifiers of every target being sized.
        \param width The width of each target should be set to.
        \param height The height each target should be set to.

        \brief This function resizes num targets to (width,height)
    */
    void sizeTargets(int num, QString * names,int width, int height);

    /*!
        \param name The name of the target to blit on screen.
        \param to The area on screen the target will blit onto.

        \brief This function blits a target onto the back buffer.
        This function cannot be called between BindTargets and ReleaseTargets.
    */
    void targetToScreen(QString name, QRect from, QRect to);



private:

    GLuint  m_fbo;
    bool m_depthEnabled;
    QMap<QString,FrameBufferTarget>  m_targets;
};

#endif // QGLXDynamicFrameBufferObject_H
