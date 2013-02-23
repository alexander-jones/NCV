#ifndef NCVNEURONSET_H
#define NCVNEURONSET_H
#include "ncvcore.h"
#include "qglxtexture2d.h"
#include "qglxboundingbox.h"

class NCVNeuronSet: public NCVElementSet
{

    Q_OBJECT
    friend class NCVConnectionSet;
public:
    NCVNeuronSet(QVector<QVector3D> positions);

    void bind(QGLXCamera camera,bool deselected = false);
    void bindSilhouettes(QGLXCamera camera);
    int count();
    void draw();
    void drawSubset(int startElement, int count);

    void release();
    void releaseSilhouettes();
    QGLXBoundingBox bounds();
    QMap<QString,NCVAttribute *> attributes();

public slots:
    void addAttribute(QString name, NCVAttribute * attribute);
    void setCurrentAttribute(QString name);
    void removeAttribute(QString name);

private:
    void m_resolve();

    int m_count;
    QGLXBuffer m_indexBuffer, m_normalBuffer, m_vertexBuffer, m_idBuffer;
    QMatrix4x4 m_scale;
    bool m_initialized;
    QMap<QString,NCVAttribute *> m_attributes;
    NCVAttribute * m_currentAttribute;
    QGLXBoundingBox m_bounds;
    QVector<QVector3D> m_positions;
    QGLXBuffer m_positionBuffer;
    QGLShaderProgram m_silhouetteProgram;
};

#endif // NCVNEURONSET_H
