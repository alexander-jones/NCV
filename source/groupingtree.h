#ifndef GROUPINGTREE_H
#define GROUPINGTREE_H
#include "qglxsystem.h"

class GroupingTree
{
public:

    GroupingTree(QGLXSystem * system)
    {
        m_system =system;
        m_parent = NULL;
        m_startIndex = 0;
        m_size = 0;
        m_endIndex = 0;
    }

    void addChild(int startIndex, int size)
    {

        if (startIndex >= 0 && (startIndex + size) <= m_system->numObjects())
        {
            GroupingTree * node = new GroupingTree(m_system);
            node->m_startIndex = startIndex;
            node->m_size = size;
            node->m_endIndex = m_startIndex + m_size-1;

            if (node->m_startIndex < m_startIndex)
                m_startIndex = node->m_startIndex;
            if (node->m_endIndex > m_endIndex)
                m_endIndex = node->m_endIndex;

            addChild(node);
        }

    }


    QVector<GroupingTree*>  children()
    {
        return m_children;
    }


    friend bool operator== (const GroupingTree & first, const GroupingTree & second)
    {
        return first.m_system == second.m_system && first.m_startIndex == second.m_startIndex  && first.m_endIndex == second.m_endIndex && first.m_parent == second.m_parent;
    }

    friend bool operator!= (const GroupingTree & first, const GroupingTree & second)
    {
        return !(first == second);
    }

    void draw()
    {
        m_system->drawSubset(m_startIndex,m_size);
    }

private:


    bool contains(GroupingTree * node, bool recursive = false)
    {
        return m_children.contains(node);
        if (recursive)
            for (QVector<GroupingNode*>::iterator it = m_children.begin(); it != m_children.end();it++)
                if ((*it)->contains(node,recursive))
                    return true;
        return false;
    }

    bool addChild(GroupingTree * node)
    {
        if (node->m_startIndex >= m_startIndex && node->m_endIndex <= m_endIndex)
        {
            for (QVector<GroupingTree *>::iterator it = m_children.begin(); it != m_children.end(); )
            {
                GroupingTree * child  = *it;
                if (child->addChild(node))
                    return true;

                else if ( node->addChild(child) )
                {
                    child->m_parent = node;
                    m_children.erase(it++);
                }

                else if (node->m_endIndex <= child->m_startIndex)
                {
                    m_children.insert(it,node);
                    node->m_parent = child;
                    return true;
                }

                else
                    ++it;
            }

        }
        return false;
    }

    int m_startIndex,m_endIndex,m_size;
    QGLXSystem* m_system;
    GroupingTree * m_parent;
    QVector<GroupingTree*>  m_children;
};

#endif // GROUPINGTREE_H
