#ifndef NCVWIDGET_H
#define NCVWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QBoxLayout>
#include "orientationbutton.h"
#include "ncvsidebar.h"
#include "continuouscolorselector.h"
#include <QToolButton>
#include "ncvcanvas.h"

class NCVWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NCVWidget(QWidget *parent = 0);
    ~NCVWidget();

public slots:
    void setNeurons(NCVNeuronSet * neurons);
    void setConnections(NCVConnectionSet * neurons);
signals:

private slots:
    void m_collapseButtonPressed();
    void m_reportFatalError();
    void m_newFrameReceived();
    void m_currentNeuronAttributeSet(QString name);
    void m_currentConnectionAttributeSet(QString name);
    void m_neuronLinkButtonPressed();
    void m_connectionLinkButtonPressed();
    void m_discreteNeuronColorationChanged( QString name);
    void m_continuousNeuronColorationChanged( QString name);
    void m_discreteConnectionColorationChanged( QString name);
    void m_continuousConnectionColorationChanged( QString name);


private:
    void m_checkAndFixInconsistantColorations(QString attributeName);
    void m_updateNeuronLinkIcon();
    void m_updateConnectionLinkIcon();
    void m_createDiscreteAttributeColorWidget(QString name,NCVDiscreteAttribute *attribute);
    void m_createContinuousAttributeColorWidget(QString name,NCVContinuousAttribute *attribute);
    bool m_discreteAttributeConsistent(QString attributeName);
    bool m_continuousAttributeConsistent(QString attributeName);

    bool m_collapsed;
    QSignalMapper *m_discreteNeuronMapper, *m_discreteConnectionMapper,*m_continuousNeuronMapper,*m_continuousConnectionMapper;

    ComboWidget * m_neuronSidebar, * m_connectionSidebar;
    QToolButton * m_neuronLinkButton, *m_connectionLinkButton;
    QIcon  m_linkedIcon,m_unlinkedIcon;
    QMap<QString,DiscreteColorSelector* > m_neuronDiscreteWidgets,m_connectionDiscreteWidgets;
    QMap<QString,ContinuousColorSelector* > m_neuronContinuousWidgets,m_connectionContinuousWidgets;

    QMap<QString,bool > m_attributeLinked,m_attributeShared;

    QBoxLayout * m_layout;
    QLabel * m_framesPerSecond;
    NCVSidebar * m_ncvSidebar;
    OrientationButton * m_collapseButton;
    QString m_expandText, m_collapseText;
    QTime m_timer;
    NCVNeuronSet * m_neurons;
    NCVConnectionSet * m_connections;
    int m_frameCount;
    NCVCanvas * m_canvas;

};

#endif // NCVWIDGET_H
