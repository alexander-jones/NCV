#include "multipletargetframebuffer.h"
#include <QtDebug>


#include <qglframebufferobject.h>

#include <qlibrary.h>

#include <qimage.h>
extern QImage qt_gl_read_framebuffer(const QSize&, bool, bool);

MultipleTargetFrameBuffer::MultipleTargetFrameBuffer()
{
}

void MultipleTargetFrameBuffer::Init()
{
    m_depthEnabled = false;
    glGenFramebuffers(1, &m_fbo);
}


void MultipleTargetFrameBuffer::BindTargets(int num, QString * names)
{
    // bind all active render targets
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    GLenum * attachments = new GLenum[num];
    for (int i =0; i < num; i ++)
        attachments[i] = m_targets[names[i]].attachment;

    // schedule all render targets as outputs of fragment shader
    glDrawBuffers(num, (const GLenum *)attachments);

}
FrameBufferTarget MultipleTargetFrameBuffer::GetTargetInfo(QString name)
{
    return m_targets[name];
}

void MultipleTargetFrameBuffer::ReleaseTargets()
{
    // release current targets by binding default target
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MultipleTargetFrameBuffer::AddTarget(QString name, GLenum internalFormat, int width, int height,GLenum attachment, GLenum targetType)
{
        if (targetType != GL_TEXTURE_2D && targetType != GL_RENDERBUFFER)
        {
            qDebug() << "Target type parameter must be either GL_TEXTURE_2D or GL_RENDERBUFFER" ;
            return;
        }
        FrameBufferTarget target = FrameBufferTarget();
        target.height = height;
        target.width = width;
        target.internalFormat = internalFormat;
        target.type = targetType;
        target.attachment = attachment;

        // if parameters specified are invalid for a depth target exit.
        if (attachment == GL_DEPTH_ATTACHMENT && m_depthEnabled )
        {
            qDebug() << "Frame buffer target ' " << name << " ' was not specified with valid parameters. ";
            return;
        }


        // determine format based on internalFormat setting
        switch (internalFormat)
        {
            case GL_RGB4:
            case GL_RGB5:
            case GL_RGB8:
            case GL_RGB10:
            case GL_RGB8UI:
            case GL_RGB12:
            case GL_RGB16:
                target.format = GL_RGB;
                break;

            case GL_RGBA4:
            case GL_RGBA8:
            case GL_RGBA12:
            case GL_RGBA16:
                target.format = GL_RGBA;
                break;

            case GL_DEPTH_COMPONENT16:
            case GL_DEPTH_COMPONENT24:
            case GL_DEPTH_COMPONENT32:
            case GL_DEPTH_COMPONENT32F:
            case GL_DEPTH_COMPONENT32F_NV:
                target.format = GL_DEPTH_COMPONENT;
                m_depthEnabled = true;
                break;

            case GL_R8UI:
            case GL_R16UI:
            case GL_R32UI:
                target.format = GL_RED_INTEGER;
                break;

            default:
                return;
        }


        // if parameters specified are invalid for a texture exit.
        if ((target.format == GL_RGB || target.format == GL_RGBA)  && (attachment < GL_COLOR_ATTACHMENT0 || attachment > GL_COLOR_ATTACHMENT15 ))
        {
            qDebug() << "Frame buffer target ' " << name << " ' was not specified with valid parameters. ";
            return;
        }

        // if parameters specified are invalid for an integer buffer exit.
        if (target.format == GL_RED_INTEGER && (attachment < GL_COLOR_ATTACHMENT0 || attachment > GL_COLOR_ATTACHMENT15 ))
        {
            qDebug() << "Frame buffer target ' " << name << " ' was not specified with valid parameters. ";
            return;
        }

        // create a render target bound to the class frame buffer object
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);


        // If target specified as render buffer, initialize it and bind to fbo. (Renderbuffer = fas    ter/ better rendering but doesn't support cpu reads)
        if (target.type == GL_RENDERBUFFER)
        {
            glGenRenderbuffers(1,&target.id);
            glBindRenderbuffer(GL_RENDERBUFFER,target.id);
            glRenderbufferStorage(GL_RENDERBUFFER,target.internalFormat,target.width,target.height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,target.attachment,GL_RENDERBUFFER,target.id);
        }
        // If target specified as texture, initialize it and bind to fbo.
        else if (target.type == GL_TEXTURE_2D)
        {
            glGenTextures(1, &target.id);
            glBindTexture(GL_TEXTURE_2D, target.id);
            glTexImage2D(GL_TEXTURE_2D, 0, target.internalFormat, target.width, target.height, 0, target.format, GL_UNSIGNED_BYTE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, target.attachment, GL_TEXTURE_2D, target.id, 0);

        }

        // release frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_targets[name] = target;



    return;
}

void MultipleTargetFrameBuffer:: SizeTargets(int width, int height)
{
    FrameBufferTarget * target;
    for ( std::map<QString,FrameBufferTarget>::iterator it = m_targets.begin() ; it != m_targets.end(); it++ )
    {
        target = &(it->second);
        target->width = width;
        target->height = height;

        // Rebind texture change the attributes
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);


        if (target->type == GL_RENDERBUFFER)
        {
            glBindRenderbuffer(GL_RENDERBUFFER,target->id);
            glRenderbufferStorage(GL_RENDERBUFFER,target->internalFormat,target->width,target->height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,target->attachment,GL_RENDERBUFFER,target->id);
        }
        else if (target->type == GL_TEXTURE_2D)
        {
            glBindTexture(GL_TEXTURE_2D, target->id);
            glTexImage2D(GL_TEXTURE_2D, 0, target->internalFormat, target->width, target->height, 0, target->format, GL_UNSIGNED_BYTE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, target->attachment, GL_TEXTURE_2D, target->id, 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void MultipleTargetFrameBuffer::RemoveTarget(QString name)
{
    if (m_targets.count(name) >0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        if (m_targets[name].type == GL_RENDERBUFFER)
            glDeleteRenderbuffers(1,&m_targets[name].id);
        else if (m_targets[name].type == GL_TEXTURE_2D)
            glDeleteTextures(1,&m_targets[name].id);
        m_targets.erase(name);
    }

}
void MultipleTargetFrameBuffer::TargetToScreen(QString name, QRect to)
{
    if (m_targets.count(name) > 0)
    {
        // set draw buffer as default buffer (if it is already not for some reason)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // set our framebuffer object as read buffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

        // tell OpenGL to read this target and blit it onto the rectangle specified
        glReadBuffer(m_targets[name].attachment);
        glBlitFramebuffer(0, 0, m_targets[name].width, m_targets[name].height,
                          to.x(), to.y(), to.width(), to.height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

        // release this framebuffer from reading, exit to default
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }
}


QImage  MultipleTargetFrameBuffer::GetTarget(QString name)
{
    QImage image;

    if (m_targets.count(name) >0 && m_targets[name].type == GL_TEXTURE_2D)
    {

        // bind requested texture and read specified pixel
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
        glReadBuffer(m_targets[name].attachment);
        //glReadPixels();
        //image = qt_gl_read_framebuffer(QSize(m_targets[name].width, m_targets[name].height), m_targets[name].internalFormat != GL_RGB, true);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }
    return image;
}

