#ifndef QGROUPVECTOR_H
#define QGROUPVECTOR_H

#include <QWidget>
#include <QGroupBox>
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

    enum CheckState
    {
        CheckNone,
        CheckMultiple,
        CheckSingle
    };


    explicit QGroupVector(QWidget *parent = 0);
    void addGroup(QString groupName);
    void insertGroup(int index, QString groupName);
    void removeGroup(QString groupName);
    bool containsGroup(QString name);
    void setGroupAlignment(QString groupName,Qt::Alignment);
    void setGroupDirection(QString groupName,Direction);
    void setGroupChecked(QString groupName,bool checked);
    void addToGroup(QString groupName,QWidget * widget);
    void insertInGroup(QString groupName,int index, QWidget * widget);
    void removeFromGroup(QString groupName, QWidget * widget);
    bool groupContainsWidget(QString groupName,QWidget * widget);
    void setDirection(Direction direction);
    void setAlignment(Qt::Alignment alignment);
    void setCheckState(CheckState state);
    void setEnableOnlyChecked(bool set);
    bool isGroupChecked(QString groupName);
    bool onlyCheckedEnabled();
    QGroupVector::CheckState checkState();

signals:
    void groupChecked(QString groupName,bool checked);

public slots:

private slots:
    void m_groupChecked(QString groupName);

private:
    void m_setGroupEnabled(int index, bool set);

    QBoxLayout * m_layout;
    QVector<QString> m_groupNames;
    QVector<QBoxLayout*> m_groupLayouts;
    QVector<QGroupBox*> m_groupBoxes;
    CheckState m_checkState;
    QSignalMapper * m_checkStateMapper;
    bool m_onlyEnableOnChecked;
};

#endif // QGROUPVECTOR_H
