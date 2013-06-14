#ifndef NCSCONNECTIONSET_H
#define NCSCONNECTIONSET_H

#include <QObject>
#include <QMap>
#include <QVector3D>
#include "ncsneuronset.h"

struct NCSConnection
{
    NCSConnection(){}
    NCSConnection(unsigned int inNeuron,unsigned int outNeuron)
    {
        this->inNeuron = inNeuron;
        this->outNeuron = outNeuron;
    }

    unsigned int inNeuron,outNeuron;
};

class NCSConnectionSet : public QObject
{
    Q_OBJECT
public:
    NCSConnectionSet(NCSNeuronSet * neurons,QVector<NCSConnection> connections,QObject *parent  = 0);
    QMap<QString,NCSAttribute *> attributes();
    QVector<NCSConnection> connections();
    int count();
    NCSNeuronSet* neurons();

public slots:
    void addAttribute(QString name, NCSAttribute * attribute);
    void removeAttribute(QString name);

signals:
    void attributeAdded(QString name, NCSAttribute * attribute );
    void attributeRemoved(QString name);

private:
    int m_count;
    QMap<QString,NCSAttribute *> m_attributes;
    QVector<NCSConnection> m_connections;
    NCSNeuronSet * m_neurons;
};


#endif // NCSCONNECTIONSET_H
