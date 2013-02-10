#ifndef NCVWIDGET_H
#define NCVWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QBoxLayout>
#include "orientationbutton.h"
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
    
private slots:
    void m_collapseButtonPressed();
    void m_reportFatalError();
    void m_newFrameReceived();

private:
    bool m_collapsed;
    QBoxLayout * m_layout;
    ManagementSidebar * m_managementSidebar;
    OrientationButton * m_collapseButton;
    QString m_expandText, m_collapseText;
    QTime m_timer;
    int m_frameCount;
    //RightToolBar *m_statusSidebar;
    NCV * m_visualization;
};

#endif // NCVWIDGET_H
