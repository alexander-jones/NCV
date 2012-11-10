#ifndef QGLXBOUNDINGBOX_H
#define QGLXBOUNDINGBOX_H
#include <QVector3D>
#include <QMatrix4x4>

const int LeftTopFront= 0;
const int LeftTopBack= 1;
const int LeftBottomFront = 2;
const int LeftBottomBack = 3;
const int RightTopFront= 4;
const int RightTopBack = 5;
const int RightBottomFront= 6;
const int RightBottomBack= 7;


/*!
    \class QGLXBoundingBox
    \author Alex Jones
    \brief An QGL extension class for testing 3D bounding conditions.
*/
class QGLXBoundingBox
{
public:
    /*!
        \param position The center position of the box.
        \param size The size of the box.
        \brief The constructor.
    */
    QGLXBoundingBox(QVector3D position = QVector3D(0,0,0), QVector3D size = QVector3D(0,0,0));

    /*!
        \brief This function returns the bottom of the bounding box;
    */
    qreal bottom();

    /*!
        \brief This function returns the center position of the bounding box;
    */
    QVector3D center();

    /*!
        \param other The other bounding box to test against.
        \brief This function checks to see if this bounding box fully contains another.
        The test is inclusive when testing against boundries.
    */
    bool contains(QGLXBoundingBox other);

    /*!
        \param point A point in space to test against.
        \brief This function checks to see if this bounding box contains a point.
    */
    bool contains(QVector3D point);

    /*!0:4
        \brief This function returns the corner positions of the bounding box.
    */
    QVector3D * corners();

    /*!
        \brief This function returns the depth of the bounding box;
    */
    qreal depth();

    /*!
        \brief This function returns the Back of the bounding box;
    */
    qreal back();

    /*!
        \brief This function returns the height of the bounding box;
    */
    qreal height();

    /*!
        \param other The other bounding box to test against.
        \brief This function checks to see if thisFar bounding box intersects another.
        The test is inclusive when testing against boundries.
    */
    bool intersects(QGLXBoundingBox other);

    /*!
        \brief This function returns the left of the bounding box;
    */
    qreal left();

    /*!
        \brief This function returns the Front of the bounding box;
    */
    qreal front();

    /*!
        \brief This function returns the right of the bounding box;
    */
    qreal right();

    /*!
        \brief This function returns the size of the bounding box;
    */
    QVector3D size();

    /*!
        \brief This function returns the top of the bounding box;
    */
    qreal top();

    /*!
        \brief This function returns the width of the bounding box;
    */
    qreal width();


protected:
    QVector3D m_size;
    QVector3D m_center;
    QVector3D m_corners[8];
    qreal m_left, m_right, m_top , m_bottom, m_front, m_back;
};

#endif // QGLXBOUNDINGBOX_H
