#ifndef NCVWIDGET_H
#define NCVWIDGET_H

#include "core/gui/plugin-interfaces/ncssubscriberwidgetplugin.h"
#include "ncvrendertool.h"
#include "ncvcanvas.h"
#include <QxtPushButton>
#include <QWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QBoxLayout>

class NCVWidget : public NCSSubscriberWidgetPlugin
{
    Q_OBJECT
public:
    explicit NCVWidget(QString projectDir,QWidget *parent = 0);
    ~NCVWidget();
    QIcon icon();
    QString title();

public slots:
    void setNeurons(NCSNeuronSet * neurons);
    void setConnections(NCSConnectionSet * connections);
    void setSelection(QVector<NCVElementRange> selection, NCVSelectionFlag flags);
    void initialize();
    void cleanup();

signals:
    void selectionChanged(QVector<NCVElementRange> selection, NCVSelectionFlag flags);

private slots:
    void m_collapseButtonPressed();
    void m_reportFatalError();
    void m_newFrameReceived();
	void m_onDeselectAll();
	void m_onRenderDeselectionSet(bool on);
	void m_onCompoundSelectionSet(bool on);

	
private:
    QString m_projectDir;
    QVector<NCVElementRange> m_currentSelection;
    NCVSelectionFlag m_selectionFlags;
    QVBoxLayout * m_selectionLayout;
    QWidgetVector * m_renderDeselectedWidget, *m_compoundSelectionWidget;
	QPushButton * m_deselectAllButton;
	QSwitch * m_renderDeselectedSwitch,*m_compoundSelectionSwitch;
	NCVRenderTool * m_renderTool;
    bool m_collapsed;
    QBoxLayout * m_layout;
    QLabel * m_framesPerSecond;
    QxtPushButton * m_collapseButton;
    QString m_expandText, m_collapseText;
    QTime m_timer;
    NCVNeuronSet * m_neurons;
    NCVConnectionSet * m_connections;
    int m_frameCount;
    NCVCanvas * m_canvas;

};

#endif // NCVWIDGET_H
