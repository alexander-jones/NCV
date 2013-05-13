#include "qglxbuffer.h"
#include <QDebug>

const unsigned int NOT_CREATED = 0;
QGLXBuffer::QGLXBuffer()
{
    m_id = NOT_CREATED;
}


bool QGLXBuffer::create()
{
    glGenBuffers(1, &m_id);
    return true;

}
void QGLXBuffer::destroy()
{
    glDeleteBuffers(1, &m_id);

}
void QGLXBuffer::allocate(const void *data, int count, UsagePattern usage )
{
    glBindBuffer(ArrayBuffer,m_id);
    glBufferData(ArrayBuffer, count , data, usage);
    glBindBuffer(ArrayBuffer,0);
}

GLuint QGLXBuffer::id()
{
    return m_id;
}

bool QGLXBuffer::bind(TargetBinding binding)
{
    m_targetBinding = binding;
    glBindBuffer(m_targetBinding, m_id);


    return true;
}

void * QGLXBuffer::map(TargetBinding binding, QGLXBuffer::Access access)
{
    m_targetBinding = binding;
    glBindBuffer(m_targetBinding, m_id);
    return glMapBuffer(m_targetBinding,access);
}

bool QGLXBuffer::isCreated()
{
    return m_id != NOT_CREATED;
}

void  QGLXBuffer::unmap()
{
    if (m_targetBinding == Unbound)
        return;
    glUnmapBuffer(m_targetBinding);
    glBindBuffer(m_targetBinding, 0);

}
void QGLXBuffer::release()
{
    if (m_targetBinding == Unbound)
        return;
    glBindBuffer(m_targetBinding, 0);
    m_targetBinding = Unbound;

}

bool QGLXBuffer::read(int offset, void *data, int count)
{
    GLenum before = glGetError();
    glBindBuffer(m_targetBinding, m_id);
    glGetBufferSubData(m_targetBinding,offset,count,data);
    glBindBuffer(m_targetBinding, 0);
    GLenum after = glGetError();
    return after == 0 && before != 0;
}

void QGLXBuffer::write(int offset, void *data, int count)
{
    if (m_targetBinding == Unbound)
    glBufferSubData(m_targetBinding,offset,count,data);
}
