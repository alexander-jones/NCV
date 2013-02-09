#include "qglxboundingbox.h"




QGLXBoundingBox::QGLXBoundingBox(QVector3D position, QVector3D size )
{
    m_center = position;
    m_size = size;

    QVector3D halfSize = size / 2;
    m_left = m_center.x() - halfSize.x();
    m_right = m_center.x() + halfSize.x();
    m_top = m_center.x() - halfSize.y();
    m_bottom = m_center.x() + halfSize.y();
    m_front = m_center.z() - halfSize.z();
    m_back = m_center.z() + halfSize.z();

    m_corners[LeftTopFront]= QVector3D(m_left,m_top,m_front);
    m_corners[LeftTopBack]= QVector3D(m_left,m_top,m_back);
    m_corners[LeftBottomFront]= QVector3D(m_left,m_bottom,m_front);
    m_corners[LeftBottomBack]= QVector3D(m_left,m_bottom,m_back);
    m_corners[RightTopFront]= QVector3D(m_right,m_top,m_front);
    m_corners[RightTopBack]= QVector3D(m_right,m_top,m_back);
    m_corners[RightBottomFront]= QVector3D(m_right,m_bottom,m_front);
    m_corners[RightBottomBack]= QVector3D(m_right,m_bottom,m_back);
}

bool QGLXBoundingBox::intersects(QGLXBoundingBox other)
{
    if (m_bottom >= other.top() && m_bottom <= other.bottom() && m_left >= other.left() && m_left <= other.right() && m_front >= other.front() && m_front <=other.back())
        return true;

    else if (m_bottom >= other.top() && m_bottom <= other.bottom() && m_left >= other.left() && m_left <= other.right() && m_back >= other.front() && m_back <=other.back())
        return true;

    else if (m_bottom >= other.top() && m_bottom <= other.bottom() && m_right >= other.left() && m_right <= other.right() && m_front >= other.front() && m_front <=other.back())
        return true;

    else if (m_bottom >= other.top() && m_bottom <= other.bottom() && m_right >= other.left() && m_right <= other.right() && m_back >= other.front() && m_back <=other.back())
        return true;

    else if (m_top >= other.top() && m_top <= other.bottom() && m_left >= other.left() && m_left <= other.right() && m_front >= other.front() && m_front <=other.back())
        return true;

    else if (m_top >= other.top() && m_top <= other.bottom() && m_left >= other.left() && m_left <= other.right() && m_back >= other.front() && m_back <=other.back())
        return true;

    else if (m_top >= other.top() && m_top <= other.bottom() && m_right >= other.left() && m_right <= other.right() && m_front >= other.front() && m_front <=other.back())
        return true;

    else if (m_top >= other.top() && m_top <= other.bottom() && m_right >= other.left() && m_right <= other.right() && m_back >= other.front() && m_back <=other.back())
        return true;
    else
        return false;
}

bool QGLXBoundingBox::contains(QGLXBoundingBox other)
{
    return m_top <= other.top() && m_bottom >= other.bottom() && m_left <= other.left() && m_right >= other.right() && m_front <= other.front() && m_back >= other.back();
}
bool QGLXBoundingBox::contains(QVector3D point)
{
    return point.y() >= m_top && point.y() <= m_bottom && point.x() >= m_left && point.x() <= m_right && point.z() >= m_front && point.z() < m_back;
}

qreal QGLXBoundingBox::top()
{
    return m_top;
}

qreal QGLXBoundingBox::bottom()
{
    return m_bottom;
}
qreal QGLXBoundingBox::left()
{
    return m_left;
}
qreal QGLXBoundingBox::right()
{
    return m_right;
}
qreal QGLXBoundingBox::front()
{
    return m_front;
}
qreal QGLXBoundingBox::back()
{
    return m_back;
}

qreal QGLXBoundingBox::width()
{
    return m_size.x();
}
qreal QGLXBoundingBox::height()
{
    return m_size.y();
}
qreal QGLXBoundingBox::depth()
{
    return m_size.z();
}

QVector3D QGLXBoundingBox::size()
{
    return m_size;
}

QVector3D QGLXBoundingBox::center()
{
    return m_center;
}

QVector3D * QGLXBoundingBox::corners()
{
    return m_corners;
}
