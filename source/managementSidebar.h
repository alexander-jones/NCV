#ifndef MANAGEMENTSIDEBAR_H
#define MANAGEMENTSIDEBAR_H

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QDirModel>
#include <QCheckBox>
#include <QSignalMapper>
#include <QSpinBox>
#include <QToolBar>
#include <QComboBox>
#include <QGridLayout>
#include "sidebar.h"
#include "camerasidebar.h"
#include "qlabel.h"
#include "qgridlayout.h"
#include "lightingsidebar.h"
#include "collapsiblewidget.h"
#include "colorrangewidget.h"
#include "attributewidget.h"


class ManagementSidebar : public Sidebar
{
    Q_OBJECT
public:
    explicit ManagementSidebar(QWidget *tool = 0);
    ~ManagementSidebar();
    void addChildren(QTreeWidgetItem* item,QString filePath);
    CameraSidebar * cameraSidebar();
    LightingSidebar * lightingSidebar();
    AttributeWidget * attributeWidget();

signals:
    
public slots:
    void updateTimeScale(int multiplier);
    void updateFPS(float framesPerSecond);
    
private:
    void m_buildGroupingTab();
    void m_buildDisplayTab();
    CameraSidebar* m_cameraSubBar;
    LightingSidebar * m_lightingSubBar;
    QMap<QString,QWidget *> m_tabs;
    QMap<QString,QCheckBox *> m_checkBoxes;
    QMap<QWidget * ,LabeledWidget* > m_labeledWidgets;
    QLabel * m_titleLabel, * m_cameraLabel, *m_framesPerSecond,* m_timeScale, * m_groupingHeader, * m_projectionOptions;
    QMap<QString,QLayout*>  m_layouts;
    QSlider * m_timeScaleSlider;
    ColorRangeWidget * m_neuronRangeMap;
    CollapsibleWidget * m_cameraWidget, * m_lightingWidget ;
    AttributeWidget * m_attributeWidget;
};

#endif // GroupingSidebar_H
