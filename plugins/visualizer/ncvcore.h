#ifndef NCVCORE_H
#define NCVCORE_H

#include "qglx/qglxbuffer.h"
#include "qglx/qglxcamera.h"
#include "qglx/qglxboundingbox.h"
#include "core/ncsattribute.h"


enum SelectionFlag
{
	RenderDeselected = 1,
	CompoundSelection = 2
};

struct Range
{
    int start;
    int end;
	bool operator==(const Range &rhs)
	{
		return start == rhs.start && end == rhs.end;
	}
};

class NCVAttribute: public QObject
{
    Q_OBJECT

public:
    NCVAttribute(NCSAttribute * parent):QObject() {m_parent = parent;}
    virtual QGLShaderProgram * program(){return NULL;}
    virtual void bind(QGLXCamera camera){Q_UNUSED(camera)}
    virtual void release(){}
	virtual bool dirty(){return false;}
    virtual NCSElementType elementType(){return m_parent->elementType();}
    virtual NCSAttributeType type(){return m_parent->type();}
    virtual void resolve(){}
    virtual void destroy() = 0;
    virtual bool reportable() const { return m_parent->reportable(); }

private:
    NCSAttribute * m_parent;
};

struct NCVConvexHull
{
	NCVConvexHull(){}
	NCVConvexHull(GLuint id,Range hullIndexRange,QVector<Range> neuronRanges,QVector<Range> connectionRanges, QGLXBoundingBox bounds)
	{
		this->id = id;
		this->hullIndexRange = hullIndexRange;
		this->neuronRanges = neuronRanges;
		this->connectionRanges = connectionRanges;
		this->bounds = bounds;
	}

	bool show;
	QGLXBoundingBox bounds;
	QVector<Range> neuronRanges;
	QVector<Range> connectionRanges;
	Range hullIndexRange;
	QVector<NCVConvexHull> children;
	GLuint id;
    // NCVElement type elementType     // could help in differentiating Columns from Layers without using multiple NCVGroupingSets
};


#endif // NCVCORE_H
