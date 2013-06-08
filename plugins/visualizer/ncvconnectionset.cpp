#include "ncvconnectionset.h"



NCVConnectionSet::NCVConnectionSet(NCSConnectionSet * ncsParent, NCVNeuronSet * neurons,QObject * parent)
    :QObject(parent)
{
    m_neurons = neurons;
    m_parent = ncsParent;
    connect(m_parent,SIGNAL(attributeAdded(QString,NCSAttribute*)),this,SLOT(m_addAttribute(QString,NCSAttribute*)));
    connect(m_parent,SIGNAL(attributeRemoved(QString)),this,SLOT(m_removeAttribute(QString)));
    QMap<QString,NCSAttribute *> parentAttribs = m_parent->attributes();
    for(QMap<QString,NCSAttribute *>::iterator it = parentAttribs.begin(); it != parentAttribs.end(); it++)
        m_addAttribute(it.key(),it.value());

    m_initialized = false;
	m_dirty = false;
    m_scale.scale(15.0f);
    m_depthBias = 0.000001f;
}



void NCVConnectionSet::destroy()
{
    if (!m_initialized)
        return;

    m_neuronIdBuffer.destroy();
    m_idBuffer.destroy();

    for(QMap<QString,NCVAttribute *>::iterator it = m_attributes.begin(); it != m_attributes.end(); it++)
        it.value()->destroy();
    m_initialized = false;
}

void NCVConnectionSet::setScale(int scale)
{
    m_scale = QMatrix4x4();
    m_scale.scale(scale);
    m_dirty = true;
}


void NCVConnectionSet::setCurrentAttribute(QString name)
{
    if (m_attributes.contains(name))
    {
        m_dirty = true;
        m_currentAttribute = m_attributes[name];
    }
}


bool NCVConnectionSet::dirty()
{
	if (m_dirty)
        return true;
    else if(m_currentAttribute != NULL)
        return m_currentAttribute->dirty();


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
        resolve();
        m_currentAttribute->resolve();
        QGLShaderProgram * program;
        if (m_currentAttribute->type() == Discrete)
            program = &m_discreteProgram;
        else
            program = &m_continuousProgram;

        program->bind();

        m_neuronIdBuffer.bind(QGLXBuffer::ArrayBuffer);
        program->enableAttributeArray( "Neuron_ID");
        program->setAttributeBuffer("Neuron_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
        glVertexAttribDivisor(program->attributeLocation("Inst_ID"), 0);

        m_idBuffer.bind(QGLXBuffer::ArrayBuffer);
        program->enableAttributeArray( "Inst_ID");
        program->setAttributeBuffer("Inst_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
        glVertexAttribDivisor(program->attributeLocation("Inst_ID"), 0);

        m_neurons->positionBufferTexture().bind(0);
        program->setUniformValue("Inst_Translation", 0);

        if (m_currentAttribute->type() == Discrete)
        {
            NCVDiscreteAttribute * attrib = dynamic_cast<NCVDiscreteAttribute *>(m_currentAttribute);

            attrib->attributeTexture().bind(1);
            program->setUniformValue("Inst_Attribute", 1);

            attrib->colorationTexture().bind(2);
            program->setUniformValue("ColorMap",2);

            program->setUniformValue("BitsPerValue", attrib->bitsPerValue());
            program->setUniformValue("BitMask", attrib->bitMask());
            program->setUniformValue("States", attrib->states().count());
        }
        else
        {
            NCVContinuousAttribute * attrib = dynamic_cast<NCVContinuousAttribute *>(m_currentAttribute);

            attrib->attributeTexture().bind(1);
            program->setUniformValue("Inst_Attribute", 1);

            attrib->colorationTexture().bind(2);
            program->setUniformValue("ColorMap",2);


            program->setUniformValue("MinimumValue", attrib->minValue());
            program->setUniformValue("MaximumValue", attrib->maxValue());
        }

        program->setUniformValue("Deselected", (int)deselected);
        program->setUniformValue("ConnectionIDStart",m_neurons->count());
        program->setUniformValue("Scale",m_scale);
        program->setUniformValue("WVP",camera.projection() * camera.view());
        program->setUniformValue("DepthBias",m_depthBias);
        program->setUniformValue("FarBias",1.0f/(float)log(camera.farPlane()*m_depthBias + 1));
        program->setUniformValue("NearPlane",camera.nearPlane());
        program->setUniformValue("FarPlane",camera.farPlane());

    }
}



void NCVConnectionSet::bindSilhouettes(QGLXCamera camera, QColor color)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // perform silhouetting
    m_silhouetteProgram.bind();

    m_neuronIdBuffer.bind(QGLXBuffer::ArrayBuffer);
    m_silhouetteProgram.enableAttributeArray( "Neuron_ID");
    m_silhouetteProgram.setAttributeBuffer("Neuron_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
    glVertexAttribDivisor( m_silhouetteProgram.attributeLocation("Neuron_ID"), 0);

    m_neurons->positionBufferTexture().bind(0);
    m_silhouetteProgram.setUniformValue("Inst_Translation", 0);

    m_silhouetteProgram.setUniformValue("WVP",camera.projection() * camera.view());
    m_silhouetteProgram.setUniformValue("Scale",m_scale);
	m_silhouetteProgram.setUniformValue("SilhouetteColor",QVector3D(color.redF(),color.greenF(),color.blueF()));
    m_silhouetteProgram.setUniformValue("CameraDirection",camera.forward());
    m_silhouetteProgram.setUniformValue("DepthBias",m_depthBias);
    m_silhouetteProgram.setUniformValue("FarBias",1.0f/(float)log(camera.farPlane()*m_depthBias + 1));
    m_silhouetteProgram.setUniformValue("NearPlane",camera.nearPlane());
    m_silhouetteProgram.setUniformValue("FarPlane",camera.farPlane());
    m_silhouetteProgram.setUniformValue("SilhouetteDepthMagnification",0.5f);
    m_silhouetteProgram.setUniformValue("SilhouetteMaxAlpha",0.5f);
}


void NCVConnectionSet::release()
{
    if (m_currentAttribute != NULL)
    {
        QGLShaderProgram * program;
        if (m_currentAttribute->type() == Discrete)
        {
            program = &m_discreteProgram;
            NCVDiscreteAttribute * attrib = dynamic_cast<NCVDiscreteAttribute *>(m_currentAttribute);
            attrib->colorationTexture().release();
            attrib->attributeTexture().release();
        }
        else
        {
            program = &m_continuousProgram;
            NCVContinuousAttribute * attrib = dynamic_cast<NCVContinuousAttribute *>(m_currentAttribute);
            attrib->colorationTexture().release();
            attrib->attributeTexture().release();
        }

        program->disableAttributeArray( "Neuron_ID" );
        program->disableAttributeArray( "Inst_ID" );
        program->release();
        m_neuronIdBuffer.release();
        m_idBuffer.release();
        m_neurons->positionBufferTexture().release();
    }
}



int NCVConnectionSet::count()
{
    return m_parent->count();
}



void NCVConnectionSet::draw()
{
    drawSubset(0,m_parent->count());
}



void NCVConnectionSet::drawSubset(int startElement, int count)
{
	m_dirty = false;
    glDrawArrays(GL_LINES,startElement * 2,count  * 2);
}



void NCVConnectionSet::releaseSilhouettes()
{
    m_silhouetteProgram.disableAttributeArray( "Neuron_ID" );
    m_silhouetteProgram.disableAttributeArray( "Inst_ID" );
    m_neuronIdBuffer.release();
    m_idBuffer.release();
    m_neurons->positionBufferTexture().release();
    m_silhouetteProgram.release();

    glDisable(GL_BLEND);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void NCVConnectionSet::resolve()
{
    if (!m_initialized)
    {

        m_discreteProgram.addShaderFromSourceFile( QGLShader::Vertex, ":/plugins/visualizer/shaders/connectionDiscrete.vert" );
        m_discreteProgram.addShaderFromSourceFile( QGLShader::Geometry, ":/plugins/visualizer/shaders/connectionDiscrete.geom" );
        m_discreteProgram.addShaderFromSourceFile( QGLShader::Fragment, ":/plugins/visualizer/shaders/discrete.frag" );
        m_discreteProgram.link();

        m_continuousProgram.addShaderFromSourceFile( QGLShader::Vertex, ":/plugins/visualizer/shaders/connectionContinuous.vert" );
        m_continuousProgram.addShaderFromSourceFile( QGLShader::Geometry, ":/plugins/visualizer/shaders/connectionContinuous.geom" );
        m_continuousProgram.addShaderFromSourceFile( QGLShader::Fragment, ":/plugins/visualizer/shaders/continuous.frag" );
        m_continuousProgram.link();

        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Vertex, ":/plugins/visualizer/shaders/connectionSilhouette.vert");
        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Geometry, ":/plugins/visualizer/shaders/connectionSilhouette.geom");
        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Fragment, ":/plugins/visualizer/shaders/silhouette.frag");
        m_silhouetteProgram.link();

        int startIndex = m_neurons->count();
        QVector<GLuint> sysIDs;
        for (int i =0; i < m_parent->count() * 2; i ++)
            sysIDs.append(startIndex + 1 + (i/2));

        QVector<NCSConnection> globalConnections = m_parent->connections();
        QVector<NCSConnection> connections;
        for (int i =0; i < globalConnections.count(); i ++)
            connections.append(NCSConnection(globalConnections[i].inNeuron+1,globalConnections[i].outNeuron +1));
        m_neuronIdBuffer.create();
        m_neuronIdBuffer.allocate( &connections[0], m_parent->count() * 2 * sizeof(GLuint));

        m_idBuffer.create();
        m_idBuffer.allocate( &sysIDs[0], m_parent->count() * 2 * sizeof(GLuint));

        m_initialized = true;

    }
}

void NCVConnectionSet::m_addAttribute(QString name,NCSAttribute * attrib)
{
    if (attrib->type() == Discrete)
        m_attributes[name] = new NCVDiscreteAttribute(dynamic_cast<NCSDiscreteAttribute*>(attrib));
    else
        m_attributes[name] = new NCVContinuousAttribute(dynamic_cast<NCSContinuousAttribute*>(attrib));

    if (m_currentAttribute == NULL)
        m_currentAttribute = m_attributes[name];

    m_dirty = true;
}


void NCVConnectionSet::m_removeAttribute(QString name)
{
    m_dirty = true;
    m_attributes.remove(name);
}

/*
void NCVConnectionSet::m_createGeometry()
{
        m_positionBuffer.create();
        m_positionBuffer.allocate(NULL,sizeof(QVector3D) * m_parent->count() * 8,QGLXBuffer::StreamDraw);

        m_normalBuffer.create();
        m_normalBuffer.allocate(NULL,sizeof(QVector3D) * m_parent->count() * 8,QGLXBuffer::StreamDraw);

        m_expandedIDBuffer.create();
        m_expandedIDBuffer.allocate(NULL,sizeof(GLuint) * m_parent->count() * 8,QGLXBuffer::StreamDraw);


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

        glDrawArrays(GL_POINTS, 0, m_parent->count());

        glEndTransformFeedback();
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        m_expansionProgram.release();
        glDisable(GL_RASTERIZER_DISCARD);


        //glBindBuffer(GL_ARRAY_BUFFER, BufferA);
        //glDrawTransformFeedback(GL_TRIANGLE_STRIP, TransformFeedback); // similar to glDrawArrays
}
*/
