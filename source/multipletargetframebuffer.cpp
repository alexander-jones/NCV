#include "multipletargetframebuffer.h"
#include <QtDebug>

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
void MultipleTargetFrameBuffer::BindTarget(QString name)
{
    // bind all active render targets
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // schedule all rendeglDrawbuffersr targets as outputs of fragment shader
    glDrawBuffers(1, (const GLenum *)&m_targets[name].attachment);

}
FrameBufferTarget MultipleTargetFrameBuffer::GetTargetInfo(QString name)
{
    return m_targets[name];
}

void MultipleTargetFrameBuffer::Release()
{
    // release current targets by binding default target
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MultipleTargetFrameBuffer::AddTarget(QString name, GLenum dpi, int width, int height,GLenum attachment )
{
        FrameBufferTarget target = FrameBufferTarget();
        target.height = height;
        target.width = width;
        target.dpi = dpi;

        // determine format, other info based on dpi setting
        switch (dpi)
        {
            case GL_RGB4:
            case GL_RGB5:
            case GL_RGB8:
            case GL_RGB10:
            case GL_RGB8UI:
            case GL_RGB12:
            case GL_RGB16:
                if (attachment < GL_COLOR_ATTACHMENT0 || attachment > GL_COLOR_ATTACHMENT15)
                    return;
                else
                {
                    target.attachment = attachment;
                    target.format = GL_RGB;
                }
                break;


            case GL_RGBA4:
            case GL_RGBA8:
            case GL_RGBA12:
            case GL_RGBA16:
                if (attachment < GL_COLOR_ATTACHMENT0 || attachment > GL_COLOR_ATTACHMENT15)
                    return;
                else
                {
                    target.attachment = attachment;
                    target.format = GL_RGBA;
                }
                break;

            case GL_DEPTH_COMPONENT16:
            case GL_DEPTH_COMPONENT24:
            case GL_DEPTH_COMPONENT32:
            case GL_DEPTH_COMPONENT32F:
            case GL_DEPTH_COMPONENT32F_NV:
                if (!m_depthEnabled && attachment == GL_DEPTH_ATTACHMENT)
                {
                    target.format = GL_DEPTH_COMPONENT;
                    target.attachment = attachment;
                    m_depthEnabled = true;
                }
                else
                    return;
                break;

            case GL_ALPHA32UI_EXT:
                if (attachment < GL_COLOR_ATTACHMENT0 || attachment > GL_COLOR_ATTACHMENT15)
                    return;
                else
                {
                    target.attachment = attachment;
                    target.format = GL_ALPHA_INTEGER;
                }
                break;

        }


        // create a render target bound to the class frame buffer object
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // create and size texture used for render target
        glGenTextures(1, &target.texture);
        glBindTexture(GL_TEXTURE_2D, target.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, target.dpi, target.width, target.height, 0, target.format, GL_UNSIGNED_BYTE, 0);

        // connect texture to framebuffer attachment slot, then rebind default buffer to release this target
        glFramebufferTexture2D(GL_FRAMEBUFFER, target.attachment, GL_TEXTURE_2D, target.texture, 0);

        // release frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_targets[name] = target;



    return;
}

void MultipleTargetFrameBuffer:: SizeTargets(int width, int height)
{
    for ( std::map<QString,FrameBufferTarget>::iterator it = m_targets.begin() ; it != m_targets.end(); it++ )
    {
        it->second.width = width;
        it->second.height = height;

        // Rebind texture change the attributes
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glDeleteTextures(1, &it->second.texture);
        glGenTextures(1, &it->second.texture);
        glBindTexture(GL_TEXTURE_2D, it->second.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, it->second.dpi, it->second.width, it->second.height, 0, it->second.format, GL_UNSIGNED_BYTE, 0);

        // connect texture to framebuffer attachment slot, then rebind default buffer to release this target
        glFramebufferTexture2D(GL_FRAMEBUFFER, it->second.attachment, GL_TEXTURE_2D, it->second.texture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void MultipleTargetFrameBuffer::RemoveTarget(QString name)
{
    if (m_targets.count(name) >0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glDeleteTextures(1,&m_targets[name].texture);
        m_targets.erase(name);
    }

}
void MultipleTargetFrameBuffer::TargetToScreen(QString name, int x, int y, int width, int height)
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
                        x, y, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        // release this framebuffer from reading, exit to default
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }
}


QColor  MultipleTargetFrameBuffer::GetTargetPixel(QString name,int x, int y)
{
    QColor pixel;

    if (m_targets.count(name) >0)
    {

        FrameBufferTarget target = m_targets[name];
        int per_pixel_depth;
        switch(target.format)
        {
        case GL_BGR:
        case GL_RGB:
            per_pixel_depth = 3;break;

        case GL_BGRA:
        case GL_RGBA:
            per_pixel_depth = 4;break;

        case GL_DEPTH_COMPONENT:
        case GL_ALPHA:
        case GL_LUMINANCE:
            per_pixel_depth =  1;break;
        }

        // establish buffer for pixel
        GLfloat * raw_pixel = new GLfloat[per_pixel_depth];

        // bind requested texture and read specified pixel
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
        glReadBuffer(target.attachment);
        glReadPixels(x,target.height - y,1,1,target.format,GL_FLOAT,raw_pixel);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        // transform pixel to QColor format
        switch(target.format)
        {
        case GL_BGR:
            pixel = QColor(raw_pixel[2],raw_pixel[1],raw_pixel[0]);break;
        case GL_RGB:
            pixel = QColor((raw_pixel[0] / 1.0f) * 255,(raw_pixel[1] / 1.0f) * 255,(raw_pixel[2] / 1.0f) * 255); break;

        case GL_BGRA:
            pixel = QColor(raw_pixel[2],raw_pixel[1],raw_pixel[0],raw_pixel[3]);break;
        case GL_RGBA:
            pixel = QColor(raw_pixel[0],raw_pixel[1],raw_pixel[2],raw_pixel[3]); break;

        case GL_DEPTH_COMPONENT:
        case GL_ALPHA:
        case GL_LUMINANCE:
            pixel = QColor(raw_pixel[0],raw_pixel[0],raw_pixel[0]); break;
        }
    }
    return pixel;
}

