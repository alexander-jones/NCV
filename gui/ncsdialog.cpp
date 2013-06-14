#include "ncsdialog.h"
NCSDialog::NCSDialog(QWidget *parent) :
    QDialog(parent)
{
    m_layout = new QVBoxLayout();

    m_headerVector = new QWidgetVector();
    m_headerVector->setAlignment(Qt::AlignLeft);
    m_headerVector->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    m_iconLabel = new QLabel();
    m_iconLabel->setPixmap(QPixmap(":/resources/images/ncvDialogIcon.png"));
    m_headerVector->addWidget(m_iconLabel);

    m_descriptionVector = new QWidgetVector();
    m_descriptionVector->setDirection(QWidgetVector::TopToBottom);
    m_titleLabel = new QLabel(this->windowTitle());
    m_descriptionVector->addWidget(m_titleLabel);
    m_descriptionLabel = new QLabel();
    m_descriptionVector->addWidget(m_descriptionLabel);
    m_headerVector->addWidget(m_descriptionVector);

    m_layout->addWidget(m_headerVector);

    QFrame * sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    m_layout->addWidget(sep);

    m_widget = NULL;

    this->setLayout(m_layout);

}

void NCSDialog::setTitle(const QString & title)
{
    m_titleLabel->setText( title);
}
QString NCSDialog::title()
{
    return m_titleLabel->text();
}

void NCSDialog::setWidget(QWidget * widget)
{
    if (m_widget != NULL)
    {
        m_layout->removeWidget(m_widget);
        m_widget = NULL;
    }
    m_widget = widget;
    m_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_layout->addWidget(m_widget);
}
QWidget * NCSDialog::widget()
{
    return m_widget;
}
void NCSDialog::setDescription(const QString &desc)
{
    m_descriptionLabel->setText(desc);
}
QString NCSDialog::description()
{
    return m_descriptionLabel->text();
}
