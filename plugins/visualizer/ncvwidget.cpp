#include "ncvwidget.h"


NCVWidget::NCVWidget(QWidget *parent) :
    NCSSubscriberWidgetPlugin(parent)
{
    m_valid = true;
    m_neurons = NULL;
    m_connections = NULL;
    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_layout->setSpacing(0);


    m_framesPerSecond = new QLabel();
    m_framesPerSecond->setAlignment(Qt::AlignCenter);

    m_renderTool = new NCVRenderTool();
    m_renderTool->setMaximumWidth(300);


    m_selectionLayout = new QVBoxLayout();

    m_renderDeselectedWidget = new QWidgetVector();
    m_renderDeselectedSwitch = new QSwitch();
    connect(m_renderDeselectedSwitch,SIGNAL(switched(bool)),this,SLOT(m_onRenderDeselectionSet(bool)));
    m_renderDeselectedWidget->addWidget(new QLabel("Render Deslected: "));
    m_renderDeselectedWidget->addWidget(m_renderDeselectedSwitch);
    m_selectionLayout->addWidget(m_renderDeselectedWidget);

    m_compoundSelectionWidget = new QWidgetVector();
    m_compoundSelectionSwitch = new QSwitch();
    connect(m_compoundSelectionSwitch,SIGNAL(switched(bool)),this,SLOT(m_onCompoundSelectionSet(bool)));
    m_compoundSelectionWidget->addWidget(new QLabel("Compound Selection: "));
    m_compoundSelectionWidget->addWidget(m_compoundSelectionSwitch);
    m_selectionLayout->addWidget(m_compoundSelectionWidget);

    connect(m_renderTool,SIGNAL(groupClicked(QLayout*,bool)),this,SLOT(m_selectionClicked(QLayout*,bool)));

    m_layout->addWidget(m_renderTool);

    m_selectionFlags = RenderDeselected;

    m_collapsed = false;

    m_expandText = ">";
    m_collapseText = "<";
    m_collapseButton = new QxtPushButton();
    QFont font = m_collapseButton->font();
    font.setPointSize(5);
    m_collapseButton->setFont(font);

    m_collapseButton->setToolTip("Click to collapse the management sidebar");
    m_collapseButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    m_collapseButton->setMaximumWidth(10);
    m_collapseButton->setText(m_collapseText);

    connect(m_collapseButton,SIGNAL(clicked()),this,SLOT(m_collapseButtonPressed()));
    m_layout->addWidget(m_collapseButton);

    QGLFormat glFormat;
    glFormat.setVersion(3,3 );
    glFormat.setDoubleBuffer(true);
    glFormat.setProfile( QGLFormat::CompatibilityProfile ); // Requires >=Qt-4.8.0
    glFormat.setSampleBuffers( true );
    m_canvas = new NCVCanvas( glFormat );
    m_layout->addWidget(m_canvas);

    setLayout(m_layout);
    setFocus();



    m_frameCount = 0;
    m_timer.start();
    connect(m_canvas,SIGNAL(frameRendered()),this,SLOT(m_newFrameReceived()));
    connect(m_canvas, SIGNAL(invalidGraphicsConfigurationDetected()),this,SLOT(m_reportFatalError()));

    connect(m_renderTool,SIGNAL(neuronRenderSet(bool)),m_canvas,SLOT(renderNeurons(bool)));
    connect(m_renderTool,SIGNAL(connectionRenderSet(bool)),m_canvas,SLOT(renderConections(bool)));

	
    connect(this,SIGNAL(selectionChanged(QVector<NCVElementRange>,NCVSelectionFlag)),m_canvas,SLOT(setSelection(QVector<NCVElementRange>,NCVSelectionFlag)));
    connect(m_canvas,SIGNAL(selectionChanged(QVector<NCVElementRange>,NCVSelectionFlag)),this,SLOT(setSelection(QVector<NCVElementRange>,NCVSelectionFlag)));


}

void NCVWidget::showEvent(QShowEvent *)
{
    if (!m_valid && m_layout->count() >0 )
    {
        m_layout->removeWidget(m_renderTool);
        m_renderTool->hide();

        m_layout->removeWidget(m_collapseButton);
        m_collapseButton->hide();

        m_layout->removeWidget(m_canvas);
        m_canvas->hide();

    }
}


void NCVWidget::openPortal(NCSProjectPortal project)
{
    m_projectDir = project.parentDirectory();
}


void NCVWidget::closePortal()
{
}

QString NCVWidget::name()
{
    return "visualizer";
}

float NCVWidget::version()
{
    return 1.0;
}

void  NCVWidget::setSelection(QVector<NCVElementRange> selection,NCVSelectionFlag flags)
{ 
	m_selectionFlags = flags;
	if ((int)m_selectionFlags & (int)RenderDeselected)
        m_renderDeselectedSwitch->setSwitched(true);
	else
        m_renderDeselectedSwitch->setSwitched(false);
	
	if ((int)m_selectionFlags & (int)CompoundSelection)
        m_compoundSelectionSwitch->setSwitched(true);
	else
        m_compoundSelectionSwitch->setSwitched(false);

	
	if (selection.count() == 0 )
	{
        if (m_currentSelection.count() != 0)
            m_renderTool->removeGroup(m_selectionLayout);
	}
	else if (m_currentSelection.count() == 0)
    {
        m_renderTool->addGroup(m_selectionLayout,"Selection");
    }

	
    m_currentSelection = selection;
    selectionChanged(m_currentSelection,m_selectionFlags);
}

void NCVWidget::m_onRenderDeselectionSet(bool on)
{
    if (on)
        m_selectionFlags = (NCVSelectionFlag)(m_selectionFlags | RenderDeselected);

    else if ((int)m_selectionFlags & (int)RenderDeselected )
        m_selectionFlags = (NCVSelectionFlag)((int)m_selectionFlags - (int)RenderDeselected);


	selectionChanged(m_currentSelection,m_selectionFlags);

}

void NCVWidget::m_onCompoundSelectionSet(bool on)
{ 
	if (on)
        m_selectionFlags = (NCVSelectionFlag)(m_selectionFlags | CompoundSelection);
	else if ((int)m_selectionFlags & (int)CompoundSelection )
        m_selectionFlags = (NCVSelectionFlag)((int)m_selectionFlags - (int)CompoundSelection);
	selectionChanged(m_currentSelection,m_selectionFlags);
}
void NCVWidget::setNeurons(NCSNeuronSet * neurons)
{
    if (neurons == NULL)
    {
        if (m_neurons != NULL)
        {
            m_neurons->destroy();
            //delete m_neurons;
        }
        m_neurons = NULL;
    }
    else
        m_neurons = new NCVNeuronSet(neurons);

    m_renderTool->setNeurons(m_neurons);
    m_canvas->setNeurons(m_neurons);

}

void  NCVWidget::setConnections(NCSConnectionSet * connections)
{

    if (connections == NULL)
    {
        if (m_connections != NULL)
        {
            m_connections->destroy();
            //delete m_connections;
        }
        m_connections = NULL;
    }
    else
        m_connections = new NCVConnectionSet(connections,m_neurons);
    m_renderTool->setConnections(m_connections);
    m_canvas->setConnections(m_connections);
}


void NCVWidget::m_collapseButtonPressed()
{
    if (!m_collapsed)
    {
        m_collapsed = true;
        m_layout->removeWidget(m_renderTool);
        m_renderTool->hide();
        m_collapseButton->setText(m_expandText);
        m_collapseButton->setToolTip("Click to expand the management sidebar");

    }
    else
    {
        m_collapsed = false;
        m_layout->insertWidget(0,m_renderTool);
        m_renderTool->show();
        m_collapseButton->setText(m_collapseText);
        m_collapseButton->setToolTip("Click to collapse the management sidebar");
    }

}

void NCVWidget::m_selectionClicked(QLayout * layout, bool checked)
{
    if (layout ==  m_selectionLayout && !checked)
    {
        m_currentSelection.clear();
        m_selectionFlags = RenderDeselected;
        selectionChanged(m_currentSelection,m_selectionFlags);
        m_renderTool->removeGroup(m_selectionLayout);
    }
}

void NCVWidget::m_newFrameReceived()
{
    // FPS counter
    m_frameCount ++;
    if (m_timer.elapsed() >= 1000)
    {
        //m_ncvSidebar->setFPS((float)m_frameCount/((float)m_timer.elapsed() /1000.0f));
        m_timer.start();
        m_frameCount = 0;
    }
}


NCVWidget::~NCVWidget()
{
   delete m_layout;
   delete m_renderTool;

   //delete m_statusSidebar;
   //delete m_canvas;


}

void NCVWidget::m_reportFatalError()
{
    m_valid = false;
}

void NCVWidget::initialize()
{
}

void NCVWidget::cleanup()
{
}

QIcon NCVWidget::icon()
{
    return QIcon(":/resources/images/visualizationIcon.png");
}

QString NCVWidget::title()
{
    return "Brain Visualizer";
}

/*void NCVSidebar::m_buildOrganizationTab()
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


}

void NCVSidebar::addChildren(QTreeWidgetItem* item,QString filePath)
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
*/
