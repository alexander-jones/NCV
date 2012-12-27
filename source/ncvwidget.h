#ifndef NCVWIDGET_H
#define NCVWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QBoxLayout>
#include "righttoolbar.h"
#include "managementSidebar.h"
#include "ncv.h"

#ifdef WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
#endif

class NCVWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NCVWidget(QWidget *parent = 0);
    ~NCVWidget();
    
signals:
    
public slots:
    void m_reportFatalError();
    
private:
    QBoxLayout * m_layout;
    ManagementSidebar * m_managementSidebar;
    RightToolBar *m_statusSidebar;
    NCV * m_visualization;
};

#endif // NCVWIDGET_H
