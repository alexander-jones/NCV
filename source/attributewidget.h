#ifndef ATTRIBUTEWIDGET_H
#define ATTRIBUTEWIDGET_H

#include <QVBoxLayout>
#include "sidebar.h"
#include "collapsiblewidget.h"
#include "colorrangewidget.h"
#include "colorbitwidget.h"
class AttributeWidget : public CollapsibleWidget
{
    Q_OBJECT
public:
    explicit AttributeWidget(QString text,QWidget *parent = 0);
    
signals:
    void currentNeuronAttributeSet(QString name);
    void currentConnectionAttributeSet(QString name);
    void neuronRangeColorationChanged(QRgb * range);
    void neuronBitColorationChanged(QColor offColor, QColor onColor);
    void connectionRangeColorationChanged(QRgb * range);
    void connectionBitColorationChanged(QColor offColor, QColor onColor);
    
public slots:
    void addNeuronRangeAttribute(QString name, float minVal, float maxVal);
    void addNeuronBitAttribute(QString name, QColor offColor, QColor onColor);
    void addConnectionRangeAttribute(QString name, float minVal, float maxVal);
    void addConnectionBitAttribute(QString name, QColor offColor, QColor onColor);


private:
    Sidebar * m_neuronSidebar, * m_connectionSidebar;
    QWidget * m_widget;
    QVBoxLayout * m_layout;
};

#endif // ATTRIBUTEWIDGET_H
