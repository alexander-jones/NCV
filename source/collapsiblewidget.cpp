#include "collapsiblewidget.h"

CollapsibleWidget::CollapsibleWidget(QWidget * widget, const QString& header, QWidget *parent) :
    QFrame(parent)
{
    m_layout = new QVBoxLayout();

    m_headerBar = new QWidget();
    m_headerBarLayout = new QHBoxLayout();

    m_collapsed =false;
    m_collapseToolOne = new QToolButton();
    m_collapseToolOne->setText("-");
    m_headerBarLayout->addWidget(m_collapseToolOne);

    m_header = new QLabel(header);
    m_header->setAlignment(Qt::AlignCenter);
    m_headerBarLayout->addWidget(m_header);

    m_collapseToolTwo = new QToolButton();
    m_collapseToolTwo->setText("-");
    m_headerBarLayout->addWidget(m_collapseToolTwo);

    m_headerBar->setLayout(m_headerBarLayout);
    m_layout->addWidget(m_headerBar);

    m_headerUnderline = new QFrame();
    m_headerUnderline->setFrameShape(QFrame::NoFrame);
    m_layout->addWidget(m_headerUnderline);

    m_widget = widget;
    if (m_widget != NULL)
        m_layout->addWidget(m_widget);

    this->setLayout(m_layout);
    connect(m_collapseToolOne, SIGNAL(clicked()),this,SLOT(m_collapseToolPressed()));
    connect(m_collapseToolTwo, SIGNAL(clicked()),this,SLOT(m_collapseToolPressed()));

}

CollapsibleWidget::~CollapsibleWidget()
{
   delete m_layout;
   delete m_header;

   delete m_collapseToolOne;
   delete m_collapseToolTwo;

   delete m_widget;
   delete m_headerUnderline;
   delete m_headerBar;
   delete m_headerBarLayout;
}

void CollapsibleWidget::setWidget(QWidget * widget)
{
    if (m_widget != NULL)
        m_layout->removeWidget(m_widget);
    m_widget = widget;
    m_layout->addWidget(m_widget);
}


void CollapsibleWidget::underlineHeader(bool underline)
{
    if (underline)
    {
        m_headerUnderline->setFrameShape(QFrame::HLine);
        m_headerUnderline->setFrameShadow(QFrame::Sunken);
    }
    else
        m_headerUnderline->setFrameShape(QFrame::NoFrame);
}

void CollapsibleWidget::setHeader(const QString& text)
{
    m_header->setText(text);
}


void CollapsibleWidget::collapse()
{
    m_collapsed =!m_collapsed;
    m_widget->hide();
    m_collapseToolOne->setText("+");
    m_collapseToolTwo->setText("+");
    collapsed();
}

void CollapsibleWidget::expand()
{
    m_collapsed =!m_collapsed;
    m_widget->show();
    m_collapseToolOne->setText("-");
    m_collapseToolTwo->setText("-");
    expanded();
}

void CollapsibleWidget::m_collapseToolPressed()
{
    if (!m_collapsed)
        collapse();
    else
        expand();
}
