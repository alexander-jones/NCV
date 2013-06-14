#ifndef NCSDIALOG_H
#define NCSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QFormLayout>
#include "gui/utilities/qwidgetvector.h"

class NCSDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NCSDialog(QWidget *parent = 0);
    void setTitle(const QString &title);
    void setDescription(const QString &title);
    void setWidget(QWidget * widget);
    QWidget * widget();
    QString description();
    QString title();

private:
    QWidget * m_widget;
    QWidgetVector * m_bodyVector, * m_headerVector,* m_descriptionVector;
    QVBoxLayout * m_layout;
    QLabel * m_descriptionLabel, * m_titleLabel, * m_iconLabel;
};

#endif // NCSDIALOG_H
