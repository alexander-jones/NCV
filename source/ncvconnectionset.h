#ifndef NCVCONNECTIONSET_H
#define NCVCONNECTIONSET_H
#include "ncvcore.h"
#include "ncvneuronset.h"

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
class NCVConnectionSet: public NCVElementSet
{

    Q_OBJECT
public:

    NCVConnectionSet(NCVNeuronSet * neurons,QVector<NeuronConnection> connections);


    void bind(QGLXCamera camera,bool deselected = false);
    void bindSilhouettes(QGLXCamera camera);
    int count();
    void draw();
    void drawSubset(int startElement, int count);
    void release();
    void releaseSilhouettes();
    QMap<QString,NCVAttribute *> attributes();

public slots:
    void addAttribute(QString name, NCVAttribute * attribute);
    void setCurrentAttribute(QString name);
    void removeAttribute(QString name);


private:
    void m_resolve();

    int m_count;
    QGLXBuffer m_neuronIdBuffer, m_idBuffer;
    QMatrix4x4 m_scale;
    bool m_initialized;
    QMap<QString,NCVAttribute *> m_attributes;
    NCVAttribute * m_currentAttribute;
    QVector<NeuronConnection> m_connections;
    NCVNeuronSet * m_neurons;
    QGLShaderProgram m_silhouetteProgram;
};

#endif // NCVCONNECTIONSET_H
