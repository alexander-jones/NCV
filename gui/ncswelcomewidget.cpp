#include "ncswelcomewidget.h"
#include <QDebug>
#include <QToolButton>

NCSWelcomeWidget::NCSWelcomeWidget(QWidget *parent) :
    QWidget(parent)
{

    m_panelMapper = new QSignalMapper(this);
    connect(m_panelMapper,SIGNAL(mapped(QWidget*)),this,SLOT(m_panelPressed(QWidget*)));

    QPalette sidebarPallete(palette());
    sidebarPallete.setColor(QPalette::Background, tr("#e5e5e5"));

    m_layout = new QVBoxLayout();
    m_layout->addWidget(m_makeSeparator(QFrame::HLine));

    m_mainVector = new QWidgetVector();

    m_leftSidePanel = new QWidget();
    m_leftSidePanel->setAutoFillBackground(true);
    m_leftSidePanel->setPalette(sidebarPallete);

    m_mainVector->addWidget(m_leftSidePanel);

    m_mainVector->addWidget(m_makeSeparator(QFrame::VLine));

    m_centralVector = new QWidgetVector();
    m_centralVector->setAlignment(Qt::AlignTop);
    m_centralVector->setDirection(QWidgetVector::TopToBottom);

    m_welcomeLabel = new QLabel("<font color = '#dd4814'> Welcome</font> ");
    QFont font = m_welcomeLabel->font();
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    font.setPointSize(56);
    font.setBold(true);
    m_welcomeLabel->setFont(font);
    m_centralVector->addWidget(m_welcomeLabel);
    m_titleLabel = new QLabel("To Neocortical View");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setFont(font);
    m_centralVector->addWidget(m_titleLabel);


    m_panelWidget = new QWidget();
    m_panelLayout = new QGridLayout();
    m_panelLayout->setAlignment(Qt::AlignCenter);
    m_panelWidget->setLayout(m_panelLayout);
    m_centralVector->addWidget(m_panelWidget);

    m_centralVector->addWidget(m_makeSeparator(QFrame::HLine));

    m_footerVector = new QWidgetVector();
    m_footerVector->setAlignment(Qt::AlignCenter);
    m_footerVector->setSpacing(25);
    m_footerVector->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    m_footerLabel = new QLabel("Developed by the Brain Computation Lab <font color = '#dd4814'><b>@</b></font>" );
    m_footerLabel->setAlignment(Qt::AlignCenter);
    font.setPointSize(24);
    font.setBold(false);
    m_footerLabel->setFont(font);
    m_footerVector->addWidget(m_footerLabel);


    m_unrLabel = new QLabel();
    m_unrLabel->setPixmap(QPixmap(":/resources/images/unr.png"));
    m_footerVector->addWidget(m_unrLabel);

    m_centralVector->addWidget(m_footerVector);

    m_mainVector->addWidget(m_centralVector);


    m_mainVector->addWidget(m_makeSeparator(QFrame::VLine));

    m_rightSidePanel = new QWidget();
    m_rightSidePanel->setAutoFillBackground(true);
    m_rightSidePanel->setPalette(sidebarPallete);
    m_mainVector->addWidget(m_rightSidePanel);


    m_layout->addWidget(m_mainVector);
    this->setLayout(m_layout);

    this->setPanelSize(QSize(175,175));

}


void NCSWelcomeWidget::setPanelEnabled(int index, bool enabled)
{
    m_quickPanels[index].button->setEnabled(enabled);
    m_quickPanels[index].textRegion->setEnabled(enabled);
}

void NCSWelcomeWidget::addQuickPanel(QPixmap image, QString text, QString desc )
{
    insertQuickPanel(m_quickPanels.count(),image,text,desc);
}

void NCSWelcomeWidget::setPanelSize(QSize size)
{
    m_panelSize = size;
    m_descWidth = m_panelSize.width() * 1.5;

    for (int i = 0; i < m_quickPanels.count(); i ++)
    {
        m_quickPanels[i].button->setIconSize(m_panelSize);
        m_quickPanels[i].description->setMinimumWidth(m_descWidth);
        m_quickPanels[i].description->setMaximumWidth(m_descWidth);
    }
}

void NCSWelcomeWidget::insertQuickPanel(int index, QPixmap image, QString text, QString desc )
{
    PanelSpec panel;
    panel.button = new QPushButton();
    panel.button->setContentsMargins(0,0,0,0);
    panel.button->setIcon(QIcon(image));
    panel.button->setIconSize(m_panelSize);
    m_panelMapper->setMapping(panel.button,panel.button);
    connect(panel.button,SIGNAL(pressed()),m_panelMapper,SLOT(map()));
    m_panelLayout->addWidget(panel.button,0,index);

    panel.textRegion = new QWidgetVector(this);
    panel.textRegion->setAlignment(Qt::AlignHCenter);
    panel.textRegion->setDirection(QWidgetVector::TopToBottom);
    panel.title = new QLabel(text);
    panel.title->setAlignment(Qt::AlignHCenter);
    QFont font = panel.title->font();
    font.setPointSize(22);
    font.setBold(true);
    panel.title->setFont(font);
    panel.textRegion->addWidget(panel.title);

    panel.description = new QLabel(desc);
    panel.description->setMinimumWidth(m_descWidth);
    panel.description->setMaximumWidth(m_descWidth);
    panel.description->setWordWrap(true);
    panel.description->setAlignment(Qt::AlignHCenter);
    panel.textRegion->addWidget(panel.description);
    m_panelLayout->addWidget(panel.textRegion,1,index);

    m_quickPanels.insert(index,panel);
}

void NCSWelcomeWidget::removeQuickPanel(int index)
{
    m_panelLayout->removeWidget(m_quickPanels[index].button);
    delete m_quickPanels[index].button;

    m_panelLayout->removeWidget(m_quickPanels[index].textRegion);
    delete m_quickPanels[index].textRegion;

    m_quickPanels.remove(index);
}

QFrame * NCSWelcomeWidget::m_makeSeparator(QFrame::Shape shape)
{
    QFrame * sep = new QFrame();
    sep->setFrameShape(shape);
    sep->setFrameShadow(QFrame::Sunken);
    return sep;
}

void NCSWelcomeWidget::m_panelPressed(QWidget* panel)
{
    for (int i = 0; i < m_quickPanels.count(); i ++)
    {
        if (m_quickPanels[i].button == (QPushButton *)panel)
        {

            quickPanelTriggered(i);
            return;
        }
    }
}
