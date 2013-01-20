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
#include "combowidget.h"
#include "camerasidebar.h"
#include "qlabel.h"
#include "qgridlayout.h"
#include "lightingsidebar.h"
#include "colorrangewidget.h"
#include "attributewidget.h"
#include <QwwTaskPanel>


class ManagementSidebar : public QWidget
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
    void updateFPS(float framesPerSecond);
    
private:
    void m_buildGroupingTab();
    void m_buildDisplayTab();

    QwwTaskPanel *m_taskPanel;
    CameraSidebar* m_cameraSubBar;
    LightingSidebar * m_lightingSubBar;
    QWidget * m_widget;
    QVBoxLayout * m_layout;
    QMap<QString,QCheckBox *> m_checkBoxes;
    QMap<QWidget * ,LabeledWidget* > m_labeledWidgets;
    QLabel * m_titleLabel, * m_cameraLabel, *m_framesPerSecond, * m_groupingHeader, * m_projectionOptions;
    ColorRangeWidget * m_neuronRangeMap;
    AttributeWidget * m_attributeWidget;
};

#endif // GroupingSidebar_H
