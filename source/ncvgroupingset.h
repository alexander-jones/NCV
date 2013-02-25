#ifndef NCVGROUPINGSET_H
#define NCVGROUPINGSET_H
#include "ncvcore.h"
#include "ncvconnectionset.h"


class NCVGroupingSet:QObject
{
    Q_OBJECT
public:
    NCVGroupingSet(NCVNeuronSet *neurons, NCVConnectionSet * connections,QVector<GLuint> hullIndices,QVector<NCVConvexHull> columnHulls );
	void prepare(QMatrix4x4 WVP);
    void bind(NCVElementType type,QGLXCamera camera,bool deselected = false);
    void bindSilhouettes(NCVElementType type,QGLXCamera camera);
    int count();
    bool dirty();
    void draw(NCVElementType type);
    void drawSubset(NCVElementType type,int startElement, int count);
    void release(NCVElementType type);
    void resolve();
    void releaseSilhouettes(NCVElementType type);
	QMap<QString,NCVAttribute *> attributes();
	NCVNeuronSet * neurons();
	NCVConnectionSet * connections();


public slots:
    void addAttribute(QString name, NCVAttribute * attribute);
    void setCurrentAttribute(QString name);
    void removeAttribute(QString name);
private:
	void m_performFrustumTest(QMatrix4x4 WVP,NCVConvexHull hull);
	void m_disperseShowFlag(NCVConvexHull hull,bool flag);
	void m_draw(NCVElementType type, NCVConvexHull hull);
	void m_drawSubset(NCVElementType type,int startElement, int count, NCVConvexHull hull);
    void m_fillIDVector(NCVConvexHull hull, QVector<GLuint> & idVec);

    int m_count;
    bool m_initialized,m_dirty;
    QVector<GLuint> m_hullIndices;
    QVector<NCVConvexHull> m_columnHulls;
    QGLXBuffer m_neuronIdBuffer,m_idBuffer;
    QMatrix4x4 m_scale;
    QMap<QString,NCVAttribute *> m_attributes;
    NCVAttribute * m_currentAttribute;
    NCVConnectionSet * m_connections;
    NCVNeuronSet * m_neurons;
    QGLShaderProgram m_silhouetteProgram;
};

#endif // NCVGROUPINGSET_H
