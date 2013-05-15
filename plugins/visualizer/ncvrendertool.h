#ifndef NCVRENDERTOOL_H
#define NCVRENDERTOOL_H

#include "ncvneuronset.h"
#include "ncvconnectionset.h"
#include "ncvdiscreteattribute.h"
#include "ncvcontinuousattribute.h"
#include "core/gui/utilities/continuouscolorselector.h"
#include "core/gui/utilities/discretecolorselector.h"
#include "core/gui/utilities/qswitch.h"
#include "core/gui/utilities/qwidgetvector.h"
#include "core/gui/utilities/qgroupvector.h"
#include <QWidget>
#include <QBoxLayout>
#include <QToolButton>

class NCVRenderTool : public QGroupVector
{
    Q_OBJECT
public:
    explicit NCVRenderTool(QWidget *parent = 0);


public slots:
    void setNeurons(NCVNeuronSet * neurons);
    void setConnections(NCVConnectionSet * neurons);

signals:
	void neuronRenderSet(bool set);
	void connectionRenderSet(bool set);

private slots:
    void m_neuronRenderSwitched(bool on);
    void m_connectionRenderSwitched(bool on);
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
    QSwitch * m_renderNeuronSwitch, * m_renderConnectionSwitch;
    QVBoxLayout * m_neuronLayout, *m_connectionLayout;
    QWidgetVector *m_renderNeuronVector, *m_renderConnectionVector;
    QSignalMapper *m_discreteNeuronMapper, *m_discreteConnectionMapper,*m_continuousNeuronMapper,*m_continuousConnectionMapper;
    ComboWidget * m_neuronAttributeComboWidget, * m_connectionAttributeComboWidget;
    QToolButton * m_neuronLinkButton, *m_connectionLinkButton;
    QIcon  m_linkedIcon,m_unlinkedIcon;
    QMap<QString,DiscreteColorSelector* > m_neuronDiscreteWidgets,m_connectionDiscreteWidgets;
    QMap<QString,ContinuousColorSelector* > m_neuronContinuousWidgets,m_connectionContinuousWidgets;
    QMap<QString,bool > m_attributeLinked,m_attributeShared;
    QBoxLayout * m_layout;
    NCVNeuronSet * m_neurons;
    NCVConnectionSet * m_connections;
    QSlider * m_neuronScaleSlider, * m_connectionScaleSlider;

};

#endif // NCVRENDERTOOL_H
