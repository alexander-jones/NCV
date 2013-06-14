#include "combowidget.h"
#include <QDebug>


ComboWidget::ComboWidget(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QVBoxLayout();
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0,0,0,0);

    m_toolbar = new QWidget();
    m_toolbarLayout = new QHBoxLayout();
    m_widgetSelector = new QComboBox();
    m_toolbarLayout->addWidget(m_widgetSelector);

    m_toolbar->setLayout(m_toolbarLayout);

    m_layout->addWidget(m_toolbar);

    m_voidWidget = new QWidget();
    m_currentWidget = m_voidWidget;
    m_layout->addWidget(m_voidWidget);
    this->setLayout(m_layout);

    connect(m_widgetSelector, SIGNAL( currentIndexChanged( QString )), this, SLOT(setWidget( QString )));
}
int ComboWidget::count()
{
    return m_widgets.count();
}

void ComboWidget::setWidget( QWidget * widget)
{
	QString key = m_widgets.key(widget, "");
	if (key  != "")
	{

		m_layout->removeWidget(m_currentWidget);
		m_currentWidget->hide();
		m_currentWidget = widget;
		m_layout->addWidget(m_currentWidget);
		m_currentWidget->show();

		widgetChanged(widget);
		widgetChanged(key);
	
	}

}
void ComboWidget::removeWidget(QString name)
{
    if (currentWidgetKey() == name)
        m_layout->removeWidget(m_widgets[name]);


    m_widgets.remove(name);
    if (m_widgets.count() == 0)
        m_layout->addWidget(m_voidWidget);

    m_currentWidget = m_voidWidget;
    m_widgetSelector->removeItem(m_widgetSelector->findText(name));
}

bool ComboWidget::containsWidget(QString name)
{
    return m_widgets.contains(name);
}

QString ComboWidget::currentWidgetKey()
{
    return m_widgetSelector->currentText();
}

void ComboWidget::addTool(QWidget * tool)
{
    m_toolbarLayout->addWidget(tool);
}
void ComboWidget::insertTool(int index,QWidget * tool)
{
    m_toolbarLayout->insertWidget(index,tool);
}

void ComboWidget::removeTool(QWidget * tool)
{
    m_toolbarLayout->removeWidget(tool);
}



void ComboWidget::addWidget(QWidget * panel,const QString& name)
{
    m_widgets[name] = panel;
    m_widgetSelector->addItem(name);

}
bool ComboWidget::containsTool(QWidget * tool)
{
    return m_toolbarLayout->indexOf(tool) != -1;
}

void ComboWidget::setVoidWidget(QWidget * panel)
{

    if (m_widgets.count() == 0)
    {
        m_layout->removeWidget(m_currentWidget);
        m_currentWidget->hide();
    }

    m_voidWidget = panel;

    if (m_widgets.count() == 0)
    {
        m_currentWidget = m_voidWidget;
        m_layout->addWidget(m_voidWidget);
        m_voidWidget->show();
    }

}
void ComboWidget::setWidget( QString name)
{
    if (m_widgets.contains(name))
    {
        m_layout->removeWidget(m_currentWidget);
        m_currentWidget->hide();
        m_currentWidget = m_widgets[name];
        m_layout->addWidget(m_currentWidget);
        m_currentWidget->show();

		widgetChanged(m_widgets[name]);
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
