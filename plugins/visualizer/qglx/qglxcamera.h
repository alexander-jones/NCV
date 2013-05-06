#ifndef QGLXCAMERA_H
#define QGLXCAMERA_H
#include "qglxcore.h"
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include "math.h"



/*!
    \class QGLXCamera
    \author Alex Jones
    \brief An QGL extension class for manipulating view and projection in 3D.
*/
class QGLXCamera
{
public:
    /*!
        \brief A constructor. That is all.
    */
    QGLXCamera();

    /*!
        \brief This function returns the current aspect ratio of the projection.
    */
    GLfloat aspectRatio( );

    /*!
        \brief This function returns the current direction the camera is facing.
    */
    QVector3D forward();

    /*!
        \brief This function returns the camera's current up vector;
    */

    QVector3D up();

    /*!
        \brief This function returns the camera's current right vector;
    */

    QVector3D right();

    /*!
        \brief This function returns the current far plane of the projection.
    */
    GLfloat farPlane( );

    /*!
        \brief This function returns the current field of view of the projection.
    */
    GLfloat fieldOfView();

    /*!
        \brief This function returns the currently set aspectRatio.
    */
    QVector3D position();

    /*!
        \brief This function returns the current position of the camera.
    */
    GLfloat nearPlane( );

    /*!
        \brief This function returns the current projection matrix.
    */
    QMatrix4x4 projection();

    /*!
        \brief This function returns the camera's current rotation about the x axis.
    */
    GLfloat rotationX();

    /*!
        \brief This function returns the camera's current rotation about the y axis.
    */
    GLfloat rotationY();

    /*!
        \brief This function returns the camera's current view matrix.
    */
    QMatrix4x4 view();

    /*!
        \param horizAngle Angle about the x axis to rotate.
        \param vertAngle Angle about the y axis to rotate.
        \brief This function rotates the camera's current view.
    */
    void rotate(GLfloat horizAngle, GLfloat vertAngle);

    /*!
        \param aspect The aspect ratio to use.
        \brief This function changes the camera's current aspect ratio.
    */
    void setAspectRatio(GLfloat aspect);

    /*!
        \param far The far plane to use.
        \brief This function changes the camera's current far plane.
    */
    void setFarPlane(GLfloat far);

    /*!
        \param fov The field of view to use.
        \brief This function changes the camera's current field of view.
    */
    void setFieldOfView(GLfloat fov);

    /*!
        \param near The near plane to use.
        \brief This function changes the camera's current far plane.
    */
    void setNearPlane(GLfloat near);

    /*!
        \param pos The vector to use.
        \brief This function changes the camera's current position.
    */
    void setPosition(QVector3D pos);


    /*!
        \param fov The field of view to use.
        \param aspect The far plane to use.
        \param near The position to use.
        \param far The field of view to use.
        \brief This function set's the camera's current projection matrix.
    */
    void setProjection(GLfloat fov, GLfloat aspect, GLfloat near, GLfloat far);


    /*!
        \param dir The vector to use.
        \brief This function changes the camera's current forward vector.
    */
    void setForward(QVector3D dir);


    /*!
        \param up The vector to use.
        \brief This function changes the camera's current up vector.
    */
    void setUp(QVector3D up);
    /*!
        \param pos The position to use.
        \param horizAngle The angle about the x axis to rotate the camera.
        \param vertAngle  The angle about the y axis to rotate the camera.
        \brief This function set's the camera's current view matrix.
    */
    void setView(QVector3D pos, GLfloat horizAngle, GLfloat vertAngle);


    /*!
        \param pos The position to use.
        \param target The target position the camera will point at.
        \param up The up vector to use.
        \brief This function set's the camera's current view matrix.
    */
    void setView(QVector3D pos, QVector3D target, QVector3D up);

    /*!
        \param x The amount to translate along the x axis of the current view space.
        \param y The amount to translate along the y axis of the current view space.
        \param z The amount to translate along the z axis of the current view space.
        \brief This function translates the camera's position in view space.
    */
    void pan(GLfloat x,  GLfloat y, GLfloat z);

    /*!
        \param x The amount to translate along the x axis.
        \param y The amount to translate along the y axis.
        \param z The amount to translate along the z axis.
        \brief This function translates the camera's position in world space.
    */
    void translate(GLfloat x, GLfloat y , GLfloat z);


private:
    void m_compileViewMatrix();
    // projection info
    QMatrix4x4  m_projectionMatrix;
    GLfloat m_fieldOfView;
    GLfloat m_aspectRatio;
    GLfloat m_nearPlane;
    GLfloat m_farPlane;

    // view matrix info
    QMatrix4x4 m_viewMatrix;
    GLfloat m_verticalAngle, m_horizontalAngle;
    QVector3D m_position, m_direction, m_right, m_up;
};


#endif // CAMERA_H
