#include "ncvgroupingset.h"

NCVGroupingSet::NCVGroupingSet(NCVNeuronSet *neurons, NCVConnectionSet * connections, QVector<GLuint> hullIndices,QVector<NCVConvexHull> columnHulls )
{
    m_neurons = neurons;
    m_connections = connections;
    m_columnHulls = columnHulls;
    m_hullIndices = hullIndices;
    m_initialized = false;
}

void NCVGroupingSet::prepare(QMatrix4x4 WVP)
{
	for (QVector<NCVConvexHull>::iterator it = m_columnHulls.begin(); it != m_columnHulls.end(); it ++ )
		m_performFrustumTest(WVP,*it);
}
void NCVGroupingSet::bind(NCVElementType type,QGLXCamera camera,bool deselected)
{
    if (type == Neuron)
        m_neurons->bind(camera,deselected);
    else if (type == Connection)
        m_connections->bind(camera,deselected);

    else if (type == Grouping )
    {

        m_currentAttribute->bind(camera);
        QGLShaderProgram * program = m_currentAttribute->program();

        glActiveTexture(GL_TEXTURE2);
        m_neurons->positionBuffer().bind();
        program->setUniformValue("Inst_Translation", 2);

        m_neuronIdBuffer.bind();
        program->enableAttributeArray( "Neuron_ID");
        program->setAttributeBuffer("Neuron_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));

        m_idBuffer.bind();
        program->enableAttributeArray( "Inst_ID");
        program->setAttributeBuffer("Inst_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
        glVertexAttribDivisor(program->attributeLocation("Inst_ID"), 0);

    }

}

void NCVGroupingSet::bindSilhouettes(NCVElementType type,QGLXCamera camera)
{

    if (type == Neuron)
        m_neurons->bindSilhouettes(camera);
    else if (type == Connection)
        m_connections->bindSilhouettes(camera);

    else if (type == Grouping )
    {

        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(1,1.5);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        m_silhouetteProgram.bind();
        glActiveTexture(GL_TEXTURE0);
        m_neurons->positionBuffer().bind();
        m_silhouetteProgram.setUniformValue("Inst_Translation", 0);

        m_silhouetteProgram.setUniformValue("WVP",camera.projection() * camera.view());

        m_neuronIdBuffer.bind();
        m_silhouetteProgram.enableAttributeArray( "Neuron_ID");
        m_silhouetteProgram.setAttributeBuffer("Neuron_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
        glVertexAttribDivisor( m_silhouetteProgram.attributeLocation("Neuron_ID"), 0);

        m_idBuffer.bind();
        m_silhouetteProgram.enableAttributeArray( "Inst_ID");
        m_silhouetteProgram.setAttributeBuffer("Inst_ID", GL_FLOAT,  0 ,1, sizeof(GLuint));
        glVertexAttribDivisor( m_silhouetteProgram.attributeLocation("Inst_ID"), 0);


        m_silhouetteProgram.setUniformValue("SilhouetteColor",QVector3D(0,0,0));
        m_silhouetteProgram.setUniformValue("CameraDirection",camera.forward());
        m_silhouetteProgram.setUniformValue("ZFar",camera.farPlane());
        m_silhouetteProgram.setUniformValue("ZNear",camera.nearPlane());
        m_silhouetteProgram.setUniformValue("ZStop",camera.farPlane()/2);
        m_silhouetteProgram.setUniformValue("DepthExponent",2.0f);
        m_silhouetteProgram.setUniformValue("MaxAlpha",0.5f);

    }

}
int NCVGroupingSet::count()
{
    return m_count;

}
void NCVGroupingSet::draw(NCVElementType type)
{
    m_dirty = false;
    for (QVector<NCVConvexHull>::iterator it = m_columnHulls.begin(); it != m_columnHulls.end(); it ++ )
		m_draw(type,*it);
}

NCVNeuronSet * NCVGroupingSet::neurons()
{
	return m_neurons;
}
NCVConnectionSet * NCVGroupingSet::connections()
{
	return m_connections;
}
void NCVGroupingSet::drawSubset(NCVElementType type,int startElement, int count)
{
    m_dirty = false;
	for (QVector<NCVConvexHull>::iterator it = m_columnHulls.begin(); it != m_columnHulls.end(); it ++ )
		m_drawSubset(type,startElement,count,*it);

}
void NCVGroupingSet::release(NCVElementType type)
{
    if (type == Neuron)
        m_neurons->releaseSilhouettes();
    else if (type == Connection)
        m_connections->releaseSilhouettes();

    else if (type == Grouping )
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
void NCVGroupingSet::releaseSilhouettes(NCVElementType type)
{

    if (type == Neuron)
        m_neurons->releaseSilhouettes();
    else if (type == Connection)
        m_connections->releaseSilhouettes();

    else if (type == Grouping )
    {
        m_silhouetteProgram.disableAttributeArray( "Neuron_ID" );
        m_silhouetteProgram.disableAttributeArray( "Inst_ID" );
        m_neuronIdBuffer.release();
        m_idBuffer.release();
        m_neurons->positionBuffer().release();
        m_silhouetteProgram.release();

        glDisable(GL_BLEND);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
}
QMap<QString,NCVAttribute *> NCVGroupingSet::attributes()
{
    return m_attributes;
}

bool NCVGroupingSet::dirty()
{
    if (m_dirty || m_neurons->dirty() || m_connections->dirty())
        return true;
    else if(m_currentAttribute != NULL)
        if (m_currentAttribute->dirty())
                return true;


    return false;

}
void NCVGroupingSet::addAttribute(QString name,NCVAttribute * attribute)
{
    m_attributes[name] = attribute;
    if (m_attributes.count() ==1)
        m_currentAttribute = attribute;
    m_dirty = true;
}

void NCVGroupingSet::setCurrentAttribute(QString name)
{
    if (m_attributes.contains(name))
        m_currentAttribute = m_attributes[name];
    m_dirty = true;
}
void NCVGroupingSet::removeAttribute(QString name)
{
    m_attributes.remove(name);
    m_dirty = true;
}

void NCVGroupingSet::resolve()
{
    if (!m_initialized)
    {
        //create grouping silhouette program.

        /* m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Vertex,":/shaders/connectionSilhouette.vert");
        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Geometry,":/shaders/connectionSilhouette.geom");
        m_silhouetteProgram.addShaderFromSourceFile(QGLShader::Fragment,":/shaders/silhouette.frag");
        m_silhouetteProgram.link();*/


        m_idBuffer.create();
        m_idBuffer.bind();
        m_idBuffer.allocate( &m_hullIndices[0], m_hullIndices.count() * sizeof(GLuint));
        m_idBuffer.release();

        QVector<GLuint> neuronIds;
        neuronIds.resize(m_hullIndices.count() * sizeof(GLuint));

        for (QVector<NCVConvexHull>::iterator it = m_columnHulls.begin(); it != m_columnHulls.end(); it ++ )
           m_fillIDVector(*it,neuronIds);

        m_neuronIdBuffer.create();
        m_neuronIdBuffer.bind();
        m_neuronIdBuffer.allocate( &neuronIds[0], neuronIds.count() * sizeof(GLuint));
        m_neuronIdBuffer.release();
        m_initialized = true;
    }

}


void NCVGroupingSet::m_performFrustumTest(QMatrix4x4 WVP,NCVConvexHull hull)
{

	/* test bounding box against wvp to compute hull.show


	*/
	if (hull.show)
	{
		for (QVector<NCVConvexHull>::iterator it = hull.children.begin(); it != hull.children.end(); it ++ )
			m_disperseShowFlag(*it,false);
	}
	else
		for (QVector<NCVConvexHull>::iterator it = hull.children.begin(); it != hull.children.end(); it ++ )
			m_performFrustumTest( WVP,*it);
	

}
void NCVGroupingSet::m_disperseShowFlag(NCVConvexHull hull,bool flag)
{
	hull.show = flag;
	for (QVector<NCVConvexHull>::iterator it = hull.children.begin(); it != hull.children.end(); it ++ )
		m_disperseShowFlag(*it,flag);

}

void NCVGroupingSet::m_draw(NCVElementType type, NCVConvexHull hull)
{
	if (hull.show)
	{
		if (type == Grouping )
		{
			int offset = hull.hullIndexRange.start * sizeof(GLuint);
			glDrawElements(GL_TRIANGLES, hull.hullIndexRange.end  - hull.hullIndexRange.start,GL_UNSIGNED_INT,&offset);
		}
	}
	else
	{

		if (type == Neuron)
		{
			for (QVector<Range>::iterator it = hull.neuronRanges.begin(); it != hull.neuronRanges.end(); it ++ )
				m_neurons->drawSubset(it->start,it->end - it->start);
		}
		else if (type == Connection)
			for (QVector<Range>::iterator it = hull.neuronRanges.begin(); it != hull.neuronRanges.end(); it ++ )
				m_connections->drawSubset(it->start,it->end - it->start);


		for (QVector<NCVConvexHull>::iterator it = hull.children.begin(); it != hull.children.end(); it ++ )
			m_draw(type,*it);
	}



}
void NCVGroupingSet::m_drawSubset(NCVElementType type,int startElement, int count, NCVConvexHull hull)
{

	if (type == Neuron)
		m_neurons->drawSubset(startElement,count);
	
	else if (type == Connection)
		m_connections->drawSubset(startElement,count);
	else if (type == Grouping)
	{
		// complicated, requires that the convex hull tree is ordered by id  and also requires a partial traversal of the convex hull tree
		// basically, we need a conglomeration of hull's hullIndexRange values (will be put in totalHullIndexRange)
		Range totalHullIndexRange;
		int offset =  totalHullIndexRange.start * sizeof(GLuint);

		glDrawElements(GL_TRIANGLES, totalHullIndexRange.end  - totalHullIndexRange.start,GL_UNSIGNED_INT,&offset);

	}


}
void NCVGroupingSet::m_fillIDVector(NCVConvexHull hull, QVector<GLuint> & idVec)
{
	for (int i = hull.hullIndexRange.start; i < hull.hullIndexRange.end; i ++)
		idVec[i] = hull.id;

	for (QVector<NCVConvexHull>::iterator it = hull.children.begin(); it != hull.children.end(); it ++ )
		m_fillIDVector(hull,idVec);
}
