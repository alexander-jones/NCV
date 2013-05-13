#ifndef NCVCONNECTIONSET_H
#define NCVCONNECTIONSET_H
#include "core/ncsconnectionset.h"
#include "ncvcontinuousattribute.h"
#include "ncvdiscreteattribute.h"
#include "ncvneuronset.h"

class NCVConnectionSet:public QObject
{

    Q_OBJECT
public:

    NCVConnectionSet(NCSConnectionSet * ncsParent,NCVNeuronSet * neurons,QObject * parent = 0);
    void bind(QGLXCamera camera,bool deselected = false);
    void bindSilhouettes(QGLXCamera camera, QColor color = QColor(0,0,0));
    int count();
    void destroy();
	void draw();
	bool dirty();
    void drawSubset(int startElement, int count);
    void resolve();
    void release();
    void releaseSilhouettes();
    QMap<QString,NCVAttribute *> attributes();
    NCVAttribute* getCurrentAttribute();

public slots:
    void setScale(int setScale);
    void setCurrentAttribute(QString name);

private slots:
    void m_addAttribute(QString name,NCSAttribute * attrib);
    void m_removeAttribute(QString name);

private:
    QGLXBuffer m_neuronIdBuffer, m_idBuffer, m_positionBuffer, m_normalBuffer, m_expandedIDBuffer;
    GLuint m_transformFeedback;
    NCSConnectionSet * m_parent;
    NCVNeuronSet * m_neurons;
    QMatrix4x4 m_scale;
    bool m_initialized,m_dirty;
    float m_depthBias;
    QMap<QString,NCVAttribute *> m_attributes;
    NCVAttribute * m_currentAttribute;
    QGLShaderProgram m_silhouetteProgram, m_continuousProgram, m_discreteProgram, m_expansionProgram;
};

#endif // NCVCONNECTIONSET_H
