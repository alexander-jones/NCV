#ifndef CAMERASIDEBAR_H
#define CAMERASIDEBAR_H

#include "sidebar.h"
#include "qglxcamera.h"
#include "collapsiblewidget.h"
#include "spinbox3d.h"
#include <QVector3D>
#include <QLineEdit>
#include <QMessageBox>
#include <QToolButton>

class CameraSidebar : public CollapsibleWidget
{
    Q_OBJECT
public:
    CameraSidebar(QString text = "Camera Settings",QWidget * parent = 0);
    ~CameraSidebar();

signals:
    void cameraCreated(QGLXCamera * cam, QString name);
    void cameraSwitched(QString name);
    void cameraDeleted(QString name);

public slots:
    void addCamera(QGLXCamera * camera, QString name);
    void switchCamera(QString name);
    void updateCamera(QString name);

private slots:
    void m_addPressed();
    void m_deletePressed();
    void m_attribChanged();

private:
    Sidebar * m_sidebar;
    CollapsibleWidget * m_projectionSection, * m_orientationSection;
    QMap<QString, QGLXCamera * >m_cameras;
    QMap<QWidget * ,LabeledWidget* > m_labeledWidgets;
    QLabel * m_titleLabel, * m_framesPerSecond,* m_timeScale, * m_groupingHeader, * m_projectionOptions;
    QMap<QString,QLayout*>  m_layouts;
    QGridLayout *m_cameraLayout,* m_projectionLayout;
    QToolButton * m_addButton, * m_deleteButton;
    QLineEdit * m_newCameraName;
    QVBoxLayout *m_widgetLayout, * m_orientationLayout;
    QSignalMapper * m_signalMapper;
    QDoubleSpinBox * m_fov,* m_nearPlane,* m_farPlane;
    QDoubleSpinBox * m_aspectRatio;
    QSlider * m_timeScaleSlider;
    SpinBox3D * m_position, * m_forward, * m_up;
    QWidget * m_widget, * m_projectionWidget, *m_orientationWidget;

};

#endif // CAMERASIDEBAR_H
