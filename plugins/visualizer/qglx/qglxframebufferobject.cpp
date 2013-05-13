#include "qglxframebufferobject.h"
#include <QtDebug>

QGLXFrameBufferObject::QGLXFrameBufferObject()
{
}

void QGLXFrameBufferObject::create()
{
    glGenFramebuffers(1, &m_fbo);
}


bool QGLXFrameBufferObject::bind()
{
    // bind all active render targets
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    return true;
}
void QGLXFrameBufferObject::enableColorAttachments(int num)
{
    GLenum * attachments = new GLenum[num];
    for (int i = 0; i < num; i ++)
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers(num, (const GLenum *)&attachments[0]);

}

void QGLXFrameBufferObject::release()
{
    // release current targets by binding default target
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void QGLXFrameBufferObject::blitTexture(QGLXTexture2D& source,QGLXTexture2D::FrameBufferAttachment sourceAttachment,GLuint offset,QRect from,QRect to, GLenum destMode )
{
    if (sourceAttachment != QGLXTexture2D::Color)
        offset = 0;

    // set draw buffer as default buffer (if it is already not for some reason)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    // set our framebuffer object as read buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);


    if (sourceAttachment != QGLXTexture2D::Depth && sourceAttachment != QGLXTexture2D::DepthStencil)
        glDrawBuffer(destMode);

    // tell OpenGL to read this target and blit it onto the rectangle specified
    source.bind(QGLXTexture2D::Read,(QGLXTexture2D::FrameBufferAttachment)((GLenum)sourceAttachment +  offset));


    if (sourceAttachment == QGLXTexture2D::Depth)
        glBlitFramebuffer(from.left(), from.top(), from.width(), from.height(),
                          to.left(),to.top(), to.width(), to.height(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    else if (sourceAttachment == QGLXTexture2D::DepthStencil)
    {
        glBlitFramebuffer(from.left(), from.top(), from.width(), from.height(),
                          to.left(),to.top(), to.width(), to.height(), GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    }
    else
    {
        glBlitFramebuffer(from.left(), from.top(), from.width(), from.height(),
                          to.left(),to.top(), to.width(), to.height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    // release this framebuffer from reading, exit to default
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

}

void QGLXFrameBufferObject::blitTexture(QGLXTexture2D & source,QRect from,  QGLXTexture2D& dest,QRect to )
{
    // set draw buffer as default buffer (if it is already not for some reason)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_fbo);
    // set our framebuffer object as read buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

    dest.bind(QGLXTexture2D::Draw,QGLXTexture2D::Color,0);
    glDrawBuffer(QGLXTexture2D::Color + 0);

    // tell OpenGL to read this target and blit it onto the rectangle specified
    source.bind(QGLXTexture2D::Draw,QGLXTexture2D::Color,1);
    glReadBuffer(QGLXTexture2D::Color + 1);


    glBlitFramebuffer(from.left(),from.top(), from.width(), from.height(),
                      to.left(),to.top(), to.width(), to.height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

    source.release();
    dest.release();
    // release this framebuffer from reading, exit to default
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    // set draw buffer as default buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

void QGLXFrameBufferObject::getTextureData(QGLXTexture2D tex,QRect area, void * data)
{
    // bind requested texture and read specified pixel
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    tex.bind(QGLXTexture2D::Read,QGLXTexture2D::Color,0);
    glReadBuffer(QGLXTexture2D::Color + 0);
    glReadPixels(area.left(),tex.height() - area.bottom(),area.width(),area.height(),tex.pixelFormat(),tex.pixelType() ,data);
    tex.release();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);


}
