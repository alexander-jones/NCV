#ifndef ATTRIBUTEWIDGET_H
#define ATTRIBUTEWIDGET_H

#include <QVBoxLayout>
#include <QMessageBox>
#include "combowidget.h"
#include "continuouscolorselector.h"
#include "discretecolorselector.h"
#include "ncvcontinuousattribute.h"
#include "ncvdiscreteattribute.h"
#include <QwwTaskPanel>
#include <QToolButton>
#include "qcustomplot.h"


class AttributeWidget: public QObject
{
    Q_OBJECT
public:
    explicit AttributeWidget(QWidget *parent = 0);
    QWidget * neuronWidget();
    QWidget * connectionWidget();


signals:
    void currentNeuronAttributeSet(QString name);
    void currentConnectionAttributeSet(QString name);

public slots:
    void addContinuousAttribute(QString name, NCVContinuousAttribute *attrib );
    void addDiscreteAttribute(QString name,  NCVDiscreteAttribute * attrib);

private slots:
    void m_neuronColorationChanged(const QString & name);
    void m_connectionColorationChanged(const QString & name);
    void m_currentNeuronAttributeSet(QString name);
    void m_currentConnectionAttributeSet(QString name);
    void m_neuronLinkButtonPressed();
    void m_connectionLinkButtonPressed();


private:

    void m_updateNeuronLinkIcon();
    void m_updateConnectionLinkIcon();
    void m_checkAndFixInconsistantColorations(QString attributeName);
    bool m_discreteAttributeConsistent(QString attributeName);
    bool m_continuousAttributeConsistent(QString attributeName);
    bool m_isSharedAttribute(QString name);

    QSignalMapper *m_connectionMapper, *m_neuronMapper;
    ComboWidget * m_neuronSidebar, * m_connectionSidebar;
    QToolButton * m_neuronLinkButton, *m_connectionLinkButton;
    QIcon  m_linkedIcon,m_unlinkedIcon;
    QMap<QString,bool > m_attributeLinked,m_attributeShared;
    QMap<QString,NCVDiscreteAttribute* > m_neuronDiscreteAttributes;
    QMap<QString,NCVContinuousAttribute* > m_neuronContinuousAttributes;
    QMap<QString,NCVDiscreteAttribute* > m_connectionDiscreteAttributes;
    QMap<QString,NCVContinuousAttribute* > m_connectionContinuousAttributes;

    QMap<QString,DiscreteColorSelector* > m_neuronDiscreteWidgets;
    QMap<QString,ContinuousColorSelector* > m_neuronContinuousWidgets;
    QMap<QString,DiscreteColorSelector* > m_connectionDiscreteWidgets;
    QMap<QString,ContinuousColorSelector* > m_connectionContinuousWidgets;
};

/* Code for constructing real-time plot.
// include this section to fully disable antialiasing for higher performance:

  customPlot->setNotAntialiasedElements(QCP::aeAll);
  QFont font;
  font.setStyleStrategy(QFont::NoAntialias);
  customPlot->xAxis->setTickLabelFont(font);
  customPlot->yAxis->setTickLabelFont(font);
  customPlot->legend->setFont(font);

  customPlot->addGraph(); // blue line
  customPlot->graph(0)->setPen(QPen(Qt::blue));
  customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
  customPlot->graph(0)->setAntialiasedFill(false);
  customPlot->addGraph(); // red line
  customPlot->graph(1)->setPen(QPen(Qt::red));
  customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1));

  customPlot->addGraph(); // blue dot
  customPlot->graph(2)->setPen(QPen(Qt::blue));
  customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(2)->setScatterStyle(QCP::ssDisc);
  customPlot->addGraph(); // red dot
  customPlot->graph(3)->setPen(QPen(Qt::red));
  customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(3)->setScatterStyle(QCP::ssDisc);

  customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
  customPlot->xAxis->setAutoTickStep(false);
  customPlot->xAxis->setTickStep(2);
  customPlot->setupFullAxesBox();

  // make left and bottom axes transfer their Continuouss to right and top axes:
  connect(customPlot->xAxis, SIGNAL(ContinuousChanged(QCPContinuous)), customPlot->xAxis2, SLOT(setContinuous(QCPContinuous)));
  connect(customPlot->yAxis, SIGNAL(ContinuousChanged(QCPContinuous)), customPlot->yAxis2, SLOT(setContinuous(QCPContinuous)));

  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible*/

#endif // ATTRIBUTEWIDGET_H
