#include "ncvcontinuousattribute.h"
NCVContinuousAttribute::NCVContinuousAttribute( NCSContinuousAttribute * parent)
    :NCVAttribute(parent)
{
    m_dataDirty = false;
    m_colorationDirty = false;
    m_parent = parent;

    // establish attribute  colorations
    ContinuousColorSelector * voltageRange = new ContinuousColorSelector();
    voltageRange->setLowThreshold(m_parent->minValue());
    voltageRange->setHighThreshold(m_parent->maxValue());
    voltageRange->addMarker(m_parent->minValue() ,QColor(0,255,0));
    voltageRange->addMarker((m_parent->minValue() +  m_parent->maxValue()) / 2.0f,QColor(0,0,255));
    voltageRange->addMarker(m_parent->maxValue(),QColor(255,0,0));

    this->attachColoration(voltageRange->getData());

    connect(m_parent,SIGNAL(dataChanged()),this,SLOT(m_parentChanged()));
}

void NCVContinuousAttribute::attachColoration(QVector< QRgb> data)
{
    m_coloration.clear();
    m_colorationData.clear();
    for (int i = 0; i < data.count(); i ++)
    {
        QColor pixel = QColor::fromRgb(data[i]);
        m_coloration.append(pixel);
        m_colorationData.push_back(QVector3D(pixel.redF(),pixel.greenF(),pixel.blueF()));
    }
    m_colorationDirty = true;
}

void NCVContinuousAttribute::attachColoration(QVector< QColor> data)
{
    m_coloration = data;
    m_colorationData.clear();
    for (int i = 0; i < data.count(); i ++)
        m_colorationData.push_back(QVector3D(data[i].redF(),data[i].greenF(),data[i].blueF()));
    m_colorationDirty = true;
}



bool NCVContinuousAttribute::dirty()
{
    return m_dataDirty || m_colorationDirty;
}


float NCVContinuousAttribute::minValue()
{
    return m_parent->minValue();
}

float NCVContinuousAttribute::maxValue()
{
    return m_parent->maxValue();
}
QVector<QColor> NCVContinuousAttribute::coloration()
{
    return m_coloration;
}

QVector<float> NCVContinuousAttribute::data()
{
    return m_parent->data();
}


void NCVContinuousAttribute::destroy()
{
    if (m_buffer.isCreated())
        m_buffer.destroy();
    if (m_colorTexture.isCreated())
        m_colorTexture.destroy();
}

QGLXTexture1D NCVContinuousAttribute::colorationTexture()
{

    if (!m_colorTexture.isCreated())
        m_colorTexture.create();

    if (m_colorationDirty)
    {
        if (m_colorationData.count() > 0)
            m_colorTexture.allocate(m_colorationData.count()  ,GL_RGB32F,&m_colorationData[0]);

        m_colorTexture.bind();
        m_colorTexture.setMinFilter(QGLXTexture1D::Linear);
        m_colorTexture.setMagFilter(QGLXTexture1D::Linear);
        m_colorTexture.setWrapFunction(QGLXTexture1D::Clamp);
        m_colorTexture.release();
        m_colorationDirty = false;
    }
    return m_colorTexture;
}
QGLXBuffer NCVContinuousAttribute::attributeBuffer()
{
    if (!m_buffer.isCreated())
        m_buffer.create();
    if (m_dataDirty)
    {
        GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
        GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,1,componentSize);

        m_buffer.bind(QGLXBuffer::TextureBuffer);
        QVector<float> data = m_parent->data();
        if (data.count() > 0);
            m_buffer.allocate(&data[0],componentSize * data.count(),textureFormat);
        m_buffer.release();
        m_dataDirty = false;
    }
    return m_buffer;
}

void NCVContinuousAttribute::m_parentChanged()
{
    m_dataDirty = true;
}
