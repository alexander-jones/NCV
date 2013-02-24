#include "ncvcontinuousattribute.h"

NCVContinuousAttribute::NCVContinuousAttribute( NCVElementType type, GLfloat minValue , GLfloat maxValue)
    :NCVAttribute()
{
    m_buffer = QGLXBuffer(QGLXBuffer::TextureBuffer);
    m_dataDirty = false;
    m_colorationDirty = false;
    m_minValue = minValue;
    m_maxValue = maxValue;
    m_shaderDirty = true;
    m_type = type;

}
void NCVContinuousAttribute::attachData(QVector<GLfloat> data)
{
    m_data = data;
    m_dataDirty = true;
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

QGLShaderProgram * NCVContinuousAttribute::program()
{
    return &m_program;
}

NCVElementType NCVContinuousAttribute::elementType()
{
    return m_type;
}


GLfloat NCVContinuousAttribute::minValue()
{
    return m_minValue;
}

GLfloat NCVContinuousAttribute::maxValue()
{
    return m_maxValue;
}
QVector<QColor> NCVContinuousAttribute::coloration()
{
    return m_coloration;
}

QVector<GLfloat> NCVContinuousAttribute::data()
{
    return m_data;
}

void NCVContinuousAttribute::bind(QGLXCamera camera)
{
    resolve();

    m_program.bind();


    glActiveTexture(GL_TEXTURE0);
    m_buffer.bind();
    m_program.setUniformValue("Inst_Attribute", 0);

    glActiveTexture(GL_TEXTURE1);
    m_colorTexture.bind();
    m_program.setUniformValue("RangeMap",1);

    m_program.setUniformValue("WVP",camera.projection() * camera.view());
    m_program.setUniformValue("MinimumValue", m_minValue);
    m_program.setUniformValue("MaximumValue", m_maxValue);




}
void NCVContinuousAttribute::release()
{
    m_colorTexture.release(); //Ass
    m_buffer.release();
    m_program.release();
}


void NCVContinuousAttribute::resolve()
{
    if (m_shaderDirty)
    {
        if (m_type ==  Neuron)
        {
            m_program.addShaderFromSourceFile( QGLShader::Vertex, ":/shaders/neuronContinuous.vert" );
            m_program.addShaderFromSourceFile( QGLShader::Fragment, ":/shaders/continuous.frag" );
            m_program.link();
        }
        else if (m_type ==  Connection)
        {
            m_program.addShaderFromSourceFile( QGLShader::Vertex, ":/shaders/connectionContinuous.vert" );
            m_program.addShaderFromSourceFile( QGLShader::Geometry, ":/shaders/connectionContinuous.geom" );
            m_program.addShaderFromSourceFile( QGLShader::Fragment, ":/shaders/continuous.frag" );
            m_program.link();
        }
        m_shaderDirty = false;
    }
    if (m_dataDirty)
    {
        GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
        GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,1,componentSize);
        if (!m_buffer.isCreated())
            m_buffer.create();
        m_buffer.bind();
        m_buffer.allocate(&m_data[0],componentSize * m_data.count(),textureFormat);
        m_buffer.release();
        m_dataDirty = false;
    }
    if (m_colorationDirty)
    {

        if (!m_colorTexture.isCreated()) //Alex is horrible
            m_colorTexture.create();
        m_colorTexture.bind();
        m_colorTexture.allocate(m_colorationData.count()  ,GL_RGB32F,&m_colorationData[0]);

        m_colorTexture.setMinFilter(QGLXTexture1D::Linear);
        m_colorTexture.setMagFilter(QGLXTexture1D::Linear);
        m_colorTexture.setWrapFunction(QGLXTexture1D::Clamp);
        m_colorTexture.release();
        m_colorationDirty = false;
    }
}
