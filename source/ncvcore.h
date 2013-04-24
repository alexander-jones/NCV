#ifndef NCVCORE_H
#define NCVCORE_H

#include "qglxbuffer.h"
#include "qglxcamera.h"
#include "qglxboundingbox.h"


enum NCVElementType
{
    Neuron,
    Connection,
    Grouping
};
enum NCVAttributeType
{
    Discrete,
    Continuous
};

enum SelectionFlag
{
	RenderDeselected = 1,
	CompoundSelection = 2
};


struct NeuronConnection
{
    NeuronConnection(){}
    NeuronConnection(GLuint in,GLuint out)
    {
        inNeuron = in;
        outNeuron = out;
    }

    GLuint inNeuron,outNeuron;
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

class NCVAttribute
{
public:
    NCVAttribute(): m_reportable(true) {}
    virtual QGLShaderProgram * program(){return NULL;}
    virtual void bind(QGLXCamera camera){Q_UNUSED(camera)}
    virtual void release(){}
	virtual bool dirty(){return false;}
    virtual NCVElementType elementType(){return Neuron;}
    virtual NCVAttributeType type(){return Continuous;}
    virtual void resolve(){}
    virtual void destroy() = 0;
    virtual bool reportable() const { return m_reportable; }
    virtual void setReportable(bool value) { m_reportable = value; }
private:
    bool m_reportable;
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
