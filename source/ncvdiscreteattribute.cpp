#include "ncvdiscreteattribute.h"

NCVDiscreteAttribute::NCVDiscreteAttribute(NCVElementType type, QVector<QString> states)
    :NCVAttribute()
{
    m_buffer = QGLXBuffer(QGLXBuffer::TextureBuffer);
    m_colorBuffer = QGLXBuffer(QGLXBuffer::TextureBuffer);
    m_dataDirty = false;
    m_colorationDirty = false;
    m_shaderDirty = true;
    m_states = states;
    m_type =type;

}
void NCVDiscreteAttribute::attachData(QVector<GLubyte> data, int bitsPerValue )
{
    m_data = data;
    m_bitsPerValue = bitsPerValue;
    m_bitsVal = 0;
    for (int i = 0;  i < bitsPerValue; i ++)
        m_bitsVal += (int)pow((double)2,(double)i);
    m_dataDirty = true;

}

void NCVDiscreteAttribute::attachColoration(QMap<QString,QColor> coloration)
{
    m_coloration = coloration;
    m_colorationData.clear();
    for (QVector<QString>::iterator it = m_states.begin(); it != m_states.end(); it++)
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
    for (QVector<QString>::iterator it = m_states.begin(); it != m_states.end(); it++)
    {
        QString key = *it;
        QColor color = QColor::fromRgb(coloration[key]);
        m_coloration[key] = color;
        m_colorationData.push_back(QVector3D(color.redF(),color.greenF(),color.blueF()));
    }
    m_colorationDirty = true;
}
QGLShaderProgram * NCVDiscreteAttribute::program()
{
    return &m_program;
}

NCVElementType NCVDiscreteAttribute::elementType()
{
    return m_type;
}

QMap<QString,QColor> NCVDiscreteAttribute::coloration()
{
    return m_coloration;
}

void NCVDiscreteAttribute::bind(QGLXCamera camera)
{
    resolve();

    m_program.bind();


    glActiveTexture(GL_TEXTURE0);
    m_buffer.bind();
    m_program.setUniformValue("Inst_Attribute", 0);

    glActiveTexture(GL_TEXTURE1);
    m_colorBuffer.bind();
    m_program.setUniformValue("ColorMap",1);

    m_program.setUniformValue("WVP",camera.projection() * camera.view());
    m_program.setUniformValue("BitsPerValue", m_bitsPerValue);
    m_program.setUniformValue("BitsVal", m_bitsVal);
    m_program.setUniformValue("States", m_states.count());




}

QVector<QString> NCVDiscreteAttribute::stateValues()
{
    return stateValues(0,(m_data.count() *m_bitsPerValue) /8);
}

QVector<QString> NCVDiscreteAttribute::stateValues(int firstElement, int count)
{
    QVector<QString> unpacked;
    int endElement = firstElement + count-1;
    for (int i = firstElement; i <= endElement; i ++)
    {
        GLubyte val = m_data[(i *m_bitsPerValue) /8] & (GLubyte) (m_bitsVal << (firstElement * m_bitsPerValue) % 8);
        val = qMin(val,(GLubyte)(m_states.count() -1));

        if ( (i+1)* (m_bitsPerValue /8) >= m_data.size() && (endElement % 8) < (firstElement % 8))
        {
            GLubyte addition = m_data[(i *m_bitsPerValue) /8] & (GLubyte) (m_bitsVal >> (m_bitsPerValue - (endElement % 8)));
            val += qMin(addition,(GLubyte)(m_states.count() -1));

        }
        unpacked.append(m_states[val]);
    }
    return unpacked;
}

QVector<GLubyte> NCVDiscreteAttribute::data()
{
    return m_data;
}

void NCVDiscreteAttribute::release()
{
    m_colorBuffer.release();
    m_buffer.release();
    m_program.release();

}

void NCVDiscreteAttribute::resolve()
{
    if (m_shaderDirty)
    {
        if (m_type == Neuron)
        {
            m_program.addShaderFromSourceFile( QGLShader::Vertex, ":/shaders/neuronDiscrete.vert" );
            m_program.addShaderFromSourceFile( QGLShader::Fragment, ":/shaders/discrete.frag" );
            m_program.link();
        }

        if (m_type == Connection)
        {
            m_program.addShaderFromSourceFile( QGLShader::Vertex, ":/shaders/connectionDiscrete.vert" );
            m_program.addShaderFromSourceFile( QGLShader::Geometry, ":/shaders/connectionDiscrete.geom" );
            m_program.addShaderFromSourceFile( QGLShader::Fragment, ":/shaders/discrete.frag" );
            m_program.link();
        }

        m_shaderDirty = false;
    }
    if (m_dataDirty)
    {
        GLuint componentSize = QGLXTexture::getComponentSize(GL_UNSIGNED_BYTE);
        GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_UNSIGNED_BYTE,1,componentSize);

        if (!m_buffer.isCreated())
            m_buffer.create();
        m_buffer.bind();
        m_buffer.allocate(&m_data[0],componentSize * m_data.count(),textureFormat);
        m_buffer.release();
        m_dataDirty = false;
    }
    if (m_colorationDirty)
    {
        GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
        GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,3,componentSize);

        if (!m_colorBuffer.isCreated())
            m_colorBuffer.create();

        m_colorBuffer.bind();
        m_colorBuffer.allocate(&m_colorationData[0],componentSize * 3 * m_colorationData.count(),textureFormat);
        m_colorBuffer.release();
        m_colorationDirty = false;
    }
}
