#include "combowidget.h"
#include <QDebug>


ComboWidget::ComboWidget(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QVBoxLayout();
    m_layout->setSpacing(0);

    m_toolbar = new QWidget();
    m_toolbarLayout = new QHBoxLayout();
    m_panelSelector = new QComboBox();
    m_toolbarLayout->addWidget(m_panelSelector);

    m_toolbar->setLayout(m_toolbarLayout);

    m_layout->addWidget(m_toolbar);

    m_voidWidget = new QWidget();
    m_currentWidget = m_voidWidget;
    m_layout->addWidget(m_voidWidget);
    this->setLayout(m_layout);

    connect(m_panelSelector, SIGNAL( currentIndexChanged( QString )), this, SLOT(setWidget( QString )));
}
int ComboWidget::count()
{
    return m_panels.count();
}

void ComboWidget::removeWidget(QString name)
{
    if (currentWidgetName() == name)
        m_layout->removeWidget(m_panels[name]);


    m_panels.remove(name);
    if (m_panels.count() == 0)
        m_layout->addWidget(m_voidWidget);

    m_panelSelector->removeItem(m_panelSelector->findText(name));
}

bool ComboWidget::containsWidget(QString name)
{
    return m_panels.contains(name);
}

QString ComboWidget::currentWidgetName()
{
    return m_panelSelector->currentText();
}

void ComboWidget::addTool(QWidget * tool)
{
    m_toolbarLayout->addWidget(tool);
    m_tools.append(tool);
}
void ComboWidget::removeTool(QWidget * tool)
{
    m_toolbarLayout->removeWidget(tool);
    for (int i =0; i < m_tools.size();i++)
        if (m_tools[i] == tool)
        {
            m_tools.remove(i);
            break;
        }

}



void ComboWidget::addWidget(QWidget * panel,const QString& name)
{
    m_panels[name] = panel;
    m_panelSelector->addItem(name);

}
bool ComboWidget::containsTool(QWidget * tool)
{
    return m_tools.contains(tool);
}

void ComboWidget::setVoidWidget(QWidget * panel)
{

    if (m_panels.count() == 0)
        m_layout->removeWidget(m_currentWidget);

    m_voidWidget = panel;

    if (m_panels.count() == 0)
        m_layout->addWidget(m_voidWidget);

}
void ComboWidget::setWidget( QString name)
{
    if (m_panels.contains(name))
    {
        m_layout->removeWidget(m_currentWidget);
        m_currentWidget->hide();
        m_currentWidget = m_panels[name];
        m_layout->addWidget(m_currentWidget);
        m_currentWidget->show();

        widgetChanged(name);
    }

}

QWidget * ComboWidget::currentWidget()
{
    return m_currentWidget;
}

QWidget * ComboWidget::voidWidget()
{
    return m_voidWidget;
}
