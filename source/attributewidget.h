#ifndef ATTRIBUTEWIDGET_H
#define ATTRIBUTEWIDGET_H

#include <QVBoxLayout>
#include <QMessageBox>
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
    void m_neuronBitColorationChanged(QColor offColor,QColor onColor);
    void m_neuronRangeColorationChanged(QList<Cursor>  markers);
    void m_connectionBitColorationChanged(QColor offColor,QColor onColor);
    void m_connectionRangeColorationChanged(QList<Cursor> markers);
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

    Sidebar * m_neuronSidebar, * m_connectionSidebar;
    QWidget * m_widget;
    QVBoxLayout * m_layout;
    QPushButton * m_neuronLinkButton, *m_connectionLinkButton;
    QIcon  m_linkedIcon,m_unlinkedIcon;
    QMap<QString,bool > m_attributeLinked;
    CollapsibleWidget* m_neuronSection, * m_connectionSection;
    QMap<QString,ColorBitWidget* > m_neuronFlagWidgets;
    QMap<QString,ColorRangeWidget* > m_neuronRangeWidgets;
    QMap<QString,ColorBitWidget* > m_connectionFlagWidgets;
    QMap<QString,ColorRangeWidget* > m_connectionRangeWidgets;
};

#endif // ATTRIBUTEWIDGET_H
