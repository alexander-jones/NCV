#include "managementSidebar.h"

ManagementSidebar::ManagementSidebar(QWidget *parent) :
    QTabWidget(parent)
{
    m_buildPresentationTab();
    m_buildOrganizationTab();
    setFixedWidth(400);
}

ManagementSidebar::~ManagementSidebar()
{

}

void ManagementSidebar::m_buildOrganizationTab()
{
    m_organizationTaskPanel = new QwwTaskPanel();
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

    m_organizationTaskPanel->addTask(tree,QIcon(":/assets/groupIcon.png"),"Groups");
    this->addTab(m_organizationTaskPanel, QIcon(":/assets/organizationIcon.png"),"Organization");

}

void ManagementSidebar::m_buildPresentationTab()
{


    m_presentationTaskPanel = new QwwTaskPanel();

    m_groupDisplayWidget = new QWidget();
    m_presentationTaskPanel->addTask(m_groupDisplayWidget,QIcon(":/assets/layerIcon.png"),"Layers");
    m_presentationTaskPanel->addTask(m_groupDisplayWidget,QIcon(":/assets/columnIcon.png"),"Columns");
    m_groupDisplayWidget->show();

    m_attributeWidget = new AttributeWidget();
    m_presentationTaskPanel->addTask(m_attributeWidget->neuronWidget(), QIcon(":/assets/neuronIcon.png"),"Neurons");
    m_attributeWidget->neuronWidget()->show();
    m_presentationTaskPanel->addTask(m_attributeWidget->connectionWidget(), QIcon(":/assets/connectionIcon.png"),"Connections");
    m_attributeWidget->connectionWidget()->show();


    m_framesPerSecond = new QLabel();
    m_framesPerSecond->setAlignment(Qt::AlignCenter);
    m_presentationTaskPanel->addTask(m_framesPerSecond,QIcon(":/assets/groupIcon.png"),"FPS");
	    m_groupDisplayWidget->show();
    this->addTab(m_presentationTaskPanel, QIcon(":/assets/presentationIcon.png") ,"Presentation");

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


AttributeWidget * ManagementSidebar::attributeWidget()
{
    return m_attributeWidget;
}
