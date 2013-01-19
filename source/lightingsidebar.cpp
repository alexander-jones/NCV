#include "lightingsidebar.h"
#include <QMessageBox>
LightingSidebar::LightingSidebar( QWidget * parent)
    :QWidget(parent)
{
    m_material = new QGLXMaterial();

    m_wholeLayout = new QVBoxLayout();

    QFont font = this->font();
    font.setPointSize(8);

    m_sidebar = new ComboWidget();
    m_wholeLayout->addWidget(m_sidebar);
    connect(m_sidebar,SIGNAL(widgetChanged(QString)),this,SLOT(updateLight(QString)));

    m_defaultLight  = new QGLXLight();
    m_defaultLight->position = QVector3D(0,0,0);
    m_defaultLight->color = QVector3D(0.7,0.7,0.7);
    m_defaultLight->constantAttenuation = 0.1f;
    m_defaultLight->linearAttenuation = 0.2f;
    m_defaultLight->quadtraticAttenuation = 0.045f;
    m_defaultLight->specularPower = 100.0f;
    m_defaultLight->radius = 10000.0f;
    m_defaultLight->enabled = false;


    m_emptyWidget = new QLabel("No Lights to Manage");
    m_emptyWidget->setAlignment(Qt::AlignCenter);
    m_sidebar->setVoidWidget(m_emptyWidget);
    m_enabled = new QCheckBox();
    m_enabled->setChecked(false);
    m_sidebar->addTool(m_enabled);
    connect(m_enabled,SIGNAL(stateChanged(int)),this,SLOT(m_lightingAttributeChanged()));

    m_newLightName = new QLineEdit();
    m_sidebar->addTool(m_newLightName);
    m_addButton = new QToolButton();
    m_addButton->setText("+");
    m_sidebar->addTool(m_addButton);
    connect(m_addButton,SIGNAL(pressed()),this,SLOT(m_addPressed()));

    m_deleteButton = new QToolButton();
    m_deleteButton->setText("-");
    m_sidebar->addTool(m_deleteButton);
    connect(m_deleteButton,SIGNAL(pressed()),this,SLOT(m_deletePressed()));

    m_sidebarPanel = new QWidget();
    m_sidebarPanel->setFixedWidth(325);
    m_sidebarPanelLayout = new QVBoxLayout();

    m_sidebarPanelLayout->setAlignment(Qt::AlignCenter);

    m_position = new SpinBox3D();
    m_position->setMinimum(-1000000,-1000000,-1000000);
    m_position->setMaximum(1000000,1000000,1000000);
    m_position->setFont(font);
    m_labeledWidgets[m_position] = new LabeledWidget("Position:",m_position);
    m_sidebarPanelLayout->addWidget(m_labeledWidgets[m_position]);
    connect(m_position,SIGNAL(valueEdited()),this,SLOT(m_lightingAttributeChanged()));

    m_color = new SpinBox3D();
    m_color->setMinimum(0,0,0);
    m_color->setMaximum(1,1,1);
    m_color->setFont(font);
    m_labeledWidgets[m_color] = new LabeledWidget("Color:",m_color);
    m_sidebarPanelLayout->addWidget(m_labeledWidgets[m_color]);
    connect(m_color,SIGNAL(valueEdited()),this,SLOT(m_lightingAttributeChanged()));


    m_radius = new QSpinBox();
    m_radius->setMinimum(0);
    m_radius->setMaximum(100000000.0f);
    m_radius->setFont(font);
    connect(m_radius,SIGNAL(editingFinished()),this,SLOT(m_lightingAttributeChanged()));
    m_labeledWidgets[m_radius] = new LabeledWidget("Radius:",m_radius);
    m_sidebarPanelLayout->addWidget(m_labeledWidgets[m_radius],0,0);


    m_specularPower = new QDoubleSpinBox();
    m_specularPower->setMinimum(0);
    m_specularPower->setMaximum(1000.0f);
    m_specularPower->setFont(font);
    connect(m_radius,SIGNAL(editingFinished()),this,SLOT(m_lightingAttributeChanged()));
    m_labeledWidgets[m_specularPower] = new LabeledWidget("Specular Power:",m_specularPower);
    m_sidebarPanelLayout->addWidget(m_labeledWidgets[m_specularPower],0,0);

    m_attenuationWidget = new QWidget();
    m_attenuationLayout = new QGridLayout();

    m_constantAttenuation = new QDoubleSpinBox();
    m_constantAttenuation->setMinimum(0);
    m_constantAttenuation->setMaximum(1);
    m_constantAttenuation->setFont(font);
    connect(m_constantAttenuation,SIGNAL(editingFinished()),this,SLOT(m_lightingAttributeChanged()));
    m_labeledWidgets[m_constantAttenuation] = new LabeledWidget("Constant:",m_constantAttenuation);
    m_attenuationLayout->addWidget(m_labeledWidgets[m_constantAttenuation],0,0);

    m_linearAttenuation = new QDoubleSpinBox();
    m_linearAttenuation->setMinimum(0);
    m_linearAttenuation->setMaximum(1);
    m_linearAttenuation->setFont(font);
    connect(m_linearAttenuation,SIGNAL(editingFinished()),this,SLOT(m_lightingAttributeChanged()));
    m_labeledWidgets[m_linearAttenuation] = new LabeledWidget("Linear:",m_linearAttenuation);
    m_attenuationLayout->addWidget(m_labeledWidgets[m_linearAttenuation],1,0);


    m_quadraticAttenuation = new QDoubleSpinBox();
    m_quadraticAttenuation->setMinimum(0);
    m_quadraticAttenuation->setMaximum(1);
    m_quadraticAttenuation->setFont(font);
    connect(m_quadraticAttenuation,SIGNAL(editingFinished()),this,SLOT(m_lightingAttributeChanged()));
    m_labeledWidgets[m_quadraticAttenuation] = new LabeledWidget("Quadratic:",m_quadraticAttenuation);
    m_attenuationLayout->addWidget(m_labeledWidgets[m_quadraticAttenuation],0,1);

    m_attenuationWidget->setLayout(m_attenuationLayout);
    m_sidebarPanelLayout->addWidget(m_attenuationWidget);

    m_sidebarPanel->setLayout(m_sidebarPanelLayout);
    m_wholeLayout->addWidget(m_sidebar);


    m_intensityWidget = new QWidget();
    m_intensityLayout = new QGridLayout();

    m_ambientIntensity = new QDoubleSpinBox();
    m_ambientIntensity->setMinimum(0);
    m_ambientIntensity->setValue(0.5f);
    m_ambientIntensity->setMaximum(1);
    m_ambientIntensity->setFont(font);
    connect(m_ambientIntensity,SIGNAL(editingFinished()),this,SLOT(m_materialPropertyChanged()));
    m_labeledWidgets[m_ambientIntensity] = new LabeledWidget("Ambient:",m_ambientIntensity);
    m_intensityLayout->addWidget(m_labeledWidgets[m_ambientIntensity],0,0);

    m_diffuseIntensity = new QDoubleSpinBox();
    m_diffuseIntensity->setMinimum(0);
    m_diffuseIntensity->setValue(0.2f);
    m_diffuseIntensity->setMaximum(1);
    m_diffuseIntensity->setFont(font);
    connect(m_diffuseIntensity,SIGNAL(editingFinished()),this,SLOT(m_materialPropertyChanged()));
    m_labeledWidgets[m_diffuseIntensity] = new LabeledWidget("Diffuse:",m_diffuseIntensity);
    m_intensityLayout->addWidget(m_labeledWidgets[m_diffuseIntensity],1,0);


    m_specularIntensity = new QDoubleSpinBox();
    m_specularIntensity->setMinimum(0);
    m_specularIntensity->setValue(0.05f);
    m_specularIntensity->setMaximum(1);
    m_specularIntensity->setFont(font);
    connect(m_specularIntensity,SIGNAL(editingFinished()),this,SLOT(m_materialPropertyChanged()));
    m_labeledWidgets[m_specularIntensity] = new LabeledWidget("Specular:",m_specularIntensity);
    m_intensityLayout->addWidget(m_labeledWidgets[m_specularIntensity],0,1);

    m_intensityWidget->setLayout(m_intensityLayout);
    m_wholeLayout->addWidget(m_intensityWidget);


    this->setLayout(m_wholeLayout);

}

LightingSidebar::~LightingSidebar()
{

}


void LightingSidebar::setLightDefault(QGLXLight * light)
{
    m_defaultLight = light;
}

void LightingSidebar::addLight(QGLXLight * light, QString  name)
{
    if (!m_sidebar->containsWidget(name))
    {
        m_lights[name] = light;
        m_sidebar->addWidget(m_sidebarPanel,name);
        updateLight(name);
    }
}


void LightingSidebar::updateLight(QString name)
{
    if (name == m_sidebar->currentWidgetName())
    {
        m_constantAttenuation->setValue(m_lights[name]->constantAttenuation);
        m_linearAttenuation->setValue(m_lights[name]->linearAttenuation);
        m_quadraticAttenuation->setValue(m_lights[name]->quadtraticAttenuation);
        m_radius->setValue(m_lights[name]->radius);
        m_specularPower->setValue(m_lights[name]->specularPower);
        m_color->setValue(m_lights[name]->color);
        m_position->setValue(m_lights[name]->position);
        m_enabled->setChecked(m_lights[name]->enabled);
    }
}


void LightingSidebar::m_lightingAttributeChanged()
{
    if (m_sidebar->count() == 0)
        return;
    QString name = m_sidebar->currentWidgetName();
    m_lights[name]->constantAttenuation = m_constantAttenuation->value();
    m_lights[name]->linearAttenuation = m_linearAttenuation->value();
    m_lights[name]->quadtraticAttenuation = m_quadraticAttenuation->value();
    m_lights[name]->color = m_color->value();
    m_lights[name]->position = m_position->value();
    m_lights[name]->specularPower = m_specularPower->value();
    m_lights[name]->radius = m_radius->value();
    if (m_sidebar->count() ==0 && m_enabled->isChecked())
        m_enabled->setChecked(false);
    else
        m_lights[name]->enabled = m_enabled->isChecked();
    lightUpdated(name);

}


void LightingSidebar::m_materialPropertyChanged()
{
    m_material->AmbientIntensity = m_ambientIntensity->value();
    m_material->SpecularIntensity = m_specularIntensity->value();
    m_material->DiffuseIntensity = m_diffuseIntensity->value();

    newMaterial(m_material);
}


void LightingSidebar::m_addPressed()
{

    QString lightToAdd = m_newLightName->text();
    bool cameraNameGiven = lightToAdd.length() > 0;
    bool alreadyCreated = m_sidebar->containsWidget(lightToAdd);

    if (cameraNameGiven && !alreadyCreated)
    {
        QGLXLight * light;
        if (m_sidebar->count() == 0)
        {
            light = m_defaultLight;
        }
        else
        {
            light  = new QGLXLight();
            light->position = m_position->value();
            light->color = m_color->value();
            light->constantAttenuation = m_constantAttenuation->value();
            light->linearAttenuation = m_linearAttenuation->value();
            light->quadtraticAttenuation = m_quadraticAttenuation->value();
            light->specularPower = m_specularPower->value();
            light->radius = m_radius->value();
            light->enabled = false;
        }
        addLight(light,lightToAdd);
        lightCreated(light,lightToAdd);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Light Could Not Be Added.");
        if (!cameraNameGiven)
            msgBox.setInformativeText("Name not specified");
        else
            msgBox.setInformativeText("Light already created under that name");

        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }


    m_newLightName->clear();
}

void LightingSidebar::m_deletePressed()
{
    QString lightToDelete = m_newLightName->text();
    bool lightNameGiven = lightToDelete.length() > 0;
    bool alreadyCreated = m_sidebar->containsWidget(lightToDelete);
    if (m_sidebar->count() == 0 || !lightNameGiven || !alreadyCreated)
    {
        QMessageBox msgBox;
        msgBox.setText("Light Could Not Be Deleted.");

        if (m_sidebar->count() == 0)
            msgBox.setInformativeText("No lights exist.");
        else if (!alreadyCreated)
            msgBox.setInformativeText("No light with that name exists.");
        else if (!lightNameGiven)
            msgBox.setInformativeText("No light name specified.");

        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
    else
    {
        m_sidebar->removeWidget(lightToDelete);
        m_lights.remove(lightToDelete);
        lightDeleted(lightToDelete);
    }
    m_newLightName->clear();

}

