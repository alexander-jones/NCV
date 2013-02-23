#ifndef NCVWIDGET_H
#define NCVWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QBoxLayout>
#include "orientationbutton.h"
#include "managementSidebar.h"
#include "ncvcanvas.h"

class NCVWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NCVWidget(QWidget *parent = 0);
    ~NCVWidget();
    
signals:
    
private slots:
    void m_collapseButtonPressed();
    void m_reportFatalError();
    void m_newFrameReceived();
    void m_createNetwork(int numNeurons,int numConnections, QVector3D worldSize);


private:
    bool m_collapsed;
    QBoxLayout * m_layout;
    ManagementSidebar * m_managementSidebar;
    OrientationButton * m_collapseButton;
    QString m_expandText, m_collapseText;
    QTime m_timer;
    NCVNeuronSet * m_neurons;
    NCVConnectionSet * m_connections;
    QMap<QString, NCVDiscreteAttribute *> m_neuronDiscreteAttributes,m_connectionDiscreteAttributes;
    QMap<QString, NCVContinuousAttribute *> m_neuronContinuousAttributes,m_connectionContinuousAttributes;
    int m_frameCount;
    NCVCanvas * m_canvas;

};

#endif // NCVWIDGET_H
