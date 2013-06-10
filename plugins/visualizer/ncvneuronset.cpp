#include "ncvneuronset.h"



NCVNeuronSet::NCVNeuronSet(NCSNeuronSet * ncsParent, QObject * parent)
    :QObject(parent)

{
    m_currentAttribute = NULL;
    m_parent = ncsParent;

    connect(m_parent,SIGNAL(attributeAdded(QString,NCSAttribute*)),this,SLOT(m_addAttribute(QString,NCSAttribute*)));
    connect(m_parent,SIGNAL(attributeRemoved(QString)),this,SLOT(m_removeAttribute(QString)));

    QMap<QString,NCSAttribute *> parentAttribs = m_parent->attributes();
    for(QMap<QString,NCSAttribute *>::iterator it = parentAttribs.begin(); it != parentAttribs.end(); it++)
        m_addAttribute(it.key(),it.value());

    QVector3D lowBound = QVector3D(100000000,100000000,100000000);
    QVector3D highBound = QVector3D(-100000000,-100000000,-100000000);

    QVector<QVector3D> positions = m_parent->positions();
    // loop through all positions, establish lowest/ highest xyz coords
    for (int i = 0 ; i < positions.count(); i++)
    {
        if (positions[i].x() < lowBound.x())
            lowBound.setX(positions[i].x());
        else if (positions[i].x() > highBound.x())
            highBound.setX(positions[i].x());

        if (positions[i].y() < lowBound.y())
            lowBound.setY(positions[i].y());

        else if (positions[i].y() > highBound.y())
            highBound.setY(positions[i].y());

        if (positions[i].z() < lowBound.z())
            lowBound.setZ(positions[i].z());

        else if (positions[i].z() > highBound.z())
            highBound.setZ(positions[i].z());
    }

    QVector3D size = QVector3D(highBound.x() - lowBound.x(),highBound.y() - lowBound.y(),highBound.z() - lowBound.z());
    QVector3D center = QVector3D(lowBound.x()+ size.x()/2, lowBound.y() + size.y()/2,lowBound.z()+ size.z()/2);
    m_bounds = QGLXBoundingBox(center,size);
    m_scale.scale( 820.5f );

    m_initialized = false;
    m_dirty = false;

    m_depthBias = 0.000001f;
}


void NCVNeuronSet::setScale(int scale)
{
    m_scale = QMatrix4x4();
    m_scale.scale(scale);
    m_dirty = true;
}


void NCVNeuronSet::destroy()
{
    if (!m_initialized)
        return;

    m_positionBuffer.destroy();
    m_idBuffer.destroy();
    m_indexBuffer.destroy();
    m_vertexBuffer.destroy();

    for(QMap<QString,NCVAttribute *>::iterator it = m_attributes.begin(); it != m_attributes.end(); it++)
        it.value()->destroy();
    m_initialized = false;
}

bool NCVNeuronSet::dirty()
{
    if(m_dirty)
        return true;
    else if(m_currentAttribute != NULL)
        return m_currentAttribute->dirty();

	return false;
}



QGLXBuffer NCVNeuronSet::positionBuffer()
{
	return m_positionBuffer;
}


QGLXBufferTexture NCVNeuronSet::positionBufferTexture()
{
    return m_positionBufferTexture;
}

void NCVNeuronSet::bind(QGLXCamera camera, bool deselected)
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
        m_indexBuffer.bind(QGLXBuffer::IndexBuffer);

        m_vertexBuffer.bind(QGLXBuffer::ArrayBuffer);
        program->enableAttributeArray( "Vert_Position");
        program->setAttributeBuffer("Vert_Position", GL_FLOAT,  0 ,3, sizeof(QVector3D));
        glVertexAttribDivisor( program->attributeLocation("Vert_Position"), 0);

        m_idBuffer.bind(QGLXBuffer::ArrayBuffer);
        program->enableAttributeArray( "Inst_ID");
        program->setAttributeBuffer("Inst_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
        glVertexAttribDivisor( program->attributeLocation("Inst_ID"), 1);

        m_positionBufferTexture.bind(0);
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

        program->setUniformValue("WVP",camera.projection() * camera.view());
        program->setUniformValue("NearPlane",camera.nearPlane());
        program->setUniformValue("FarPlane",camera.farPlane());
        program->setUniformValue("DepthBias",m_depthBias);
        program->setUniformValue("FarBias",1.0f/(float)log(camera.farPlane()*m_depthBias + 1));
        program->setUniformValue("Deselected", (int)deselected);
        program->setUniformValue("Scale",m_scale);

    }
}



void NCVNeuronSet::bindSilhouettes(QGLXCamera camera, QColor color)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // perform silhouetting
    m_silhouetteProgram.bind();

    m_indexBuffer.bind(QGLXBuffer::IndexBuffer);

    m_vertexBuffer.bind(QGLXBuffer::ArrayBuffer);
    m_silhouetteProgram.enableAttributeArray( "Vert_Position");
    m_silhouetteProgram.setAttributeBuffer("Vert_Position", GL_FLOAT,  0 ,3, sizeof(QVector3D));
    glVertexAttribDivisor( m_silhouetteProgram.attributeLocation("Vert_Position"), 0);

    m_idBuffer.bind(QGLXBuffer::ArrayBuffer);
    m_silhouetteProgram.enableAttributeArray( "Inst_ID");
    m_silhouetteProgram.setAttributeBuffer("Inst_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
    glVertexAttribDivisor( m_silhouetteProgram.attributeLocation("Inst_ID"), 1);

    m_positionBufferTexture.bind(0);
    m_silhouetteProgram.setUniformValue("Inst_Translation", 0);

    m_silhouetteProgram.setUniformValue("WVP",camera.projection() * camera.view());
    m_silhouetteProgram.setUniformValue("Scale",m_scale);
    m_silhouetteProgram.setUniformValue("SilhouetteColor",QVector3D(color.redF(),color.greenF(),color.blueF()));
    m_silhouetteProgram.setUniformValue("CameraDirection",camera.forward());
    m_silhouetteProgram.setUniformValue("NearPlane",camera.nearPlane());
    m_silhouetteProgram.setUniformValue("FarPlane",camera.farPlane());
    m_silhouetteProgram.setUniformValue("DepthBias",m_depthBias);
    m_silhouetteProgram.setUniformValue("FarBias",1.0f/(float)log(camera.farPlane()*m_depthBias + 1));
    m_silhouetteProgram.setUniformValue("SilhouetteDepthMagnification",0.5f);
    m_silhouetteProgram.setUniformValue("SilhouetteMaxAlpha",0.5f);


}



void NCVNeuronSet::release()
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

        program->disableAttributeArray( "Vert_Position" );
        program->disableAttributeArray( "Inst_ID" );
        m_idBuffer.release();
        m_vertexBuffer.release();
        m_indexBuffer.release();
        m_positionBufferTexture.release();

        program->release();
    }
}



void NCVNeuronSet::releaseSilhouettes()
{
    m_silhouetteProgram.disableAttributeArray( "Vert_Position" );
    m_silhouetteProgram.disableAttributeArray( "Inst_ID" );
    m_idBuffer.release();
    m_vertexBuffer.release();
    m_indexBuffer.release();

    m_positionBufferTexture.release();
    m_silhouetteProgram.release();

    glDisable(GL_BLEND);
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
    {
        m_dirty = true;
        m_currentAttribute = m_attributes[name];
    }

}



void NCVNeuronSet::removeAttribute(QString name)
{
    m_attributes.remove(name);

	m_dirty = true;
}



int NCVNeuronSet::count()
{
    return m_parent->count();
}



void NCVNeuronSet::draw()
{
    drawSubset(0,m_parent->count());
}



void NCVNeuronSet::drawSubset(int startElement, int count)
{
	m_dirty = false;
    glDrawElementsInstancedBaseInstance(GL_TRIANGLE_STRIP,14 ,GL_UNSIGNED_INT,0,count , startElement  );
}



void NCVNeuronSet::resolve()
{
    if (!m_initialized)
    {

        m_discreteProgram.addShaderFromSourceFile( QGLShader::Vertex, ":/plugins/visualizer/shaders/neuronDiscrete.vert" );
        m_discreteProgram.addShaderFromSourceFile( QGLShader::Fragment, ":/plugins/visualizer/shaders/discrete.frag" );
        m_discreteProgram.link();

        m_continuousProgram.addShaderFromSourceFile( QGLShader::Vertex, ":/plugins/visualizer/shaders/neuronContinuous.vert" );
        m_continuousProgram.addShaderFromSourceFile( QGLShader::Fragment, ":/plugins/visualizer/shaders/continuous.frag" );
        m_continuousProgram.link();

        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Vertex,":/plugins/visualizer/shaders/neuronSilhouette.vert" );
        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Fragment,":/plugins/visualizer/shaders/silhouette.frag" );
        m_silhouetteProgram.link();

        GLuint componentSize = QGLXTexture::getComponentSize(GL_FLOAT);
        GLenum textureFormat  = QGLXTexture::bufferFormatToTextureFormat(GL_FLOAT,3,componentSize);

        QVector<QVector3D> positions = m_parent->positions();
        m_positionBuffer.create();
        m_positionBuffer.allocate(&positions[0],sizeof(QVector3D) * m_parent->count());

        m_positionBufferTexture.create();
        m_positionBufferTexture.attach(m_positionBuffer,textureFormat);

        QVector3D vertices[8] ={
            QVector3D( -0.5, 0.5,-0.5 ),
            QVector3D(  0.5, 0.5,-0.5 ),
            QVector3D( -0.5, 0.5, 0.5 ),
            QVector3D(  0.5, 0.5, 0.5 ),
            QVector3D( -0.5,-0.5,-0.5 ),
            QVector3D(  0.5,-0.5,-0.5 ),
            QVector3D(  0.5,-0.5, 0.5 ),
            QVector3D( -0.5,-0.5, 0.5 )};


        GLuint indices[14] = {3, 2, 6, 7, 4 ,2, 0,3,1, 6, 5, 4, 1, 0};;

        QVector<GLuint> sysIDs;
        for (int i =0; i < m_parent->count(); i ++)
            sysIDs.append( i+1);

        m_vertexBuffer.create();
        m_vertexBuffer.bind(QGLXBuffer::ArrayBuffer);
        m_vertexBuffer.allocate( &vertices[0], 8 * sizeof(QVector3D));
        m_vertexBuffer.release();

        m_indexBuffer.create();
        m_indexBuffer.bind(QGLXBuffer::IndexBuffer);
        m_indexBuffer.allocate( &indices[0], 14 * sizeof(GLuint));
        m_indexBuffer.release();

        m_idBuffer.create();
        m_idBuffer.bind(QGLXBuffer::ArrayBuffer);
        m_idBuffer.allocate( &sysIDs[0], m_parent->count() * sizeof(GLuint));
        m_idBuffer.release();

        m_initialized = true;
    }

}

void NCVNeuronSet::m_addAttribute(QString name,NCSAttribute * attrib)
{
    if (attrib->type() == Discrete)
        m_attributes[name] = new NCVDiscreteAttribute(dynamic_cast<NCSDiscreteAttribute*>(attrib));
    else
        m_attributes[name] = new NCVContinuousAttribute(dynamic_cast<NCSContinuousAttribute*>(attrib));

    if (m_currentAttribute == NULL)
        m_currentAttribute = m_attributes[name];
    m_dirty = true;
}

void NCVNeuronSet::m_removeAttribute(QString name)
{
    m_attributes.remove(name);
}

