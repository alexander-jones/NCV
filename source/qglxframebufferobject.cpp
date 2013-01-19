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


void QGLXFrameBufferObject::blitTarget(QGLXTexture2D& source,QGLXTexture2D::FrameBufferAttachment sourceAttachment,QRect from,QRect to, GLenum destMode )
{
    GLuint preCallError = glGetError();

    // set draw buffer as default buffer (if it is already not for some reason)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    // set our framebuffer object as read buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);


    if (sourceAttachment != QGLXTexture2D::Depth && sourceAttachment != QGLXTexture2D::DepthStencil)
        glDrawBuffer(destMode);

    // tell OpenGL to read this target and blit it onto the rectangle specified
    source.bind(QGLXTexture2D::Read,sourceAttachment);


    if (sourceAttachment == QGLXTexture2D::Depth)
        glBlitFramebuffer(from.left(), from.top(), from.width(), from.height(),
                          to.left(),to.top(), to.width(), to.height(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    else if (sourceAttachment == QGLXTexture2D::DepthStencil)
    {
        glBlitFramebuffer(from.left(), from.top(), from.width(), from.height(),
                          to.left(),to.top(), to.width(), to.height(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBlitFramebuffer(from.left(), from.top(), from.width(), from.height(),
                          to.left(),to.top(), to.width(), to.height(), GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    }
    else
    {
        glBlitFramebuffer(from.left(), from.top(), from.width(), from.height(),
                          to.left(),to.top(), to.width(), to.height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // release this framebuffer from reading, exit to default
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    GLuint postCallError = glGetError();
    if (postCallError != 0  && preCallError ==0)
        qDebug() <<  QGLXCore::getErrorString(postCallError) <<"error in QGLXFrameBufferObject::blitTarget." ;

}

void QGLXFrameBufferObject::blitTarget(QGLXTexture2D & source,QRect from,  QGLXTexture2D& dest,QRect to )
{
    GLuint preCallError = glGetError();

    // set draw buffer as default buffer (if it is already not for some reason)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_fbo);
    // set our framebuffer object as read buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

    dest.bind(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    glDrawBuffer(QGLXTexture2D::Color0);

    // tell OpenGL to read this target and blit it onto the rectangle specified
    source.bind(QGLXTexture2D::Draw,QGLXTexture2D::Color1);
    glReadBuffer(QGLXTexture2D::Color1);
    glBlitFramebuffer(from.left(),from.top(), from.width(), from.height(),
                      to.left(),to.top(), to.width(), to.height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // release this framebuffer from reading, exit to default
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    // set draw buffer as default buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    GLuint postCallError = glGetError();
    if (postCallError != 0  && preCallError ==0)
        qDebug() << postCallError << " GL error in QGLXFrameBufferObject::blitTarget." ;
}

void QGLXFrameBufferObject::getTargetPixels(QGLXTexture2D tex,QRect area, void * data)
{

    // bind requested texture and read specified pixel
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

    tex.bind(QGLXTexture2D::Read,QGLXTexture2D::Color0);
    glReadBuffer(QGLXTexture2D::Color0);
    glReadPixels(area.left(),tex.height() - area.bottom(),area.width(),area.height(),tex.pixelFormat(),tex.pixelType() ,data);
    glReadBuffer(GL_BACK);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);


}
