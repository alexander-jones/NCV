#include "qgroupvector.h"
#include <QDebug>
QGroupVector::QGroupVector(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_checkedBehavior = NoChecked;
    m_uncheckedBehavior = EnableUnchecked;
    m_checkMapper = new QSignalMapper();
    connect(m_checkMapper,SIGNAL(mapped(QString)),this,SLOT(m_groupChecked(QString)));
    this->setLayout(m_layout);
}

void QGroupVector::addGroup(QString groupName, QLayout * layout)
{
    insertGroup(m_groupNames.count(),groupName,layout);
}
void QGroupVector::insertGroup(int index, QString groupName,QLayout *  layout )
{
    m_groupNames.insert(index,groupName);
    m_groupLayouts.insert(index,layout);

    QxtGroupBox * groupBox = new QxtGroupBox(groupName);
    groupBox->setLayout(layout);
    m_groupBoxes.insert(index,groupBox);
    m_layout->insertWidget(index,groupBox);

    m_groupBoxes[index]->setCollapsive(m_uncheckedBehavior == CollapseUnchecked);

    if (m_checkedBehavior == NoChecked)
        m_groupBoxes[index]->setCheckable(false);
    else
    {
        m_groupBoxes[index]->setCheckable(true);
        connect(m_groupBoxes[index],SIGNAL(clicked(bool)),m_checkMapper,SLOT(map()));
        m_checkMapper->setMapping(m_groupBoxes[index],m_groupNames[index]);
    }

    if (m_checkedBehavior == SingleChecked)
    {
        bool boxAlreadyChecked = false;
        for (int i = 0; i < m_groupBoxes.count(); i ++)
        {
            if (!boxAlreadyChecked)
            {
                m_groupBoxes[i]->setChecked(true);
                m_setGroupEnabled(i,true);
                boxAlreadyChecked = true;
                if (m_uncheckedBehavior == CollapseUnchecked)
                    m_groupBoxes[i]->setCollapsed(false);
            }
            else
            {
                m_groupBoxes[i]->setChecked(false);
                if (m_uncheckedBehavior == DisableUnchecked)
                    m_setGroupEnabled(i,false);
                else if (m_uncheckedBehavior == CollapseUnchecked)
                    m_groupBoxes[i]->setCollapsed(true);
            }
        }
    }
}
void QGroupVector::removeGroup(QString groupName)
{
    int index = m_groupNames.indexOf(groupName);
    if (index == -1)
        return;
    m_layout->removeWidget(m_groupBoxes[index]);
    m_groupBoxes[index]->hide();
    m_groupNames.remove(index);
    m_groupLayouts.remove(index);
    m_groupBoxes.remove(index);
}
bool QGroupVector::containsGroup(QString name)
{
    return m_groupNames.contains(name);
}
void QGroupVector::setDirection(Direction direction)
{
    m_layout->setDirection((QBoxLayout::Direction)direction);
}
void QGroupVector::setAlignment(Qt::Alignment alignment)
{
    m_layout->setAlignment(alignment);
}

void QGroupVector::setGroupChecked(QString groupName,bool checked)
{
    int index = m_groupNames.indexOf(groupName);
    if (index == -1)
        return;
    m_groupBoxes[index]->setChecked(checked);
}

void QGroupVector::setUncheckedBehavior(UncheckedBehavior behavior)
{
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
    bool boxAlreadyChecked = false;
    for (int i = 0; i < m_groupBoxes.count(); i ++)
    {
        if (behavior == NoChecked)
        {
            if (m_checkedBehavior != NoChecked)
                disconnect(m_groupBoxes[i],SIGNAL(clicked(bool)),m_checkMapper,SLOT(map()));
            m_groupBoxes[i]->setCheckable(false);
        }
        else if (behavior == SingleChecked)
        {
            m_groupBoxes[i]->setCheckable(true);
            if (!boxAlreadyChecked)
            {
                m_groupBoxes[i]->setChecked(true);
                boxAlreadyChecked = true;
                m_setGroupEnabled(i,true);
                if (m_uncheckedBehavior == CollapseUnchecked)
                    m_groupBoxes[i]->setCollapsed(false);
            }
            else
            {
                m_groupBoxes[i]->setChecked(false);
                if (m_uncheckedBehavior == DisableUnchecked)
                    m_setGroupEnabled(i,false);
                else if (m_uncheckedBehavior == CollapseUnchecked)
                    m_groupBoxes[i]->setCollapsed(true);
            }

            connect(m_groupBoxes[i],SIGNAL(clicked(bool)),m_checkMapper,SLOT(map()));
            m_checkMapper->setMapping(m_groupBoxes[i],m_groupNames[i]);
        }
        else if (behavior == MultipleChecked)
        {
            m_groupBoxes[i]->setCheckable(true);

            if (m_groupBoxes[i]->isChecked())
                m_setGroupEnabled(i,true);
            else if (m_uncheckedBehavior == DisableUnchecked)
                m_setGroupEnabled(i,false);
            else if (m_uncheckedBehavior == CollapseUnchecked)
                m_groupBoxes[i]->setCollapsed(true);


            connect(m_groupBoxes[i],SIGNAL(clicked(bool)),m_checkMapper,SLOT(map()));
            m_checkMapper->setMapping(m_groupBoxes[i],m_groupNames[i]);
        }
    }

    m_checkedBehavior = behavior;

}

void QGroupVector::m_groupChecked(QString groupName)
{
    int groupIndex = m_groupNames.indexOf(groupName);
    if (m_checkedBehavior == SingleChecked)
    {
        if (!m_groupBoxes[groupIndex]->isChecked())
        {
            m_groupBoxes[groupIndex]->blockSignals(true);
            m_groupBoxes[groupIndex]->setChecked(true);
            m_setGroupEnabled(groupIndex,true);
            if (m_uncheckedBehavior == CollapseUnchecked)
                m_groupBoxes[groupIndex]->setCollapsed(false);
            m_groupBoxes[groupIndex]->blockSignals(false);
        }
        else
        {
            m_setGroupEnabled(groupIndex,true);
            for (int i = 0; i < m_groupBoxes.count(); i ++)
                if (i != groupIndex)
                {
                    m_groupBoxes[i]->blockSignals(true);
                    if (m_uncheckedBehavior == DisableUnchecked)
                        m_setGroupEnabled(i,false);
                    else if (m_uncheckedBehavior == CollapseUnchecked)
                        m_groupBoxes[i]->setCollapsed(true);
                    m_groupBoxes[i]->setChecked(false);
                    m_groupBoxes[i]->blockSignals(false);
                }

        }
    }
    groupChecked(groupName,m_groupBoxes[groupIndex]->isChecked());
}


bool QGroupVector::isGroupChecked(QString groupName)
{
    int index = m_groupNames.indexOf(groupName);
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
