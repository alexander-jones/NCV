#ifndef NCVBUILDWIDGET_H
#define NCVBUILDWIDGET_H

#include <QWidget>

#include "qglxpainter.h"
#include "ncvneuronset.h"
#include "ncvconnectionset.h"
#include "ncvdiscreteattribute.h"
#include "ncvcontinuousattribute.h"
#include "continuouscolorselector.h"

class NCVBuildWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NCVBuildWidget(QWidget *parent = 0);
    
    //TEMPORARY METHOD. USED IN ABSENCE OF UI TO BUILD NETWORK
    void createNetwork(int numNeurons, int numConnections,QVector3D worldSize);
signals:
    void neuronsFinalized(NCVNeuronSet *);
    void connectionsFinalized(NCVConnectionSet *);
public slots:
    
private:
    NCVNeuronSet * m_neurons;
    NCVConnectionSet * m_connections;
    QMap<QString,NCVDiscreteAttribute *> m_discreteNeuronAttributes,m_discreteConnectionAttributes;
    QMap<QString,NCVContinuousAttribute *> m_continuousNeuronAttributes,m_continuousConnectionAttributes;

};

#endif // NCVBUILDWIDGET_H
