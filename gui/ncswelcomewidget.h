#ifndef NCSWELCOMEWIDGET_H
#define NCSWELCOMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include "gui/utilities/qwidgetvector.h"
#include <QPainter>
#include <QLinearGradient>


class NCSWelcomeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NCSWelcomeWidget(QWidget *parent = 0);
    void addQuickPanel(QPixmap image, QString text = "", QString desc = "");
    void insertQuickPanel(int index, QPixmap image, QString text = "", QString desc = "");
    void removeQuickPanel(int index);
    void setPanelEnabled(int index, bool enabled);
    void setPanelSize(QSize size);

signals:
    void quickPanelTriggered(int index);

private slots:
    void m_panelPressed(QWidget * panel);
    
private:
    struct PanelSpec
    {
        QPushButton * button;
        QLabel * title, * description;
        QWidgetVector * textRegion;
    };

    QFrame * m_makeSeparator(QFrame::Shape shape);

    int m_descWidth;
    QSignalMapper * m_panelMapper;
    QSize m_panelSize;
    QWidget * m_panelWidget;
    QGridLayout * m_panelLayout;
    QVector< PanelSpec > m_quickPanels;

    QWidget * m_leftSidePanel, * m_rightSidePanel;
    QWidgetVector * m_bodyVector,* m_headerVector, * m_footerVector,* m_descriptionVector, * m_centralVector, *m_mainVector;
    QVBoxLayout * m_layout;
    QLabel * m_footerLabel,  * m_unrLabel;
    QLabel * m_welcomeLabel,  * m_titleLabel;
};

#endif // NCSWELCOMEWIDGET_H
