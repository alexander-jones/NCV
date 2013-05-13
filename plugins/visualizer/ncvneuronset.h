#ifndef NCVNEURONSET_H
#define NCVNEURONSET_H

#include "qglx/qglxtexture2d.h"
#include "qglx/qglxboundingbox.h"
#include "core/ncsneuronset.h"
#include "ncvcontinuousattribute.h"
#include "ncvdiscreteattribute.h"

enum NCVSelectionFlag
{
    RenderDeselected = 1,
    CompoundSelection = 2
};

struct NCVElementRange
{
    int start;
    int end;
    bool operator==(const NCVElementRange &rhs)
    {
        return start == rhs.start && end == rhs.end;
    }
};


class NCVNeuronSet:public QObject
{
    Q_OBJECT
public:
    NCVNeuronSet(NCSNeuronSet * ncsParent,QObject * parent = 0);
    void bind(QGLXCamera camera,bool deselected = false);
    void bindSilhouettes(QGLXCamera camera, QColor color = QColor(0,0,0));
    int count();
    void destroy();
    void draw();
    void drawSubset(int startElement, int count);
	bool dirty();
    void resolve();
    void release();
    void releaseSilhouettes();
	QGLXBuffer positionBuffer();
    QGLXBufferTexture positionBufferTexture();
    QGLXBoundingBox bounds();
    QMap<QString,NCVAttribute *> attributes();
    NCVAttribute* getCurrentAttribute();

public slots:
    void setScale(int setScale);
    void addAttribute(QString name, NCVAttribute * attribute);
    void setCurrentAttribute(QString name);
    void removeAttribute(QString name);

private slots:
    void m_addAttribute(QString name,NCSAttribute * attrib);
    void m_removeAttribute(QString name);

private:
    NCSNeuronSet * m_parent;
    QGLXBuffer m_indexBuffer, m_vertexBuffer, m_idBuffer;
    QMatrix4x4 m_scale;
    bool m_initialized,m_dirty;
    float m_depthBias;
    QMap<QString,NCVAttribute *> m_attributes;
    NCVAttribute * m_currentAttribute;
    QGLXBoundingBox m_bounds;
    QGLXBuffer m_positionBuffer;
    QGLXBufferTexture m_positionBufferTexture;
    QGLShaderProgram m_silhouetteProgram , m_continuousProgram, m_discreteProgram;
};


#endif // NCVNEURONSET_H
