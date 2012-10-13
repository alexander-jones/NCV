#ifndef MULTIPLETARGETFRAMEBUFFER_H
#define MULTIPLETARGETFRAMEBUFFER_H

#ifdef WIN32
    #pragma comment(lib,"glew32.lib")

#endif
#include <GL/glew.h>
#include <QGLWidget>
#include <QImage>
#include <algorithm>
#include <map>

struct FrameBufferTarget
{
    GLuint id;
    GLenum format, internalFormat,attachment, type ;
    int width;
    int height;
};

/*!
    \class MultipleTargetFrameBuffer
    \author Alex Jones
    \brief Enables simpler management of output targets.
    \note Implemented as no native Qt Implementation exists to bind multiple targets.
*/
class MultipleTargetFrameBuffer
{
public:

    //! \brief The constructor.
    MultipleTargetFrameBuffer();

    /*!
        \brief This function initializes the frame buffer.
        Must be called after OpenGL Context is created.
    */
    void Init();

    /*!
        \param num The number of targets to bind
        \param names A pointer to an rray of target names;

        \brief This function binds up to 16 color targets in addition to one depth target.
        All targets pointed at by names must have been created previously with AddTarget.
        In addition, no two targets may occupy the same attachment slot.
    */
    void BindTargets(int num, QString * names);


    /*!
        \brief This function releases all targets bound previously by BindTargets.
    */
    void ReleaseTargets();

    /*!
        \param name The name of the target to bind. (Will be used as identifier for target)
        \param internalFormat An OpenGL constant defining the internal format of the target.
        (GL_RGB8, GL_RGBA16, GL_DEPTH_COMPONENT32,...)
        \param width The width of the target.
        \param height The height of the target.
        \param attachment The attachment slot of this target.
        (GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT15, GL_DEPTH_ATTACHMENT,...)
        \param targetType The type of target to create
        (GL_RENDERBUFFER, GL_TEXTURE_2D)

        \brief This function creates a target and adds it to the list of targets.
    */
    void AddTarget(QString name, GLenum internalFormat, int width, int height,GLenum attachment, GLenum targetType);

    /*!
        \param name The name of the target to delete.

        \brief If present in the list of targets, this function deletes the target associated with name.
    */
    void RemoveTarget(QString name);


    /*!
        \param name The name of the target to get info about.

        \brief This returns a FrameBufferTarget that describes a target's properties.
    */
    FrameBufferTarget GetTargetInfo(QString name);

    /*!
        \param name The name of the target to blit on screen.
        \param to The area on screen the target will blit onto.

        \brief This function blits a target onto the back buffer.
        This function cannot be called between BindTargets and ReleaseTargets.
    */
    void TargetToScreen(QString name, QRect to);

    /*!
        \param width The width of each target should be set to.
        \param height The height each target should be set to.

        \brief This function resizes all targets to (width,height)
    */
    void SizeTargets(int width, int height);

    /*!
        \param name The name of the target to get info about.
        \warning NOT WORKING
        \brief This returns a QImage containing a target's contents.
        This will only work if the target associated with name was created with
        a target type 'GL_TEXTURE_2D'
    */
    QImage GetTarget(QString name);

private:
    GLuint  m_fbo;
    bool m_depthEnabled;
    std::map<QString,FrameBufferTarget>  m_targets;
};

#endif // MULTIPLETARGETFRAMEBUFFER_H
