#include "ncvconnectionset.h"



NCVConnectionSet::NCVConnectionSet(NCVNeuronSet *neurons,QVector<NeuronConnection> connections)
{
    m_neurons = neurons;
    m_connections = connections;
    m_count = connections.count();

    m_initialized = false;
	m_dirty = false;

    m_scale.scale((m_neurons->bounds().size().length()/m_neurons->count()) * 10.0f );
}



void NCVConnectionSet::addAttribute(QString name,NCVAttribute * attribute)
{
	m_dirty = true;
    m_attributes[name] = attribute;
    if (m_attributes.count() ==1)
        m_currentAttribute = attribute;
}



void NCVConnectionSet::setCurrentAttribute(QString name)
{
	m_dirty = true;
    if (m_attributes.contains(name))
        m_currentAttribute = m_attributes[name];
}



void NCVConnectionSet::removeAttribute(QString name)
{
	m_dirty = true;
    m_attributes.remove(name);
}



bool NCVConnectionSet::dirty()
{
	if (m_dirty)
        return true;
    else if(m_currentAttribute != NULL)
    {
        if (m_currentAttribute->dirty())
                return true;
    }

	return false;
}



QMap<QString, NCVAttribute *> NCVConnectionSet::attributes()
{
    return m_attributes;
}



NCVAttribute* NCVConnectionSet::getCurrentAttribute()
{
    return m_currentAttribute;
}



void NCVConnectionSet::bind(QGLXCamera camera, bool deselected)
{
    if (m_currentAttribute != NULL)
    {
        m_currentAttribute->bind(camera);
        QGLShaderProgram * program = m_currentAttribute->program();

        glActiveTexture(GL_TEXTURE2);
        m_neurons->positionBuffer().bind(QGLXBuffer::TextureBuffer);
        program->setUniformValue("Inst_Translation", 2);

        program->setUniformValue("Deselected", (int)deselected);
        program->setUniformValue("ConnectionIDStart",m_neurons->count());
        program->setUniformValue("Scale",m_scale);

        m_neuronIdBuffer.bind(QGLXBuffer::ArrayBuffer);
        program->enableAttributeArray( "Neuron_ID");
        program->setAttributeBuffer("Neuron_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));

        m_idBuffer.bind(QGLXBuffer::ArrayBuffer);
        program->enableAttributeArray( "Inst_ID");
        program->setAttributeBuffer("Inst_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
        glVertexAttribDivisor(program->attributeLocation("Inst_ID"), 0);
    }
}



void NCVConnectionSet::bindSilhouettes(QGLXCamera camera, QColor color)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // perform silhouetting
    m_silhouetteProgram.bind();
    glActiveTexture(GL_TEXTURE0);
    m_neurons->positionBuffer().bind(QGLXBuffer::TextureBuffer);
    m_silhouetteProgram.setUniformValue("Inst_Translation", 0);

    m_silhouetteProgram.setUniformValue("WVP",camera.projection() * camera.view());

    m_neuronIdBuffer.bind(QGLXBuffer::ArrayBuffer);
    m_silhouetteProgram.enableAttributeArray( "Neuron_ID");
    m_silhouetteProgram.setAttributeBuffer("Neuron_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
    glVertexAttribDivisor( m_silhouetteProgram.attributeLocation("Neuron_ID"), 0);

    m_idBuffer.bind(QGLXBuffer::ArrayBuffer);
    m_silhouetteProgram.enableAttributeArray( "Inst_ID");
    m_silhouetteProgram.setAttributeBuffer("Inst_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
    glVertexAttribDivisor( m_silhouetteProgram.attributeLocation("Inst_ID"), 0);

    m_silhouetteProgram.setUniformValue("Scale",m_scale);
	m_silhouetteProgram.setUniformValue("SilhouetteColor",QVector3D(color.redF(),color.greenF(),color.blueF()));
    m_silhouetteProgram.setUniformValue("CameraDirection",camera.forward());
    m_silhouetteProgram.setUniformValue("ZFar",camera.farPlane());
    m_silhouetteProgram.setUniformValue("ZNear",camera.nearPlane());
    m_silhouetteProgram.setUniformValue("ZStop",camera.farPlane());
    m_silhouetteProgram.setUniformValue("DepthExponent",0.15f);
    m_silhouetteProgram.setUniformValue("MaxAlpha",1.0f);
}



void NCVConnectionSet::release()
{
    if (m_currentAttribute != NULL)
    {
        QGLShaderProgram * program = m_currentAttribute->program();
        program->disableAttributeArray( "Neuron_ID" );
        program->disableAttributeArray( "Inst_ID" );
        m_neuronIdBuffer.release();
        m_idBuffer.release();
        m_neurons->positionBuffer().release();
        m_currentAttribute->release();
    }
}



int NCVConnectionSet::count()
{
    return m_count;
}



void NCVConnectionSet::draw()
{
    drawSubset(0,m_count);
}



void NCVConnectionSet::drawSubset(int startElement, int count)
{
	m_dirty = false;
    glDrawArrays(GL_LINES,startElement * 2,count  * 2);
}



NCVNeuronSet* NCVConnectionSet::neurons()
{
	return m_neurons;
}



void NCVConnectionSet::releaseSilhouettes()
{
    m_silhouetteProgram.disableAttributeArray( "Neuron_ID" );
    m_silhouetteProgram.disableAttributeArray( "Inst_ID" );
    m_neuronIdBuffer.release();
    m_idBuffer.release();
    m_neurons->positionBuffer().release();
    m_silhouetteProgram.release();

    glDisable(GL_BLEND);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void NCVConnectionSet::resolve()
{
    if (!m_initialized)
    {
        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Vertex, ":/shaders/connectionSilhouette.vert");
        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Geometry, ":/shaders/connectionSilhouette.geom");
        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Fragment, ":/shaders/silhouette.frag");
        m_silhouetteProgram.link();

        int startIndex = m_neurons->count();
        QVector<GLuint> sysIDs;
        for (int i =0; i < m_count * 2; i ++)
            sysIDs.append(startIndex + 1 + (i/2));

        m_neuronIdBuffer.create();
        m_neuronIdBuffer.allocate( &m_connections[0], m_count * 2 * sizeof(GLuint));

        m_idBuffer.create();
        m_idBuffer.allocate( &sysIDs[0], m_count * 2 * sizeof(GLuint));

        m_initialized = true;

    }
    for(QMap<QString,NCVAttribute *>::iterator it = m_attributes.begin(); it != m_attributes.end(); it++)
        it.value()->resolve();
}

/*
void NCVConnectionSet::m_createGeometry()
{
        m_positionBuffer.create();
        m_positionBuffer.allocate(NULL,sizeof(QVector3D) * m_count * 8,QGLXBuffer::StreamDraw);

        m_normalBuffer.create();
        m_normalBuffer.allocate(NULL,sizeof(QVector3D) * m_count * 8,QGLXBuffer::StreamDraw);

        m_expandedIDBuffer.create();
        m_expandedIDBuffer.allocate(NULL,sizeof(GLuint) * m_count * 8,QGLXBuffer::StreamDraw);


        glGenTransformFeedbacks(1, &m_transformFeedback);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_positionBuffer.id());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, m_normalBuffer.id());
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, m_expandedIDBuffer.id());
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);


        glEnable(GL_RASTERIZER_DISCARD);

        m_expansionProgram.bind();
        glActiveTexture(GL_TEXTURE0);
        m_neurons->positionBuffer().bind(QGLXBuffer::TextureBuffer);
        m_expansionProgram.setUniformValue("Inst_Translation", 0);

        m_neuronIdBuffer.bind(QGLXBuffer::ArrayBuffer);
        m_expansionProgram.enableAttributeArray( "Neuron_ID");
        m_expansionProgram.setAttributeBuffer("Neuron_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
        glVertexAttribDivisor( m_expansionProgram.attributeLocation("Neuron_ID"), 0);

        m_idBuffer.bind(QGLXBuffer::ArrayBuffer);
        m_expansionProgram.enableAttributeArray( "Inst_ID");
        m_expansionProgram.setAttributeBuffer("Inst_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
        glVertexAttribDivisor( m_expansionProgram.attributeLocation("Inst_ID"), 0);

        m_expansionProgram.setUniformValue("Scale",m_scale);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback);
        glBeginTransformFeedback(GL_POINTS);

        glDrawArrays(GL_POINTS, 0, m_count);

        glEndTransformFeedback();
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        m_expansionProgram.release();
        glDisable(GL_RASTERIZER_DISCARD);


        //glBindBuffer(GL_ARRAY_BUFFER, BufferA);
        //glDrawTransformFeedback(GL_TRIANGLE_STRIP, TransformFeedback); // similar to glDrawArrays
}
*/
