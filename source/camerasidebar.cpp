#include "camerasidebar.h"
#include <QDebug>
CameraSidebar::CameraSidebar(QString text, QWidget * parent)
    :CollapsibleWidget(NULL,text ,parent)
{
    m_sidebar = new Sidebar();
    connect(m_sidebar,SIGNAL(panelChanged(QString)),this,SIGNAL(cameraSwitched(QString)));
    this->setWidget(m_sidebar);

    m_newCameraName = new QLineEdit();
    m_sidebar->addTool(m_newCameraName);
    m_addButton = new QToolButton();
    m_addButton->setText("+");
    m_sidebar->addTool(m_addButton);
    connect(m_addButton,SIGNAL(pressed()),this,SLOT(m_addPressed()));

    m_deleteButton = new QToolButton();
    m_deleteButton->setText("-");
    m_sidebar->addTool(m_deleteButton);
    connect(m_deleteButton,SIGNAL(pressed()),this,SLOT(m_deletePressed()));

    m_widget = new QWidget();
    m_widget->setFixedWidth(325);
    m_widgetLayout = new QVBoxLayout();
    m_widgetLayout->setAlignment(Qt::AlignCenter);



    m_projectionWidget = new QWidget();
    m_projectionLayout = new QGridLayout();

    QFont font = m_projectionWidget->font();
    font.setPointSize(8);

    m_fov = new QDoubleSpinBox();
    m_fov->setMinimum(0);
    m_fov->setMaximum(180);
    m_fov->setFont(font);
    connect(m_fov,SIGNAL(editingFinished()),this,SLOT(m_attribChanged()));
    m_labeledWidgets[m_fov] = new LabeledWidget("FOV:",m_fov);
    m_projectionLayout->addWidget(m_labeledWidgets[m_fov],0,0);


    m_aspectRatio = new QDoubleSpinBox();
    m_aspectRatio->setEnabled(false);
    m_aspectRatio->setFont(font);
    m_labeledWidgets[m_aspectRatio] = new LabeledWidget("Aspect:",m_aspectRatio);
    m_projectionLayout->addWidget( m_labeledWidgets[m_aspectRatio],0,1 );


    m_nearPlane = new QDoubleSpinBox();
    m_nearPlane->setMinimum(0.1);
    m_nearPlane->setFont(font);
    connect(m_nearPlane,SIGNAL(editingFinished()),this,SLOT(m_attribChanged()));
    m_labeledWidgets[m_nearPlane] = new LabeledWidget("Near:",m_nearPlane);
    m_projectionLayout->addWidget(m_labeledWidgets[m_nearPlane],1,0);


    m_farPlane = new QDoubleSpinBox();
    m_farPlane->setMinimum(1.0);
    m_farPlane->setMaximum(1000000000.0);
    m_farPlane->setFont(font);
    connect(m_farPlane,SIGNAL(editingFinished()),this,SLOT(m_attribChanged()));
    m_labeledWidgets[m_farPlane] = new LabeledWidget("Far:",m_farPlane);
    m_projectionLayout->addWidget(m_labeledWidgets[m_farPlane],1,1);


    m_projectionWidget->setLayout(m_projectionLayout);

    m_projectionSection = new CollapsibleWidget(m_projectionWidget,"Projection");
    m_projectionSection->setFixedWidth(300);
    m_projectionSection->underlineHeader(true);
    m_projectionSection->expand();
    m_widgetLayout->addWidget(m_projectionSection);







    m_orientationWidget = new QWidget();
    m_orientationLayout = new QVBoxLayout();

    m_position = new SpinBox3D();
    m_position->setMinimum(-1000000,-1000000,-1000000);
    m_position->setMaximum(1000000,1000000,1000000);
    m_position->setFont(font);
    m_labeledWidgets[m_position] = new LabeledWidget("Position:",m_position);
    m_orientationLayout->addWidget(m_labeledWidgets[m_position]);
    connect(m_position,SIGNAL(valueEdited()),this,SLOT(m_attribChanged()));

    m_up = new SpinBox3D();
    m_up->setAutoNormalize();
    m_up->setMinimum(0,0,0);
    m_up->setMaximum(1,1,1);
    m_up->setFont(font);
    m_labeledWidgets[m_up] = new LabeledWidget("Up:",m_up);
    m_orientationLayout->addWidget(m_labeledWidgets[m_up]);
    connect(m_up,SIGNAL(valueEdited()),this,SLOT(m_attribChanged()));

    m_forward = new SpinBox3D();
    m_up->setAutoNormalize();
    m_forward->setMinimum(0,0,0);
    m_forward->setMaximum(1,1,1);
    m_forward->setFont(font);
    m_labeledWidgets[m_forward] = new LabeledWidget("Forward:",m_forward);
    m_orientationLayout->addWidget(m_labeledWidgets[m_forward]);
    connect(m_forward,SIGNAL(valueEdited()),this,SLOT(m_attribChanged()));

    m_orientationWidget->setLayout(m_orientationLayout);
    m_orientationSection = new CollapsibleWidget(m_orientationWidget,"Orientation");
    m_orientationSection->setFixedWidth(300);
    m_orientationSection->underlineHeader(true);
    m_orientationSection->expand();
    m_widgetLayout->addWidget(m_orientationSection);

    m_widget->setLayout(m_widgetLayout);

}


 CameraSidebar::~CameraSidebar()
{
    delete m_widget;
    delete m_widgetLayout;
    delete m_sidebar;

    delete m_projectionSection;
    delete m_orientationSection;

    delete m_titleLabel;
    delete m_framesPerSecond;
    delete m_timeScale;
    delete m_groupingHeader;
    delete m_projectionOptions;

    delete m_cameraLayout;
    delete m_projectionLayout;
    delete m_addButton;
    delete m_deleteButton;
    delete m_newCameraName;
    delete m_widgetLayout;
    delete m_orientationLayout;
    delete m_signalMapper;
    delete m_fov;
    delete m_nearPlane;
    delete m_farPlane;

    delete m_aspectRatio;
    delete m_timeScaleSlider;
    delete m_position;
    delete m_forward;
    delete m_up;
    delete m_projectionWidget;
    delete m_orientationWidget;

    for (QMap<QWidget * ,LabeledWidget*>::iterator it = m_labeledWidgets.begin(); it != m_labeledWidgets.end();it++)
        delete it.value();

}

void CameraSidebar::m_addPressed()
{
    QString cameraToAdd = m_newCameraName->text();
    bool cameraNameGiven = cameraToAdd.length() > 0;
    bool alreadyCreated = m_sidebar->containsPanel(cameraToAdd);
    if (cameraNameGiven && !alreadyCreated)
    {
        QGLXCamera * newCam = new QGLXCamera();
        newCam->setProjectionMatrix((GLfloat)m_fov->value(),(GLfloat)m_aspectRatio->value(),(GLfloat)m_nearPlane->value(),(GLfloat)m_farPlane->value());
        newCam->setViewMatrix(m_position->value(),m_position->value() + m_forward->value(), m_up->value());
        addCamera(newCam,cameraToAdd);
        cameraCreated(newCam,cameraToAdd);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Camera Could Not Be Added.");
        if (!cameraNameGiven)
            msgBox.setInformativeText("Name not specified");
        else
            msgBox.setInformativeText("Camera already created under that name");

        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }

    m_newCameraName->clear();
}

void CameraSidebar::switchCamera(QString name)
{
    if (m_sidebar->currentPanel() != name)
    {
        if (!m_sidebar->containsPanel(name))
            m_sidebar->addPanel(m_widget,name);
        if (m_sidebar->currentPanel() != name)
            m_sidebar->setPanel(name);
    }
}

void CameraSidebar::addCamera(QGLXCamera * camera, QString name)
{
    if (!m_sidebar->containsPanel(name))
    {
        m_cameras[name] = camera;
        m_sidebar->addPanel(m_widget,name);
        updateCamera(name);
    }
}

void CameraSidebar::m_deletePressed()
{
    QString cameraToDelete = m_newCameraName->text();

    bool cameraNameGiven = cameraToDelete.length() > 0;
    bool alreadyCreated = m_sidebar->containsPanel(cameraToDelete);

    if (m_sidebar->numPanels() > 1 && cameraNameGiven && alreadyCreated)
    {
        m_sidebar->removePanel(cameraToDelete);
        cameraDeleted(cameraToDelete);
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Camera Could Not Be Deleted.");
        if (!cameraNameGiven)
            msgBox.setInformativeText("Name not specified.");
        else if(!alreadyCreated)
            msgBox.setInformativeText("Camera hasn't been created under that name.");
        else
            msgBox.setInformativeText("At least one camera must remain in the set.");

        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }

    m_newCameraName->clear();
}


void CameraSidebar::updateCamera(QString name)
{
    if (name == m_sidebar->currentPanel())
    {
        m_fov->setValue(m_cameras[name]->getFieldOfView());
        m_aspectRatio->setValue(m_cameras[name]->getAspectRatio());
        m_nearPlane->setValue(m_cameras[name]->getNearPlane());
        m_farPlane->setValue(m_cameras[name]->getFarPlane());
        m_position->setValue(m_cameras[name]->getPosition());
        m_forward->setValue(m_cameras[name]->getForward());
        m_up->setValue(m_cameras[name]->getUp());
    }
}

void CameraSidebar::m_attribChanged()
{
    QString name = m_sidebar->currentPanel();
    m_cameras[name]->setViewMatrix(m_position->value(),m_position->value() + m_forward->value(), m_up->value());
    m_cameras[name]->setProjectionMatrix(m_fov->value(),m_aspectRatio->value(),m_nearPlane->value(),m_farPlane->value());
    //lightUpdated(name);

}

