#ifndef QGLXOCTREE_H
#define QGLXOCTREE_H
#include "qglxboundingbox.h"

template< class T>
struct QGLXOctreeLeaf
{
    QGLXOctreeLeaf(T object, QGLXBoundingBox bounds)
    {
        this->object = object;
        this->bounds = bounds;
    }

    T object;
    QGLXBoundingBox bounds;
};

template< class T>
class QGLXOctree: public QGLXBoundingBox
{
public:
    QGLXOctree()
    {
        for (int i = 0; i < 8; i ++)
            m_children[i] = NULL;
        m_levels = 0;
    }
    QGLXOctree(QVector3D center, QVector3D size ,int levels)
        :QGLXBoundingBox(center,size)
    {
        for (int i = 0; i < 8; i ++)
            m_children[i] = NULL;
        m_levels = levels;

    }

    ~QGLXOctree()
    {
        //if (m_children != NULL)
            //delete [] m_children;
    }

    int levels()
    {
        return m_levels;
    }

    void setLevels(int levels)
    {
        if (m_levels != levels && levels >= 0)
        {
            int oldLevel = m_levels;
            m_levels = levels;

            if (m_levels == 0)
            {
                if (m_levels < oldLevel)
                {
                    if (m_levels == 0)
                    {
                        for (int i =0; i < 8; i ++ )
                            if (m_children[i] != NULL)
                            {

                                m_children[i]->getContents(m_contents,true);
                                delete m_children[i];
                                m_children[i] = NULL;
                            }
                    }
                }
                else
                {
                    for (int i =0; i < m_contents.size(); i ++ )
                    {
                        QGLXOctreeLeaf<T>  item = m_contents[i];
                        m_contents.erase(m_contents.begin() + i);
                        insert(item.object,item.bounds);
                    }
                }
            }
            else
            {
                for (int i =0; i < 8; i ++ )
                    if (m_children[i] != NULL)
                        m_children[i]->setLevels(levels-1);
            }
        }

    }
    void insert(T object, QGLXBoundingBox bounds )
    {
        QVector3D childSize = size() / 2;
        if (m_levels >0 && (bounds.size().x() <= childSize.x() )&& (bounds.size().y() <= childSize.y() )&&( bounds.size().z() <= childSize.z()) )
        {
            // find closest child to object
            int childIndex = (bounds.center().x() < center().x() ? 0:4) + (bounds.center().y() < center().y() ? 0:2)+ (bounds.center().z() < center().z() ? 0:1);

            if (m_children[childIndex] == NULL)
                splitAt(childIndex);
            m_children[childIndex]->insert(object,bounds);
         }
        else
        {
            m_contents.push_back(QGLXOctreeLeaf<T>(object,bounds));
        }
    }

    void getContents(std::vector<T> & items,bool recursive = false)
    {
        for (int i = 0; i < m_contents.size(); i ++)
            items.push_back(m_contents[i].object);

        if (recursive)
            for (int i = 0; i < 8 ;i ++)
                if (m_children[i] != NULL)
                    m_children[i]->getContents(items,recursive);
    }

    void getContents(std::vector< QGLXOctreeLeaf<T> > & items,bool recursive = false)
    {
        for (int i = 0; i < m_contents.size(); i ++)
            items.push_back(m_contents[i]);

        if (recursive)
            for (int i = 0; i < 8 ;i ++)
                if (m_children[i] != NULL)
                    m_children[i]->getContents(items,recursive);
    }

    void remove(T object);

    void objectsWithinFrustum(std::vector<T> & items,QMatrix4x4 wvp)
    {

        QVector4D clip;

        for (int i =0; i <8; i ++)
        {
            if (m_children[i] != NULL)
            {
                clip = wvp * QVector4D(m_children[i]->m_center,1.0f);
                bool centerInFrustum = (abs(clip.x()) < clip.w() && abs(clip.y()) < clip.w() && 0 < clip.z() && clip.z() < clip.w());

                if (centerInFrustum)
                    m_children[i]->getContents(items,true);
                else
                    m_children[i]->objectsWithinFrustum(items,wvp);
            }

        }
        bool includeThis = false;
        if (includeThis)
            getContents(items,false);

    }

    void draw(QMatrix4x4 wvp)
    {
    }

    T* raycast(QVector3D direction);

private:
    void setContents(std::vector< QGLXOctreeLeaf<T> > & contents)
    {
        m_contents = contents;
    }
    void splitAt(int index)
    {
        QVector3D quarterSize = size() / 4;
        QVector3D childSize = size() / 2;

        if (index == LeftTopFront)
            m_children[LeftTopFront] = new QGLXOctree<T>(center() + QVector3D(-quarterSize.x(),-quarterSize.y(),-quarterSize.z()), childSize,m_levels -1);
        else if (index == LeftTopBack)
            m_children[LeftTopBack] = new QGLXOctree <T>(center() + QVector3D(-quarterSize.x(),-quarterSize.y(),quarterSize.z()), childSize,m_levels -1);
        else if (index == LeftBottomFront)
            m_children[LeftBottomFront] = new QGLXOctree <T>(center() + QVector3D(-quarterSize.x(),quarterSize.y(),-quarterSize.z()), childSize,m_levels -1);
        else if (index == LeftBottomBack)
            m_children[LeftBottomBack] = new QGLXOctree <T>(center() + QVector3D(-quarterSize.x(),quarterSize.y(),quarterSize.z()), childSize,m_levels -1);
        else if (index == RightTopFront)
            m_children[RightTopFront] = new QGLXOctree <T>(center() + QVector3D(quarterSize.x(),-quarterSize.y(),-quarterSize.z()), childSize,m_levels -1);
        else if (index == RightTopBack)
            m_children[RightTopBack] = new QGLXOctree<T> (center() + QVector3D(quarterSize.x(),-quarterSize.y(),quarterSize.z()), childSize,m_levels -1);
        else if (index == RightBottomFront)
            m_children[RightBottomFront] = new QGLXOctree <T>(center() + QVector3D(quarterSize.x(),quarterSize.y(),-quarterSize.z()), childSize,m_levels -1);
        else if (index == RightBottomBack)
            m_children[RightBottomBack] = new QGLXOctree<T>( center() + QVector3D(quarterSize.x(),quarterSize.y(),quarterSize.z()), childSize,m_levels -1);

        //m_children[index]->setContents(m_contents);
    }


    T* raycast(std::vector<T> & items,QVector3D direction);
    QGLXOctree<T> * m_children[8];
    std::vector< QGLXOctreeLeaf<T> > m_contents;
    int m_levels;
    float m_looseness;
    bool m_canSplit;
    int m_depth;
    int m_startIndex, m_contentSize;
};

#endif // QGLXOCTREE_H
