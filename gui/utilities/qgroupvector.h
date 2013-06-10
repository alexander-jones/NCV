#ifndef QGROUPVECTOR_H
#define QGROUPVECTOR_H

#include <QWidget>
#include <QxtGroupBox>
#include <QBoxLayout>
#include <QSignalMapper>

class QGroupVector : public QWidget
{
    Q_OBJECT
public:

    enum Direction
    {
        TopToBottom = QBoxLayout::TopToBottom,
        BottomToTop = QBoxLayout::BottomToTop,
        LeftToRight = QBoxLayout::LeftToRight,
        RightToLeft = QBoxLayout::RightToLeft
    };

    enum CheckedBehavior
    {
        NoChecked,
        SingleChecked,
        MultipleChecked
    };

    enum UncheckedBehavior
    {
        EnableUnchecked,
        DisableUnchecked,
        CollapseUnchecked
    };

    explicit QGroupVector(QWidget *parent = 0);
    void addGroup(QLayout * layout);
    void addGroup(QLayout * layout,const QString & title);
    void insertGroup(int index, QLayout * layout);
    void insertGroup(int index, QLayout * layout,const QString & title);
    void removeGroup(QLayout * layout);
    bool containsGroup(QLayout * layout);
    void setGroupChecked(QLayout * layout,bool checked);
    void setDirection(Direction direction);
    void setAlignment(Qt::Alignment alignment);
    void setCheckedBehavior(CheckedBehavior behavior);
    void setUncheckedBehavior(UncheckedBehavior state);
    bool isGroupChecked(QLayout * layout);
    QGroupVector::CheckedBehavior checkBehavior();
    QGroupVector::UncheckedBehavior uncheckedBehavior();

signals:
    void groupClicked(QLayout * layout,bool checked = false);
    void groupChecked(QLayout * layout);

public slots:

private slots:
    void m_groupChecked(QObject * layout);

private:
    void m_setGroupEnabled(int index, bool set);
    void m_setGroupChecked(int index, bool checked);

    QBoxLayout * m_layout;
    QVector<QLayout*> m_groupLayouts;
    QVector<QxtGroupBox*> m_groupBoxes;
    CheckedBehavior m_checkedBehavior;
    UncheckedBehavior m_uncheckedBehavior;
    QSignalMapper * m_checkMapper;
};

#endif // QGROUPVECTOR_H
