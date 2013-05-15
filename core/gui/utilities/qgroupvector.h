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
    void addGroup(QString groupName, QLayout * layout);
    void insertGroup(int index,QString groupName, QLayout * layout);
    void removeGroup(QString groupName);
    bool containsGroup(QString name);
    bool containsGroup(QLayout * layout);
    void setGroupChecked(QString groupName,bool checked);
    void setDirection(Direction direction);
    void setAlignment(Qt::Alignment alignment);
    void setCheckedBehavior(CheckedBehavior behavior);
    void setUncheckedBehavior(UncheckedBehavior state);
    bool isGroupChecked(QString groupName);
    QGroupVector::CheckedBehavior checkBehavior();
    QGroupVector::UncheckedBehavior uncheckedBehavior();

signals:
    void groupChecked(QString groupName,bool checked);

public slots:

private slots:
    void m_groupChecked(QString groupName);

private:
    void m_setGroupEnabled(int index, bool set);

    QBoxLayout * m_layout;
    QVector<QString> m_groupNames;
    QVector<QLayout*> m_groupLayouts;
    QVector<QxtGroupBox*> m_groupBoxes;
    CheckedBehavior m_checkedBehavior;
    UncheckedBehavior m_uncheckedBehavior;
    QSignalMapper * m_checkMapper;
};

#endif // QGROUPVECTOR_H
