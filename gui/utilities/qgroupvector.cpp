#include "qgroupvector.h"
#include <QDebug>
QGroupVector::QGroupVector(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_checkedBehavior = NoChecked;
    m_uncheckedBehavior = EnableUnchecked;
    m_checkMapper = new QSignalMapper();
    connect(m_checkMapper,SIGNAL(mapped(QObject*)),this,SLOT(m_groupChecked(QObject*)));
    this->setLayout(m_layout);
}


void QGroupVector::addGroup(QLayout * layout)
{
    insertGroup(m_groupBoxes.count(),layout);
}

void QGroupVector::addGroup(QLayout * layout,const QString & title)
{
    insertGroup(m_groupBoxes.count(),layout);
    m_groupBoxes[m_groupBoxes.count() -1]->setTitle(title);
}

void QGroupVector::insertGroup(int index, QLayout * layout)
{
    m_groupLayouts.insert(index,layout);

    QxtGroupBox * groupBox = new QxtGroupBox();
    groupBox->setLayout(layout);
    m_groupBoxes.insert(index,groupBox);
    m_layout->insertWidget(index,groupBox);

    m_groupBoxes[index]->setCollapsive(m_uncheckedBehavior == CollapseUnchecked);

    if (m_checkedBehavior == NoChecked)
        m_groupBoxes[index]->setCheckable(false);
    else
    {
        m_groupBoxes[index]->setCheckable(true);
        m_setGroupChecked(index,true);
        connect(m_groupBoxes[index],SIGNAL(clicked(bool)),m_checkMapper,SLOT(map()));
        m_checkMapper->setMapping(m_groupBoxes[index],layout);
    }

    if (m_checkedBehavior == SingleChecked)
    {
        if (m_groupBoxes.count() > 1)
            m_setGroupChecked(index,false);
        else
            m_setGroupChecked(index,true);

    }
}

void QGroupVector::insertGroup(int index, QLayout * layout,const QString & title)
{
    insertGroup(index,layout);
    m_groupBoxes[index]->setTitle(title);
}


void QGroupVector::removeGroup(QLayout * layout)
{
    int index = m_groupLayouts.indexOf(layout);
    if (index == -1)
        return;

    bool checkNewAfterRemove = (m_checkedBehavior == SingleChecked && m_groupBoxes[index]->isChecked());

    m_layout->removeWidget(m_groupBoxes[index]);
    m_groupBoxes[index]->setParent(this);
    m_groupBoxes[index]->hide();
    m_groupLayouts.remove(index);
    m_groupBoxes.remove(index);

    if (checkNewAfterRemove && m_groupBoxes.count() > 0)
        setGroupChecked(m_groupLayouts[0],true);
}
bool QGroupVector::containsGroup(QLayout * layout)
{
    return m_groupLayouts.contains(layout);
}
void QGroupVector::setDirection(Direction direction)
{
    m_layout->setDirection((QBoxLayout::Direction)direction);
}
void QGroupVector::setAlignment(Qt::Alignment alignment)
{
    m_layout->setAlignment(alignment);
}

void QGroupVector::setGroupChecked(QLayout * layout,bool checked)
{
    int index = m_groupLayouts.indexOf(layout);
    if (index == -1)
        return;

    if (m_checkedBehavior == NoChecked)
        return;

    if (m_checkedBehavior == MultipleChecked)
        m_setGroupChecked(index,checked);

    else if (m_checkedBehavior == SingleChecked)
    {
        if (checked && !m_groupBoxes[index]->isChecked())
            for (int i = 0; i < m_groupBoxes.count(); i ++)
            {
                if (index == i)
                    m_setGroupChecked(i,true);

                else if (m_groupBoxes[i]->isChecked())
                    m_setGroupChecked(i,false);
            }
    }

}

void QGroupVector::setUncheckedBehavior(UncheckedBehavior behavior)
{
    if (m_uncheckedBehavior == behavior)
        return;

    m_uncheckedBehavior = behavior;
    for (int i = 0; i < m_groupBoxes.count(); i ++)
    {
        if (m_uncheckedBehavior == CollapseUnchecked)
        {
            m_groupBoxes[i]->setCollapsive(true);
            if (!m_groupBoxes[i]->isChecked())
                m_groupBoxes[i]->setCollapsed(true);
        }
        else
        {
            bool enableGroups = m_uncheckedBehavior == EnableUnchecked;
            m_groupBoxes[i]->setCollapsive(false);
            m_setGroupEnabled(i, enableGroups);
        }

    }
}

void QGroupVector::setCheckedBehavior(CheckedBehavior behavior )
{

    if (m_checkedBehavior == behavior)
        return;

    bool boxChecked = false;
    for (int i = 0; i < m_groupBoxes.count(); i ++)
    {
        if (behavior == NoChecked)
        {
            if (m_checkedBehavior != NoChecked)
            {
                disconnect(m_groupBoxes[i],SIGNAL(clicked(bool)),m_checkMapper,SLOT(map()));
                m_checkMapper->removeMappings(m_groupBoxes[i]);
                m_groupBoxes[i]->setCheckable(false);
            }
        }
        else if (behavior == SingleChecked)
        {
            if (m_checkedBehavior == NoChecked)
            {
                m_groupBoxes[i]->setCheckable(true);

                if (!boxChecked)
                {
                    m_setGroupChecked(i,true);
                    boxChecked = true;
                }
                else
                    m_setGroupChecked(i,false);

                connect(m_groupBoxes[i],SIGNAL(clicked(bool)),m_checkMapper,SLOT(map()));
                m_checkMapper->setMapping(m_groupBoxes[i],m_groupLayouts[i]);
            }
            else if (m_checkedBehavior == MultipleChecked)
            {
                if (!boxChecked)
                {
                    if (m_groupBoxes[i]->isChecked())
                        boxChecked = true;

                    else if (i == ( m_groupBoxes.count() -1 ) )
                        m_setGroupChecked(0,true);

                }
                else if (m_groupBoxes[i]->isChecked())
                    m_setGroupChecked(i,false);

            }

        }
        else if (behavior == MultipleChecked)
        {
            if (m_checkedBehavior == NoChecked)
            {
                m_groupBoxes[i]->setCheckable(true);

                if (m_groupBoxes[i]->isChecked())
                    m_setGroupEnabled(i,true);

                connect(m_groupBoxes[i],SIGNAL(clicked(bool)),m_checkMapper,SLOT(map()));
                m_checkMapper->setMapping(m_groupBoxes[i],m_groupLayouts[i]);
            }

        }
    }

    m_checkedBehavior = behavior;

}

void QGroupVector::m_groupChecked(QObject * layout)
{
    QLayout * groupLayout= (QLayout *) layout;
    int groupIndex = m_groupLayouts.indexOf(groupLayout);
    bool newCheck = true;

    m_groupBoxes[groupIndex]->blockSignals(true);
    if (m_checkedBehavior == SingleChecked)
    {
        // if group was unchecked during single check behavior,
        // then check it again so that at least one group is checked.
        if (!m_groupBoxes[groupIndex]->isChecked())
        {
            m_setGroupChecked(groupIndex,true);
            newCheck = false;
        }
        else
        {
            m_setGroupChecked(groupIndex,true);
            for (int i = 0; i < m_groupBoxes.count(); i ++)
                if (i != groupIndex)
                    m_setGroupChecked(i,false);
        }
    }
    else if (m_checkedBehavior == MultipleChecked)
        m_setGroupChecked(groupIndex,m_groupBoxes[groupIndex]->isChecked());

    m_groupBoxes[groupIndex]->blockSignals(false);

    if (newCheck)
        groupChecked(groupLayout);
    groupClicked(groupLayout,m_groupBoxes[groupIndex]->isChecked());
}


bool QGroupVector::isGroupChecked(QLayout * layout)
{
    int index = m_groupLayouts.indexOf(layout);
    return m_groupBoxes[index]->isChecked();
}

QGroupVector::CheckedBehavior QGroupVector::checkBehavior()
{
    return m_checkedBehavior;
}

QGroupVector::UncheckedBehavior QGroupVector::uncheckedBehavior()
{
    return m_uncheckedBehavior;
}


void QGroupVector::m_setGroupEnabled(int index, bool set)
{
    for (int i = 0; i < m_groupLayouts[index]->count(); i ++)
        m_groupLayouts[index]->itemAt(i)->widget()->setEnabled(set);
}

void QGroupVector::m_setGroupChecked(int index, bool checked)
{
    if (checked)
    {
        m_setGroupEnabled(index,true);
        if (m_uncheckedBehavior == CollapseUnchecked)
            m_groupBoxes[index]->setCollapsed(false);
    }
    else
    {
        if (m_uncheckedBehavior == DisableUnchecked)
            m_setGroupEnabled(index,false);
        else if (m_uncheckedBehavior == CollapseUnchecked)
            m_groupBoxes[index]->setCollapsed(true);
    }
    m_groupBoxes[index]->setChecked(checked);
}
