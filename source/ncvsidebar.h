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
#include <QwwTaskPanel>
#include "ncvcore.h"


class NCVSidebar : public QTabWidget
{
    Q_OBJECT
public:
    explicit NCVSidebar(QWidget *tool = 0);
    ~NCVSidebar();

    void setTabWidget(const QString& string, NCVElementType element, QWidget * widget,bool show = false);
    void addTab(const QIcon & icon, const QString & string);
    void addTab(const QString & string);


signals:

    
private:
    void m_addChildren(QTreeWidgetItem* item,QString filePath);
    void m_buildPresentationTab();
    void m_buildOrganizationTab();

    QLabel *m_framesPerSecond;
    QWidget * m_emptyWidget;
    QMap<QString, QwwTaskPanel *>m_taskPanels;
};

#endif // GroupingSidebar_H
