#include "distributewidget.h"

DistributeWidget::DistributeWidget(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QVBoxLayout();
    m_loginWidget = new QwwLoginBox();

    m_loginWidget->setFields( QwwLoginBox::HostField | QwwLoginBox::PortField | QwwLoginBox::UserField	| QwwLoginBox::PasswordField
                             |QwwLoginBox::RepeatPasswordField| QwwLoginBox::RememberPasswordField |QwwLoginBox::ProxyField);
    m_layout->addWidget(m_loginWidget);
    this->setLayout(m_layout);

}
