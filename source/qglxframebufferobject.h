#ifndef QGLXFRAMEBUFFEROBJECT_H
#define QGLXFRAMEBUFFEROBJECT_H

#include "qglxcore.h"
#include "qglxtexture2d.h"
#include <QGLWidget>
#include <QImage>
#include <algorithm>
#include <QMap>


/*!
    \class QGLXFrameBufferObject
    \author Alex Jones
    \brief Expands QGLFrameBufferObject by allowing rendering of multiple targets simultaneously in addition to allowing pixel transfer functions.
    \note Implemented as Qt 4.8 does not implement a multiple target frame buffer. See \htmlonly<a href="http://qt-project.org/doc/qt-4.8/qglframebufferobject.html"> QGLFrameBufferObject</a>  \endhtmlonly
*/
class QGLXFrameBufferObject
{
public:

    //! \brief The constructor.
    QGLXFrameBufferObject();

    /*!
        \param num The number of targets to bind
        \param names A pointer to an rray of target names;

        \brief This function binds up to 16 color targets in addition to one depth
        All targets being bound must have been created previously with Add
        In addition, no two targets being bound may occupy the same attachment slot.
        It is assumed that a shader program is already bound.
        \note Once created, the depth target does not have to be bound.
    */
    bool bind();


    /*!
        \brief This function initializes the frame buffer.
        Must be called after OpenGL Context is created.
    */
    void create();

    /*!
        \brief This function releases all targets bound previously by BindTargets.
    */
    void release();


    /*!
        \param source The name of the target to blit from.
        \param from The area on the source to sample from.
        \param to The area on the screen the target will blit onto.
        \param destMode The screen buffer to draw to. i.e. GL_FRONT, GL_BACK, GL_LEFT, etc...

        \brief This function blits a target onto another target.
        This function cannot be called between BindTargets and ReleaseTargets.
    */
    void blitTarget( QGLXTexture2D& source,QRect from,QRect to, GLenum destMode = GL_BACK );

    /*!
        \param source The name of the target to blit from.
        \param from The area on the source to sample from.
        \param dest The name of the target to blit to.
        \param to The area on the destination the target will blit onto.

        \brief This function blits a target onto the screen
        This function cannot be called between BindTargets and ReleaseTargets.
    */
    void blitTarget( QGLXTexture2D& source,QRect from,  QGLXTexture2D& dest,QRect to );

    void enableColorAttachments(int num);

    void getTargetPixels(QGLXTexture2D tex,QRect area, void * data);

private:

    GLuint  m_fbo;
};

#endif // QGLXFRAMEBUFFEROBJECT_H
