#include "distributewidget.h"

DistributeWidget::DistributeWidget(QWidget *parent) :
    QWidget(parent)
{

    m_layout = new QVBoxLayout();

    m_activeColor = QColor(221,72,30);
    m_inactiveColor = QColor(125,125,125);
    m_widgetStack = new SlidingStackedWidget();
    m_loginWidget = new QwwLoginBox();
    m_loginWidget->setFields( QwwLoginBox::HostField | QwwLoginBox::PortField | QwwLoginBox::UserField	| QwwLoginBox::PasswordField
                             |QwwLoginBox::RepeatPasswordField| QwwLoginBox::RememberPasswordField |QwwLoginBox::ProxyField);

    m_widgetStack->addWidget(m_loginWidget);

    m_clusterEditor = new NCSClusterEditor();
    m_widgetStack->addWidget(m_clusterEditor);
    m_widgetStack->setSpeed(300);

    m_layout->addWidget(m_widgetStack);

    m_buttonWidget = new QWidget();
    m_buttonLayout = new QHBoxLayout();

    m_previousButton = new ColorButton();
    m_previousButton->setColor(m_inactiveColor,QColor(255,255,255));
    m_previousButton->hide();
    connect(m_previousButton,SIGNAL(pressed()),m_widgetStack,SLOT(slideInPrev()));
    m_buttonLayout->addWidget(m_previousButton);

    m_nextButton = new ColorButton();
    m_nextButton->setText("Select Hardware >>>");
    m_nextButton->setColor(m_activeColor,QColor(255,255,255));
    connect(m_nextButton,SIGNAL(pressed()),m_widgetStack,SLOT(slideInNext()));
    m_buttonLayout->addWidget(m_nextButton);
    m_buttonWidget->setLayout(m_buttonLayout);

    connect(m_widgetStack,SIGNAL(currentChanged(int)),this,SLOT(m_mainWidgetChanged(int)));
    connect(m_widgetStack,SIGNAL(currentChanged(int)),this,SLOT(m_mainWidgetChanged(int)));

    m_layout->addWidget(m_buttonWidget);
    this->setLayout(m_layout);


}

void DistributeWidget::m_mainWidgetChanged(int index)
{
    if (index == 0)
    {
        m_previousButton->hide();
        m_nextButton->setEnabled(true);
        m_nextButton->setText("Select Hardware >>>");
        m_nextButton->setColor(m_activeColor,QColor(255,255,255));

        disconnect(m_nextButton,SIGNAL(pressed()));
        connect(m_nextButton,SIGNAL(pressed()),m_widgetStack,SLOT(slideInNext()));
    }
    else if (index == 1)
    {
        m_previousButton->show();
        m_previousButton->setEnabled(true);
        m_previousButton->setText("<<< Connect to Cluster");
        m_previousButton->setColor(m_activeColor,QColor(255,255,255));

        m_nextButton->setEnabled(false);
        m_nextButton->setText("Finalize");
        m_nextButton->setColor(m_activeColor,QColor(255,255,255));

        disconnect(m_nextButton,SIGNAL(pressed()));
        connect(m_nextButton,SIGNAL(pressed()),this,SLOT(m_finalize()));
    }
}

void  DistributeWidget::m_finalize()
{

}
