#include "sidebar.h"
#include <QDebug>


Sidebar::Sidebar(QWidget *parent) :
    QWidget(parent)
{
    m_voidPanel = NULL;
    m_layout = new QVBoxLayout();
    m_toolbar = new QToolBar();
    m_panelSelector = new QComboBox();
    m_toolbar->addWidget(m_panelSelector);
    m_layout->addWidget(m_toolbar);

    m_scrollArea = new QScrollArea();
    m_scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_layout->addWidget(m_scrollArea);

    this->setLayout(m_layout);

    connect(m_panelSelector, SIGNAL( currentIndexChanged(const QString &)), this, SLOT(setPanel(const QString &)));
}
int Sidebar::numPanels()
{
    return m_panels.count();
}
void Sidebar::removePanel(QWidget * panel)
{
    //m_panels.remove(name);
}

void Sidebar::removePanel(QString name)
{
    if (currentPanel() == name)
        m_scrollArea->takeWidget();

    if (m_panels.count() == 1 && m_voidPanel != NULL)
        m_scrollArea->setWidget(m_voidPanel);

    m_panels.remove(name);
    m_panelSelector->removeItem(m_panelSelector->findText(name));
}

bool Sidebar::containsPanel(QString name)
{
    return m_panels.contains(name);
}

QString Sidebar::currentPanel()
{
    return m_panelSelector->currentText();
}

void Sidebar::addTool(QWidget * tool)
{
    m_toolbar->addWidget(tool);
}

void Sidebar::setScrollBarPolicy(Qt::ScrollBarPolicy horizontal,Qt::ScrollBarPolicy vertical)
{
    m_scrollArea->setHorizontalScrollBarPolicy( horizontal);
    m_scrollArea->setVerticalScrollBarPolicy( vertical);
}

void Sidebar::addPanel(QWidget * panel,const QString& name)
{
    m_panels[name] = panel;
    m_panelSelector->addItem(name);
    if (m_panels.count() == 0)
        setPanel(name);

}

void Sidebar::setVoidPanel(QWidget * panel)
{
    m_voidPanel = panel;
    if (m_panels.count() == 0)
        m_scrollArea->setWidget(m_voidPanel);

}
void Sidebar::setPanel(const QString& name)
{
    if (m_panels.contains(name))
    {
        m_scrollArea->takeWidget();
        m_firstPanelAdded = true;
        m_scrollArea->setWidget(m_panels[name]);

        panelChanged(name);
    }
}
