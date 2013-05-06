#include "ncvdiscreteattribute.h"

NCVDiscreteAttribute::NCVDiscreteAttribute(NCSDiscreteAttribute * parent)
    :NCVAttribute(parent)
{
    m_parent = parent;

    // establish discrete attribute states;
    QVector<QString> states = m_parent->states();
    QMap<QString,QColor >coloration;
    for (int i = 0; i < states.count(); i ++)
    {
        if (i % 2 == 0)
            coloration[states[i]] =QColor(0,255,0);
        else
            coloration[states[i]] = QColor(255,0,0);
    }
    this->attachColoration(coloration);

    m_dataDirty = true;
    m_colorationDirty = true;

}

int NCVDiscreteAttribute::bitMask()
{
    return m_parent->bitMask();
}

int NCVDiscreteAttribute::bitsPerValue()
{
    return m_parent->bitsPerValue();
}

bool NCVDiscreteAttribute::dirty()
{
    return m_dataDirty || m_colorationDirty;
}

void NCVDiscreteAttribute::attachColoration(QMap<QString,QColor> coloration)
{
    m_coloration = coloration;
    m_colorationData.clear();
    QVector<QString> states = m_parent->states();
    for (QVector<QString>::iterator it = states.begin(); it != states.end(); it++)
    {
        QString key = *it;
        QColor color = coloration[key];
        m_colorationData.push_back(QVector3D(color.redF(),color.greenF(),color.blueF()));
    }
    m_colorationDirty = true;
}
void NCVDiscreteAttribute::attachColoration(QMap<QString,QRgb> coloration)
{
    m_coloration.clear();
    m_colorationData.clear();
    QVector<QString> states = m_parent->states();
    for (QVector<QString>::iterator it = states.begin(); it != states.end(); it++)
    {
        QString key = *it;
        QColor color = QColor::fromRgb(coloration[key]);
        m_coloration[key] = color;
        m_colorationData.push_back(QVector3D(color.redF(),color.greenF(),color.blueF()));
    }
    m_colorationDirty = true;
}
QGLXBuffer NCVDiscreteAttribute::attributeBuffer()
{
    if (!m_buffer.isCreated())
        m_buffer.create();

    if (m_dataDirty)
    {
        GLuint componentSize = QGLXTexture::getComponentSize(GL_UNSIGNED_BYTE);
        GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_UNSIGNED_BYTE,1,componentSize);

        QVector<unsigned char> data = m_parent->data();
        if (data.count() > 0)
            m_buffer.allocate(&data[0],componentSize * data.count(),textureFormat);

        m_dataDirty = false;
    }
    return m_buffer;
}

QGLXBuffer NCVDiscreteAttribute::colorationBuffer()
{
    if (!m_colorBuffer.isCreated())
        m_colorBuffer.create();

    if (m_colorationDirty)
    {
        GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
        GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,3,componentSize);

        if (m_colorationData.count() > 0)
            m_colorBuffer.allocate(&m_colorationData[0],sizeof(QVector3D) * m_colorationData.count(),textureFormat);
        m_colorationDirty = false;
    }
    return m_colorBuffer;
}


QMap<QString,QColor> NCVDiscreteAttribute::coloration()
{
    return m_coloration;
}


QVector<QString> NCVDiscreteAttribute::states()
{
   return m_parent->states();
}

QVector<QString> NCVDiscreteAttribute::stateValues()
{
    return m_parent->stateValues();
}

QVector<QString> NCVDiscreteAttribute::stateValues(int firstElement, int count)
{
    return m_parent->stateValues(firstElement,count);
}

QVector<unsigned char> NCVDiscreteAttribute::data()
{
    return m_parent->data();
}


void NCVDiscreteAttribute::destroy()
{
    if (m_buffer.isCreated())
        m_buffer.destroy();
    if (m_colorBuffer.isCreated())
        m_colorBuffer.destroy();
}

void NCVDiscreteAttribute::m_parentChanged()
{
    m_dataDirty = true;
}
