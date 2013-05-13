#ifndef QGLXFRAMEBUFFEROBJECT_H
#define QGLXFRAMEBUFFEROBJECT_H
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
        \brief This function binds up to 16 color targets in addition to one depth target.
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
        \param attachment The frame buffer attachment slot to use in the currently bound framebuffer.
        \param offset The offset from the first attachment slot to use. Only applies to color attachments.
        \param from The area on the source to sample from.
        \param to The area on the screen the target will blit onto.
        \param destMode The screen buffer to draw to. i.e. GL_FRONT, GL_BACK, GL_LEFT, etc...
        \brief This function blits a target onto another target.
    */    
    void blitTexture( QGLXTexture2D& source,QGLXTexture2D::FrameBufferAttachment sourceAttachment,GLuint offset, QRect from,QRect to, GLenum destMode = GL_BACK );

    /*!
        \param source The name of the target to blit from.
        \param from The area on the source to sample from.
        \param dest The name of the target to blit to.
        \param to The area on the destination the target will blit onto.
        \brief This function blits a target onto the screen
    */
    void blitTexture( QGLXTexture2D& source,QRect from,  QGLXTexture2D& dest,QRect to );

    /*!
        \param num The number of color targets being rendered to.
        \brief This function enables rendering to multiple targets.
    */
    void enableColorAttachments(int num);

    /*!
        \param tex The texture to grab data from.
        \param area The area of the texture that will be sampled.
        \param data A pointer to the data source.
        \brief This function enables rendering to multiple targets.
    */
    void getTextureData(QGLXTexture2D tex,QRect area, void * data);

private:
    GLuint  m_fbo;

};

#endif // QGLXFRAMEBUFFEROBJECT_H
