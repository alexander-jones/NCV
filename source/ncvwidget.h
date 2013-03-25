#ifndef NCVWIDGET_H
#define NCVWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QBoxLayout>
#include "orientationbutton.h"
#include "ncvrendertool.h"
#include "ncvcanvas.h"
#include "qwidgetvector.h"

class NCVWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NCVWidget(QWidget *parent = 0);
    ~NCVWidget();

public slots:
    void setNeurons(NCVNeuronSet * neurons);
    void setConnections(NCVConnectionSet * neurons);
	void setSelection(QVector<Range> selection, SelectionFlag flags);

signals:
	void selectionChanged(QVector<Range> selection, SelectionFlag flags);

private slots:
    void m_collapseButtonPressed();
    void m_reportFatalError();
    void m_newFrameReceived();
	void m_onDeselectAll();
	void m_onRenderDeselectionSet(bool on);
	void m_onCompoundSelectionSet(bool on);

	
private:
    QWidgetVector * m_pluginWidget;
	QVector<Range> m_currentSelection;
	SelectionFlag m_selectionFlags;
	QWidgetVector * m_selectionWidget,* m_renderDeselectedWidget, *m_compoundSelectionWidget;
	QPushButton * m_deselectAllButton;
	QSwitch * m_renderDeselectedSwitch,*m_compoundSelectionSwitch;
	NCVRenderTool * m_renderTool;
    bool m_collapsed;
    QBoxLayout * m_layout;
    QLabel * m_framesPerSecond;
    QTabWidget * m_ncvSidebar;
    OrientationButton * m_collapseButton;
    QString m_expandText, m_collapseText;
    QTime m_timer;
    NCVNeuronSet * m_neurons;
    NCVConnectionSet * m_connections;
    int m_frameCount;
    NCVCanvas * m_canvas;

};

#endif // NCVWIDGET_H
