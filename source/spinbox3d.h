#ifndef SPINBOX3D_H
#define SPINBOX3D_H

#include <QSpinBox>
#include <QVector3D>
#include <QHBoxLayout>

class ExpandableSpinBox: public QDoubleSpinBox
{
    Q_OBJECT
public:
    ExpandableSpinBox(QWidget *parent = 0);

signals:
    void entered();

};

class SpinBox3D : public QWidget
{
    Q_OBJECT
public:
    SpinBox3D(QWidget *parent = 0);
    QDoubleSpinBox  * x();
    QDoubleSpinBox  * y();
    QDoubleSpinBox  * z();

    QVector3D value();
    void setMinimum(double x, double y, double z);
    void setValue(double x, double y, double z);
    void setValue(QVector3D value);
    void setMaximum(double x, double y, double z);

    void setAutoNormalize(double normalizedMax = 1.0f);
    void setFont(const QFont & font);
    void setX(double val);
    void setY(double val);
    void setZ(double val);

signals:
    void valueEdited();

private slots:
    void m_subValueSubmitted();
    void m_expandX();
    void m_expandY();
    void m_expandZ();

private:
    double m_normalizedMax;
    bool m_normalized;
    QHBoxLayout *  m_layout;
    ExpandableSpinBox  * m_x, * m_y,* m_z;
    
};

#endif // SPINBOX3D_H
