#include "ncvsidebar.h"

NCVSidebar::NCVSidebar(QWidget *parent) :
    QTabWidget(parent)
{

    m_emptyWidget = new QWidget();

    setFixedWidth(400);
}

NCVSidebar::~NCVSidebar()
{

}

void NCVSidebar::setTabWidget(const QString& string, NCVElementType element, QWidget * widget, bool show )
{
    if (m_taskPanels.contains(string))
    {
        if (element == Grouping)
        {
            m_taskPanels[string]->removeTask(0);
            m_taskPanels[string]->insertTask(0,widget,QIcon(":/assets/groupIcon.png"),"Groupings");
        }
        else if (element == Neuron)
        {
            m_taskPanels[string]->removeTask(1);
            m_taskPanels[string]->insertTask(1,widget,QIcon(":/assets/neuronIcon.png"),"Neurons");
        }
        else if (element == Connection)
        {
            m_taskPanels[string]->removeTask(2);
            m_taskPanels[string]->insertTask(2,widget,QIcon(":/assets/connectionIcon.png"),"Connections");
        }
        if (show)
            widget->show();
    }

}
void NCVSidebar::addTab(const QIcon & icon, const QString & string)
{

    m_taskPanels[string]  = new QwwTaskPanel();
    m_taskPanels[string]->addTask(m_emptyWidget,QIcon(":/assets/groupIcon.png"),"Groupings");
    m_taskPanels[string]->addTask(m_emptyWidget,QIcon(":/assets/neuronIcon.png"),"Neurons");
    m_taskPanels[string]->addTask(m_emptyWidget,QIcon(":/assets/connectionIcon.png"),"Connections");
    QTabWidget::addTab(m_taskPanels[string], icon,string);
}
void NCVSidebar::addTab(const QString & string)
{

    m_taskPanels[string]  = new QwwTaskPanel();
    m_taskPanels[string]->addTask(m_emptyWidget,QIcon(":/assets/layerIcon.png"),"Layers");
    m_taskPanels[string]->addTask(m_emptyWidget,QIcon(":/assets/columnIcon.png"),"Columns");
    m_taskPanels[string]->addTask(m_emptyWidget,QIcon(":/assets/neuronIcon.png"),"Neurons");
    m_taskPanels[string]->addTask(m_emptyWidget,QIcon(":/assets/connectionIcon.png"),"Layers");
    QTabWidget::addTab(m_taskPanels[string],string);
}

