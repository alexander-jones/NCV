#ifndef LIGHTINGSIDEBAR_H
#define LIGHTINGSIDEBAR_H
#include "sidebar.h"
#include "spinbox3d.h"
#include "collapsiblewidget.h"
#include "qglxcore.h"
#include <QVector3D>
#include <QLineEdit>
#include <QPushButton>

class LightingSidebar : public CollapsibleWidget
{
    Q_OBJECT
public:
    LightingSidebar(QString text = "Render Settings",QWidget * parent = 0);
    ~LightingSidebar();
    void addLight(QGLXLight * light, QString  name);


signals:
    void lightCreated(QGLXLight * light, QString  name);
    void lightDeleted(QString  name);
    void lightSwitched( QString  name);
    void lightUpdated(QString  name);
    void newMaterial(QGLXMaterial *);

public slots:
    void updateLight(QString name);
    void setLightDefault(QGLXLight * light);

private slots:
    void m_addPressed();
    void m_deletePressed();
    void m_lightingAttributeChanged();
    void m_materialPropertyChanged();

private:
    QGLXMaterial * m_material;
    QMap<QString,QGLXLight *> m_lights;
    Sidebar * m_sidebar;
    QLabel * m_emptyWidget;
    QGLXLight * m_defaultLight;
    QCheckBox * m_enabled;
    QMap<QWidget * ,LabeledWidget* > m_labeledWidgets;
    SpinBox3D * m_position, * m_color;
    QGridLayout * m_intensityLayout, * m_attenuationLayout;
    QVBoxLayout *m_sidebarPanelLayout, * m_wholeLayout;
    CollapsibleWidget * m_intensitySection, * m_attenuationSection, * m_lightSection;
    QSpinBox * m_radius;
    QDoubleSpinBox * m_ambientIntensity, * m_diffuseIntensity, * m_specularIntensity, * m_constantAttenuation, * m_linearAttenuation, *m_quadraticAttenuation, * m_specularPower;
    QWidget * m_sidebarPanel, * m_intensityWidget, *m_attenuationWidget, * m_wholeWidget;
    QLineEdit * m_newLightName;
    QToolButton * m_addButton,* m_deleteButton;
};

#endif // LightingSidebar_H
