
#include "spinbox3d.h"
#include <QDebug>
#include <QLineEdit>


ExpandableSpinBox::ExpandableSpinBox(QWidget *parent)
    :QDoubleSpinBox(parent)
{
    connect(this->lineEdit(),SIGNAL(selectionChanged()),this,SIGNAL(entered()));
}

SpinBox3D::SpinBox3D(QWidget * parent )
    :QWidget(parent)
{
    m_normalized = false;
    m_layout = new QHBoxLayout();
    m_layout->setAlignment(Qt::AlignCenter);

    m_x = new ExpandableSpinBox();
    m_layout->addWidget(m_x);
    connect(m_x,SIGNAL(entered()),this,SLOT(m_expandX()));
    connect(m_x,SIGNAL(editingFinished()),this,SLOT(m_subValueSubmitted()));

    m_y = new ExpandableSpinBox();
    m_layout->addWidget(m_y);
    connect( m_y,SIGNAL(entered()),this,SLOT(m_expandY()));
    connect(m_y,SIGNAL(editingFinished()),this,SLOT(m_subValueSubmitted()));

    m_z = new ExpandableSpinBox();
    m_layout->addWidget(m_z);
    this->setLayout(m_layout);
    connect(m_z,SIGNAL(entered()),this,SLOT(m_expandZ()));
    connect(m_z,SIGNAL(editingFinished()),this,SLOT(m_subValueSubmitted()));
}

QDoubleSpinBox  * SpinBox3D::x() {return m_x;}
QDoubleSpinBox  * SpinBox3D::y() {return m_y;}
QDoubleSpinBox  * SpinBox3D::z() {return m_z;}

QVector3D SpinBox3D::value()
{
    return QVector3D(m_x->value(),m_y->value(),m_z->value());
}
void SpinBox3D::setMinimum(double x, double y, double z)
{
    m_x->setMinimum(x);
    m_y->setMinimum(y);
    m_z->setMinimum(z);
}

void SpinBox3D::setValue(double x, double y, double z)
{
    m_x->setValue(x);
    m_y->setValue(y);
    m_z->setValue(z);
}
void SpinBox3D::setValue(QVector3D value)
{
    m_x->setValue(value.x());
    m_y->setValue(value.y());
    m_z->setValue(value.z());
}


void SpinBox3D::setMaximum(double x, double y, double z)
{
    m_x->setMaximum(x);
    m_y->setMaximum(y);
    m_z->setMaximum(z);
}

void SpinBox3D::setAutoNormalize(double normalizedMax)
{
    m_normalized = !m_normalized;
    m_normalizedMax = normalizedMax;
}
void SpinBox3D::setFont(const QFont & font)
{
    m_x->setFont(font);
    m_y->setFont(font);
    m_z->setFont(font);
}

void SpinBox3D::m_subValueSubmitted()
{
    if (m_normalized)
    {
        QVector3D normalized =  QVector3D(m_x->value(),m_y->value(),m_z->value()).normalized() * m_normalizedMax;
        m_x->setValue(normalized.x());
        m_y->setValue(normalized.y());
        m_z->setValue(normalized.z());

    }
    m_x->show();
    m_y->show();
    m_z->show();
    valueEdited();
}

void SpinBox3D::setX(double val)
{
    m_x->disconnect();
    m_x->setValue(val);
    connect(m_x,SIGNAL(entered()),this,SLOT(m_expandX()));
    connect(m_x,SIGNAL(editingFinished()),this,SLOT(m_subValueSubmitted()));
}


void SpinBox3D::setY(double val)
{
    m_y->disconnect();
    m_y->setValue(val);
    connect(m_y,SIGNAL(entered()),this,SLOT(m_expandY()));
    connect(m_y,SIGNAL(editingFinished()),this,SLOT(m_subValueSubmitted()));
}


void SpinBox3D::setZ(double val)
{
    m_z->disconnect();
    m_z->setValue(val);
    connect(m_z,SIGNAL(entered()),this,SLOT(m_expandZ()));
    connect(m_z,SIGNAL(editingFinished()),this,SLOT(m_subValueSubmitted()));
}

void SpinBox3D::m_expandX()
{
    /*m_x->show();
    m_y->hide();
    m_z->hide();*/
}

void SpinBox3D::m_expandY()
{

    /*m_x->hide();
    m_y->show();
    m_z->hide();*/
}


void SpinBox3D::m_expandZ()
{

    /*m_x->hide();
    m_y->hide();
    m_z->show();*/
}
