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
#include "qlabel.h"
#include "qgridlayout.h"
#include "attributewidget.h"
#include <QwwTaskPanel>


class ManagementSidebar : public QTabWidget
{
    Q_OBJECT
public:
    explicit ManagementSidebar(QWidget *tool = 0);
    ~ManagementSidebar();
    void addChildren(QTreeWidgetItem* item,QString filePath);
    AttributeWidget * attributeWidget();

signals:
    
public slots:
    void setFPS(float framesPerSecond);
    
private:
    void m_buildPresentationTab();
    void m_buildOrganizationTab();

    QLabel *m_framesPerSecond;
    AttributeWidget * m_attributeWidget;
    QWidget * m_groupDisplayWidget;
    QwwTaskPanel * m_presentationTaskPanel, * m_organizationTaskPanel;
};

#endif // GroupingSidebar_H
