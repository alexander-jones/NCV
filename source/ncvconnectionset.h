#ifndef NCVCONNECTIONSET_H
#define NCVCONNECTIONSET_H
#include "ncvcore.h"
#include "ncvneuronset.h"

class NCVConnectionSet:public QObject
{

    Q_OBJECT
public:

    NCVConnectionSet(NCVNeuronSet * neurons,QVector<NeuronConnection> connections);
    void bind(QGLXCamera camera,bool deselected = false);
    void bindSilhouettes(QGLXCamera camera, QColor color = QColor(0,0,0));
    int count();
	void draw();
	bool dirty();
    void drawSubset(int startElement, int count);
    void resolve();
    void release();
    void releaseSilhouettes();
	NCVNeuronSet * neurons();
    QMap<QString,NCVAttribute *> attributes();
    NCVAttribute* getCurrentAttribute();

public slots:
    void setScale(int setScale);
    void addAttribute(QString name, NCVAttribute * attribute);
    void setCurrentAttribute(QString name);
    void removeAttribute(QString name);


private:

    int m_count;
    QGLXBuffer m_neuronIdBuffer, m_idBuffer, m_positionBuffer, m_normalBuffer, m_expandedIDBuffer;
    GLuint m_transformFeedback;
    QMatrix4x4 m_scale;
    bool m_initialized,m_dirty;
    QMap<QString,NCVAttribute *> m_attributes;
    NCVAttribute * m_currentAttribute;
    QVector<NeuronConnection> m_connections;
    NCVNeuronSet * m_neurons;
    QGLShaderProgram m_silhouetteProgram, m_expansionProgram;
};

#endif // NCVCONNECTIONSET_H
