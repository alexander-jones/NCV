#include "ncvneuronset.h"



NCVNeuronSet::NCVNeuronSet(QVector<QVector3D> positions)
{
    m_currentAttribute = NULL;
    m_positions = positions;
    m_count = m_positions.count();
    QVector3D lowBound = QVector3D(100000000,100000000,100000000);
    QVector3D highBound = QVector3D(-100000000,-100000000,-100000000);

    // loop through all positions, establish lowest/ highest xyz coords
    for (int i = 0 ; i < m_positions.count(); i++)
    {
        if (m_positions[i].x() < lowBound.x())
            lowBound.setX(m_positions[i].x());
        else if (m_positions[i].x() > highBound.x())
            highBound.setX(m_positions[i].x());

        if (m_positions[i].y() < lowBound.y())
            lowBound.setY(m_positions[i].y());

        else if (m_positions[i].y() > highBound.y())
            highBound.setY(m_positions[i].y());

        if (m_positions[i].z() < lowBound.z())
            lowBound.setZ(m_positions[i].z());

        else if (m_positions[i].z() > highBound.z())
            highBound.setZ(m_positions[i].z());
    }

    QVector3D size = QVector3D(highBound.x() - lowBound.x(),highBound.y() - lowBound.y(),highBound.z() - lowBound.z());
    QVector3D center = QVector3D(lowBound.x()+ size.x()/2, lowBound.y() + size.y()/2,lowBound.z()+ size.z()/2);
    m_bounds = QGLXBoundingBox(center,size);

    m_scale.scale((size.length() / m_positions.count()) * 80.0f );

    m_positionBuffer = QGLXBuffer(QGLXBuffer::TextureBuffer);
    m_initialized = false;
    m_dirty = false;
}



bool NCVNeuronSet::dirty()
{
    if(m_dirty)
		return true;
    else if(m_currentAttribute != NULL)
    {
        if(m_currentAttribute->dirty())
                return true;
    }

	return false;
}



QGLXBuffer NCVNeuronSet::positionBuffer()
{
	return m_positionBuffer;
}



void NCVNeuronSet::bind(QGLXCamera camera, bool deselected)
{
    if (m_currentAttribute != NULL)
    {
        m_currentAttribute->bind(camera);

        QGLShaderProgram * program = m_currentAttribute->program();

        glActiveTexture(GL_TEXTURE2);
        m_positionBuffer.bind();
        program->setUniformValue("Inst_Translation", 2);

        program->setUniformValue("Deselected", (int)deselected);
        program->setUniformValue("Scale",m_scale);

        m_indexBuffer.bind();

        m_vertexBuffer.bind();
        program->enableAttributeArray( "Vert_Position");
        program->setAttributeBuffer("Vert_Position", GL_FLOAT,  0 ,3, sizeof(QVector3D));
        glVertexAttribDivisor( program->attributeLocation("Vert_Position"), 0);

        m_normalBuffer.bind();
        program->enableAttributeArray( "Vert_Normal");
        program->setAttributeBuffer("Vert_Normal", GL_FLOAT,  0 ,3, sizeof(QVector3D));
        glVertexAttribDivisor( program->attributeLocation("Vert_Normal"), 0);

        m_idBuffer.bind();
        program->enableAttributeArray( "Inst_ID");
        program->setAttributeBuffer("Inst_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
        glVertexAttribDivisor( program->attributeLocation("Inst_ID"), 1);
    }
}



void NCVNeuronSet::bindSilhouettes(QGLXCamera camera)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(1,1.5);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // perform silhouetting
    m_silhouetteProgram.bind();

    glActiveTexture(GL_TEXTURE0);
    m_positionBuffer.bind();
    m_silhouetteProgram.setUniformValue("Inst_Translation", 0);

    m_silhouetteProgram.setUniformValue("WVP",camera.projection() * camera.view());

    m_indexBuffer.bind();

    m_vertexBuffer.bind();
    m_silhouetteProgram.enableAttributeArray( "Vert_Position");
    m_silhouetteProgram.setAttributeBuffer("Vert_Position", GL_FLOAT,  0 ,3, sizeof(QVector3D));
    glVertexAttribDivisor( m_silhouetteProgram.attributeLocation("Vert_Position"), 0);

    m_normalBuffer.bind();
    m_silhouetteProgram.enableAttributeArray( "Vert_Normal");
    m_silhouetteProgram.setAttributeBuffer("Vert_Normal", GL_FLOAT,  0 ,3, sizeof(QVector3D));
    glVertexAttribDivisor( m_silhouetteProgram.attributeLocation("Vert_Normal"), 0);

    m_idBuffer.bind();
    m_silhouetteProgram.enableAttributeArray( "Inst_ID");
    m_silhouetteProgram.setAttributeBuffer("Inst_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
    glVertexAttribDivisor( m_silhouetteProgram.attributeLocation("Inst_ID"), 1);

    m_silhouetteProgram.setUniformValue("Scale",m_scale);

    m_silhouetteProgram.setUniformValue("SilhouetteColor",QVector3D(0,0,0));
    m_silhouetteProgram.setUniformValue("CameraDirection",camera.forward());
    m_silhouetteProgram.setUniformValue("ZFar",camera.farPlane());
    m_silhouetteProgram.setUniformValue("ZNear",camera.nearPlane());
    m_silhouetteProgram.setUniformValue("ZStop",camera.farPlane()/2);
    m_silhouetteProgram.setUniformValue("DepthExponent",2.0f);
    m_silhouetteProgram.setUniformValue("MaxAlpha",0.5f);
}



void NCVNeuronSet::release()
{
    if (m_currentAttribute != NULL)
    {
        QGLShaderProgram * program = m_currentAttribute->program();
        program->disableAttributeArray( "Vert_Position" );
        program->disableAttributeArray( "Vert_Normal" );
        program->disableAttributeArray( "Inst_ID" );
        m_normalBuffer.release();
        m_vertexBuffer.release();
        m_indexBuffer.release();
        m_idBuffer.release();

        m_positionBuffer.release();
        m_currentAttribute->release();
    }
}



void NCVNeuronSet::releaseSilhouettes()
{
    m_silhouetteProgram.disableAttributeArray( "Vert_Position" );
    m_silhouetteProgram.disableAttributeArray( "Vert_Normal" );
    m_silhouetteProgram.disableAttributeArray( "Inst_ID" );
    m_normalBuffer.release();
    m_vertexBuffer.release();
    m_indexBuffer.release();
    m_idBuffer.release();

    m_positionBuffer.release();
    m_silhouetteProgram.release();

    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}



QGLXBoundingBox NCVNeuronSet::bounds()
{
    return m_bounds;
}



QMap<QString,NCVAttribute *> NCVNeuronSet::attributes()
{
    return m_attributes;
}



NCVAttribute* NCVNeuronSet::getCurrentAttribute()
{
    return m_currentAttribute;
}



void NCVNeuronSet::addAttribute(QString name, NCVAttribute * attribute)
{
    m_attributes[name] = attribute;
    if (m_attributes.count() ==1)
        m_currentAttribute = attribute;

	m_dirty = true;
}



void NCVNeuronSet::setCurrentAttribute(QString name)
{
    if (m_attributes.contains(name))
        m_currentAttribute = m_attributes[name];

	m_dirty = true;
}



void NCVNeuronSet::removeAttribute(QString name)
{
    m_attributes.remove(name);

	m_dirty = true;
}



int NCVNeuronSet::count()
{
    return m_count;
}



void NCVNeuronSet::draw()
{
    drawSubset(0,m_count);
}



void NCVNeuronSet::drawSubset(int startElement, int count)
{
	m_dirty = false;
    glDrawElementsInstancedBaseInstance(GL_TRIANGLES,36 ,GL_UNSIGNED_INT,0,count , startElement  );
}



void NCVNeuronSet::resolve()
{
    if (!m_initialized)
    {
        GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
        GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,3,componentSize);

        m_positionBuffer.create();
        m_positionBuffer.bind();
        m_positionBuffer.allocate(&m_positions[0],3* componentSize * m_count,textureFormat);
        m_positionBuffer.release();

        QVector3D vertices[8] = {
            QVector3D(-0.57735, -0.57735, -0.57735) ,
            QVector3D(-0.57735, 0.57735, -0.57735) ,
            QVector3D(0.57735, 0.57735, -0.57735) ,
            QVector3D(0.57735, -0.57735, -0.57735),
            QVector3D(0.57735, -0.57735, 0.57735) ,
            QVector3D(-0.57735, -0.57735, 0.57735) ,
            QVector3D(-0.57735, 0.57735, 0.57735) ,
            QVector3D(0.57735, 0.57735, 0.57735) };


        QVector3D normals[8] = {
            QVector3D(-0.57735, -0.57735, -0.57735) ,
            QVector3D(-0.408248, 0.816497, -0.408248) ,
            QVector3D(0.408248, 0.408248, -0.816497) ,
            QVector3D(0.816497, -0.408248, -0.408248) ,
            QVector3D(0.408248, -0.816497, 0.408248) ,
            QVector3D(-0.408248, -0.408248, 0.816497),
            QVector3D(-0.816497, 0.408248, 0.408248) ,
            QVector3D(0.57735, 0.57735, 0.57735) };

        GLuint indices[36] = {0,1,2, 0,2,3, 0, 3,4, 0,4,5 ,0,5,6 ,0,6,1 ,7,4,3 ,7,3,2 ,7,2,1 , 7,1,6 ,7,6,5, 7,5,4};

        QVector<GLuint> sysIDs;
        for (int i =0; i < m_count; i ++)
            sysIDs.append( i+1);

        m_normalBuffer.create();
        m_normalBuffer.bind();
        m_normalBuffer.allocate( &normals[0], 8 * sizeof(QVector3D));
        m_normalBuffer.release();

        m_vertexBuffer.create();
        m_vertexBuffer.bind();
        m_vertexBuffer.allocate( &vertices[0], 8 * sizeof(QVector3D));
        m_vertexBuffer.release();

        m_indexBuffer = QGLXBuffer(QGLXBuffer::IndexBuffer);
        m_indexBuffer.create();
        m_indexBuffer.bind();
        m_indexBuffer.allocate( &indices[0], 36 * sizeof(GLuint));
        m_indexBuffer.release();

        m_idBuffer.create();
        m_idBuffer.bind();
        m_idBuffer.allocate( &sysIDs[0], m_count * sizeof(GLuint));
        m_idBuffer.release();

        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Vertex,":/shaders/neuronSilhouette.vert" );
        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Fragment,":/shaders/silhouette.frag" );
        m_silhouetteProgram.link();

        m_initialized = true;
    }

    for(QMap<QString,NCVAttribute *>::iterator it = m_attributes.begin(); it != m_attributes.end(); it++)
        it.value()->resolve();
}
