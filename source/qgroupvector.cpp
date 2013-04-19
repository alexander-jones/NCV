#include "qgroupvector.h"
#include <QDebug>
QGroupVector::QGroupVector(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_onlyEnableOnChecked = false;
    m_checkState = CheckNone;
    m_checkStateMapper = new QSignalMapper();
    connect(m_checkStateMapper,SIGNAL(mapped(QString)),this,SLOT(m_groupChecked(QString)));
    this->setLayout(m_layout);
}

void QGroupVector::addGroup(QString groupName)
{
    insertGroup(m_groupNames.count(),groupName);
}
void QGroupVector::insertGroup(int index, QString groupName )
{
    m_groupNames.insert(index,groupName);
    QBoxLayout * layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_groupLayouts.insert(index,layout);

    QGroupBox * groupBox = new QGroupBox(groupName);
    groupBox->setLayout(layout);
    m_groupBoxes.insert(index,groupBox);
    m_layout->insertWidget(index,groupBox);

    if (m_checkState != CheckNone)
    {
        m_groupBoxes[index]->setCheckable(true);
        connect(m_groupBoxes[index],SIGNAL(clicked(bool)),m_checkStateMapper,SLOT(map()));
        m_checkStateMapper->setMapping(m_groupBoxes[index],m_groupNames[index]);
    }
    if (m_checkState == CheckSingle)
    {
        bool boxAlreadyChecked = false;
        for (int i = 0; i < m_groupBoxes.count(); i ++)
        {
            if (!boxAlreadyChecked)
            {
                m_groupBoxes[i]->setChecked(true);
                if (m_onlyEnableOnChecked)
                    m_setGroupEnabled(i,true);
                boxAlreadyChecked = true;
            }
            else
            {
                m_groupBoxes[i]->setChecked(false);
                if (m_onlyEnableOnChecked)
                    m_setGroupEnabled(i,false);
            }
        }
    }
}
void QGroupVector::removeGroup(QString groupName)
{
    int index = m_groupNames.indexOf(groupName);
    if (index == -1)
        return;
    m_groupNames.remove(index);
    m_groupLayouts.remove(index);
    m_groupBoxes.remove(index);
}
bool QGroupVector::containsGroup(QString name)
{
    return m_groupNames.contains(name);
}
void QGroupVector::addToGroup(QString groupName,QWidget * widget)
{
    int index = m_groupNames.indexOf(groupName);
    if (index == -1)
        return;
    m_groupLayouts[index]->addWidget(widget);

}
void QGroupVector::insertInGroup(QString groupName,int index, QWidget * widget)
{
    m_groupLayouts[index]->insertWidget(index,widget);
}
void QGroupVector::removeFromGroup(QString groupName, QWidget * widget)
{
    int index = m_groupNames.indexOf(groupName);
    if (index == -1)
        return;
    m_groupLayouts[index]->removeWidget(widget);
}
bool QGroupVector::groupContainsWidget(QString groupName,QWidget * widget)
{
    int index = m_groupNames.indexOf(groupName);
    if (index == -1)
        return false;
    return m_groupLayouts[index]->indexOf(widget) != -1;
}
void QGroupVector::setDirection(Direction direction)
{
    m_layout->setDirection((QBoxLayout::Direction)direction);
}
void QGroupVector::setAlignment(Qt::Alignment alignment)
{
    m_layout->setAlignment(alignment);
}

void QGroupVector::setGroupAlignment(QString groupName,Qt::Alignment alignment)
{
    int index = m_groupNames.indexOf(groupName);
    if (index == -1)
        return;
    m_groupLayouts[index]->setAlignment(alignment);
}
void QGroupVector::setGroupDirection(QString groupName,Direction direction)
{
    int index = m_groupNames.indexOf(groupName);
    if (index == -1)
        return;
    m_groupLayouts[index]->setDirection((QBoxLayout::Direction)direction);
}
void QGroupVector::setGroupChecked(QString groupName,bool checked)
{
    int index = m_groupNames.indexOf(groupName);
    if (index == -1)
        return;
    m_groupBoxes[index]->setChecked(checked);
}
QGroupVector::CheckState QGroupVector::checkState()
{
    return m_checkState;
}

void QGroupVector::setCheckState(CheckState state)
{
    bool boxAlreadyChecked = false;

    for (int i = 0; i < m_groupBoxes.count(); i ++)
    {
        if (state == CheckNone)
        {
            if (m_checkState != CheckNone)
                disconnect(m_groupBoxes[i],SIGNAL(clicked(bool)),m_checkStateMapper,SLOT(map()));
            m_groupBoxes[i]->setCheckable(false);
        }
        else if (state == CheckSingle)
        {
            m_groupBoxes[i]->setCheckable(true);
            if (!boxAlreadyChecked)
            {
                m_groupBoxes[i]->setChecked(true);
                boxAlreadyChecked = true;
                if (m_onlyEnableOnChecked)
                    m_setGroupEnabled(i,true);
            }
            else
            {
                m_groupBoxes[i]->setChecked(false);
                if (m_onlyEnableOnChecked)
                    m_setGroupEnabled(i,false);
            }

            connect(m_groupBoxes[i],SIGNAL(clicked(bool)),m_checkStateMapper,SLOT(map()));
            m_checkStateMapper->setMapping(m_groupBoxes[i],m_groupNames[i]);
        }
        else if (state == CheckMultiple)
        {
            m_groupBoxes[i]->setCheckable(true);

            if (m_onlyEnableOnChecked)
            {
                if (m_groupBoxes[i]->isChecked())
                    m_setGroupEnabled(i,true);
                else
                    m_setGroupEnabled(i,false);
            }

            connect(m_groupBoxes[i],SIGNAL(clicked(bool)),m_checkStateMapper,SLOT(map()));
            m_checkStateMapper->setMapping(m_groupBoxes[i],m_groupNames[i]);
        }
    }

    m_checkState = state;
}

void QGroupVector::m_groupChecked(QString groupName)
{
    int groupIndex = m_groupNames.indexOf(groupName);
    if (m_checkState == CheckSingle)
    {
        if (!m_groupBoxes[groupIndex]->isChecked())
        {
            m_groupBoxes[groupIndex]->blockSignals(true);
            m_groupBoxes[groupIndex]->setChecked(true);
            if (m_onlyEnableOnChecked)
                m_setGroupEnabled(groupIndex,true);
            m_groupBoxes[groupIndex]->blockSignals(false);
        }
        else
        {
            if (m_onlyEnableOnChecked)
                m_setGroupEnabled(groupIndex,true);
            for (int i = 0; i < m_groupBoxes.count(); i ++)
                if (i != groupIndex)
                {
                    m_groupBoxes[i]->blockSignals(true);
                    if (m_onlyEnableOnChecked)
                        m_setGroupEnabled(i,false);
                    m_groupBoxes[i]->setChecked(false);
                    m_groupBoxes[i]->blockSignals(false);
                }

        }
    }
    groupChecked(groupName,m_groupBoxes[groupIndex]->isChecked());
}

void QGroupVector::m_setGroupEnabled(int index, bool set)
{
    for (int i = 0; i < m_groupLayouts[index]->count(); i ++)
        m_groupLayouts[index]->itemAt(i)->widget()->setEnabled(set);
}

void QGroupVector::setEnableOnlyChecked(bool set)
{
    m_onlyEnableOnChecked = set;
}

bool QGroupVector::onlyCheckedEnabled()
{
    return m_onlyEnableOnChecked;
}

bool QGroupVector::isGroupChecked(QString groupName)
{
    int index = m_groupNames.indexOf(groupName);
    return m_groupBoxes[index]->isChecked();
}
