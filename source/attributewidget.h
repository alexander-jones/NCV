#ifndef ATTRIBUTEWIDGET_H
#define ATTRIBUTEWIDGET_H

#include <QVBoxLayout>
#include <QMessageBox>
#include "combowidget.h"
#include "colorrangewidget.h"
#include "colorbitwidget.h"
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
    void neuronRangeColorationChanged(QString,QRgb * range,int width);
    void neuronBitColorationChanged(QString,QColor offColor, QColor onColor);
    void connectionRangeColorationChanged(QString,QRgb * range,int width);
    void connectionBitColorationChanged(QString,QColor offColor, QColor onColor);

public slots:
    void addNeuronRangeAttribute(QString name, float minVal, float maxVal);
    void addNeuronBitAttribute(QString name, QColor offColor, QColor onColor);
    void addConnectionRangeAttribute(QString name, float minVal, float maxVal);
    void addConnectionBitAttribute(QString name, QColor offColor, QColor onColor);

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
    bool m_flagAttributeConsistent(QString attributeName);
    bool m_rangeAttributeConsistent(QString attributeName);
    bool m_isSharedAttribute(QString name);

    QSignalMapper *m_connectionMapper, *m_neuronMapper;
    ComboWidget * m_neuronSidebar, * m_connectionSidebar;
    QWidget * m_widget;
    QVBoxLayout * m_layout;
    QToolButton * m_neuronLinkButton, *m_connectionLinkButton;
    QIcon  m_linkedIcon,m_unlinkedIcon;
    QMap<QString,bool > m_attributeLinked,m_attributeShared;
    QMap<QString,ColorBitWidget* > m_neuronFlagWidgets;
    QMap<QString,ColorRangeWidget* > m_neuronRangeWidgets;
    QMap<QString,ColorBitWidget* > m_connectionFlagWidgets;
    QMap<QString,ColorRangeWidget* > m_connectionRangeWidgets;
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

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible*/

#endif // ATTRIBUTEWIDGET_H
