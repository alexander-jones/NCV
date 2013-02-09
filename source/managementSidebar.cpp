#include "managementSidebar.h"

ManagementSidebar::ManagementSidebar(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QVBoxLayout();
    m_taskPanel = new QwwTaskPanel();


    QTreeWidget  *tree = new QTreeWidget ();
    tree->setSelectionMode( QAbstractItemView::MultiSelection);
    tree->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    QFont font = tree->font();
    font.setPointSize(8);
    tree->setFont(font);
    //Set QTreeWidget Column Header
    QTreeWidgetItem* headerItem = new QTreeWidgetItem();
    headerItem->setText(0,QString("Group"));
    headerItem->setText(1,QString("Neruons"));
    headerItem->setText(2,QString("Connections"));
    headerItem->setText(3,QString("Subgroups"));
    tree->setHeaderItem(headerItem);


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

    m_taskPanel->addTask(tree,QIcon(":/assets/groupIcon.png"),"Groups");

    m_attributeWidget = new AttributeWidget();
    m_taskPanel->addTask(m_attributeWidget->neuronWidget(), QIcon(":/assets/neuronIcon.png"),"Neurons");
    m_taskPanel->addTask(m_attributeWidget->connectionWidget(), QIcon(":/assets/connectionIcon.png"),"Connections");

    /*m_cameraSubBar = new CameraSidebar();
    m_cameraSubBar->setFixedWidth(300);
    m_taskPanel->addTask(m_cameraSubBar,QIcon(":/assets/cameraIcon.png"),"Camera Settings");

    m_lightingSubBar = new LightingSidebar();
    m_taskPanel->addTask(m_lightingSubBar, QIcon(":/assets/neuronIcon.png"),"Lighting Settings");*/
    m_layout->addWidget(m_taskPanel);

    m_framesPerSecond = new QLabel();
    m_framesPerSecond->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_framesPerSecond);


    this->setLayout(m_layout);
    setFixedWidth(400);

}

ManagementSidebar::~ManagementSidebar()
{

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



void ManagementSidebar::setFPS(float fps)
{
    QString str = "FPS:";
    str.append(QString(" %1").arg(fps));
    m_framesPerSecond->setText(str);

}


CameraSidebar * ManagementSidebar::cameraSidebar()
{
    return m_cameraSubBar;
}

AttributeWidget * ManagementSidebar::attributeWidget()
{
    return m_attributeWidget;
}
