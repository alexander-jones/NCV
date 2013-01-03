#include "managementSidebar.h"

ManagementSidebar::ManagementSidebar(QWidget *parent) :
    Sidebar(parent)
{

    m_buildGroupingTab();
    m_buildDisplayTab();
    //setMinimumWidth(400);
    //setMaximumWidth(400);
    setFixedWidth(400);
    setScrollBarPolicy(Qt::ScrollBarAlwaysOff,Qt::ScrollBarAsNeeded);

}

ManagementSidebar::~ManagementSidebar()
{

   delete m_cameraSubBar;
   delete m_lightingSubBar;

   delete m_titleLabel;
   delete m_cameraLabel;
   delete m_framesPerSecond;
   delete m_timeScale;
   delete m_groupingHeader;
   delete m_projectionOptions;

   delete m_timeScaleSlider;
   delete m_cameraWidget;
   delete m_lightingWidget;

   for (QMap<QWidget * ,LabeledWidget*>::iterator it = m_labeledWidgets.begin(); it != m_labeledWidgets.end();it++)
       delete it.value();

   for (QMap<QString ,QWidget*>::iterator it = m_tabs.begin(); it != m_tabs.end();it++)
       delete it.value();

   for (QMap<QString ,QCheckBox*>::iterator it = m_checkBoxes.begin(); it != m_checkBoxes.end();it++)
       delete it.value();
}



void ManagementSidebar::addChildren(QTreeWidgetItem* item,QString filePath)
{
    QDir* rootDir = new QDir(filePath);
    QFileInfoList filesList = rootDir->entryInfoList();

    foreach(QFileInfo fileInfo, filesList)
    {
        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setText(0,fileInfo.fileName());
        child->setText(1,QString::number(50000));

          //child->setIcon(0,*(new QIcon("folder.jpg")));
        child->setText(2,QString::number(500000));

        item->addChild(child);
    }
}

void ManagementSidebar::m_buildGroupingTab()
{
    m_tabs["simulation"] = new QWidget();
    m_layouts["simulation"] = new QVBoxLayout();
    QTreeWidget  *tree = new QTreeWidget ();

    m_groupingHeader = new QLabel("Groups to Render:");
    m_groupingHeader->setAlignment(Qt::AlignCenter);
    m_layouts["simulation"]->addWidget(m_groupingHeader);

    QFont font = tree->font();
    tree->setSelectionMode( QAbstractItemView::MultiSelection);
    tree->setFixedSize(325,400);
    font.setPointSize(8);
    tree->setFont(font);
    //Set QTreeWidget Column Header
    QTreeWidgetItem* headerItem = new QTreeWidgetItem();
    headerItem->setText(0,QString("Group"));
    headerItem->setText(1,QString("Neruons"));
    headerItem->setText(2,QString("Connections"));
    headerItem->setText(3,QString("Subgroups"));
    tree->setHeaderItem(headerItem);
    tree->setColumnWidth(0,this->width() / 4);
    tree->setColumnWidth(1,this->width() / 4);
    tree->setColumnWidth(2,this->width() / 4);
    tree->setColumnWidth(3,this->width() / 4);


    QDir* rootDir = new QDir("/");
    QFileInfoList filesList = rootDir->entryInfoList();


    foreach(QFileInfo fileInfo, filesList)
    {
      QTreeWidgetItem* item = new QTreeWidgetItem();
      item->setText(0,fileInfo.fileName());

      item->setText(0,fileInfo.fileName());
      item->setText(1,QString::number(50000));

        //child->setIcon(0,*(new QIcon("folder.jpg")));
      item->setText(2,QString::number(500000));


      if(fileInfo.isDir())
      {
        //item->setIcon(0,*(new QIcon("folder.jpg")));
        addChildren(item,fileInfo.filePath());
      }
      tree->addTopLevelItem(item);
    }


    m_layouts["simulation"]->addWidget(tree);

    m_tabs["simulation"]->setLayout(m_layouts["simulation"]);

    m_timeScale = new QLabel("Time Scale: 1.0 X");

    m_timeScale->setAlignment(Qt::AlignCenter);
    m_layouts["simulation"]->addWidget(m_timeScale);

    m_timeScaleSlider = new QSlider(Qt::Horizontal);
    m_layouts["simulation"]->addWidget(m_timeScaleSlider);
    m_timeScaleSlider->setMinimum(0);
    m_timeScaleSlider->setMaximum(100);
    m_timeScaleSlider->setValue(100);

    connect(m_timeScaleSlider,SIGNAL(valueChanged(int)),this,SLOT(updateTimeScale(int)));

    m_attributeWidget = new AttributeWidget("Attribute Configuration");
    m_attributeWidget->underlineHeader(true);
    m_attributeWidget->setFrameShape(QFrame::Box);
    m_attributeWidget->setFrameShadow(QFrame::Sunken);
    m_layouts["simulation"]->addWidget(m_attributeWidget);

    this->addPanel(m_tabs["simulation"],"Simulation Manager");
}

void ManagementSidebar::updateTimeScale(int value)
{
    float multiplier = (float)value / (float)m_timeScaleSlider->maximum();
    QString str = "Time Scale:";
    str.append(QString(" %1 X").arg(multiplier));
    m_timeScale->setText(str);

}

void ManagementSidebar::updateFPS(float fps)
{
    QString str = "FPS:";
    str.append(QString(" %1").arg(fps));
    m_framesPerSecond->setText(str);

}

void ManagementSidebar::m_buildDisplayTab()
{
    m_tabs["display"] = new QWidget();
    this->addPanel(m_tabs["display"],"Display Manager");

    m_layouts["display"] = new QVBoxLayout();

    m_layouts["display"]->setAlignment(Qt::AlignCenter);

    m_framesPerSecond = new QLabel();
    m_framesPerSecond->setAlignment(Qt::AlignCenter);
    m_layouts["display"]->addWidget(m_framesPerSecond);

    m_cameraSubBar = new CameraSidebar();
    m_cameraSubBar->setFixedWidth(350);
    m_cameraSubBar->underlineHeader(true);
    m_cameraSubBar->setFrameShape(QFrame::Box);
    m_cameraSubBar->setFrameShadow(QFrame::Sunken);
    m_layouts["display"]->addWidget(m_cameraSubBar);

    m_lightingSubBar = new LightingSidebar();
    m_lightingSubBar->setFixedWidth(350);
    m_lightingSubBar->underlineHeader(true);
    m_lightingSubBar->setFrameShape(QFrame::Box);
    m_lightingSubBar->setFrameShadow(QFrame::Sunken);
    m_layouts["display"]->addWidget(m_lightingSubBar);



    //m_checkBoxes["performLighting"] = new QCheckBox("Perform Lighting");
    //m_layouts["display"]->addWidget(m_checkBoxes["performLighting"]);

    m_tabs["display"]->setLayout(m_layouts["display"]);


}

CameraSidebar * ManagementSidebar::cameraSidebar()
{
    return m_cameraSubBar;
}

LightingSidebar * ManagementSidebar::lightingSidebar()
{
    return m_lightingSubBar;
}

AttributeWidget * ManagementSidebar::attributeWidget()
{
    return m_attributeWidget;
}
