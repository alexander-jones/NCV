#include "qglxcamera.h"

QGLXCamera::QGLXCamera(void)
{
    m_viewMatrix = QMatrix4x4();
    m_projectionMatrix = QMatrix4x4();

}

void QGLXCamera::m_compileViewMatrix()
{
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt(m_position,m_position + m_direction,m_up);
}

// VIEW FUNCTIONS

QMatrix4x4 QGLXCamera::view()
{
   return m_viewMatrix;
}

void QGLXCamera::setView(QVector3D pos, GLfloat horizAngle, GLfloat vertAngle)
{
   m_position = pos;
   m_horizontalAngle = horizAngle;
   m_verticalAngle = vertAngle;
   m_direction = QVector3D(
       cos(m_verticalAngle) * sin(m_horizontalAngle),
       sin(m_verticalAngle),
       cos(m_verticalAngle) * cos(m_horizontalAngle)
   ).normalized();

   m_right = QVector3D(
       sin(m_horizontalAngle - PI/2.0),
       0,
       cos(m_horizontalAngle - PI/2.0)
   ).normalized();
   m_up = QVector3D::crossProduct( m_right, m_direction ).normalized();

   m_compileViewMatrix();
}

void QGLXCamera::setView(QVector3D pos, QVector3D target, QVector3D m_up)
{

   m_position = pos;

   m_direction = (target - pos).normalized();
   this->m_up = m_up.normalized();


   m_verticalAngle = asin(m_direction.y());
   m_horizontalAngle = acos(m_direction.z() / cos(m_verticalAngle));

   m_right = QVector3D(
       sin(m_horizontalAngle - PI/2.0),
       0,
       cos(m_horizontalAngle - PI/2.0)
   ).normalized();
   m_compileViewMatrix();
}

void QGLXCamera::setForward(QVector3D dir)
{
    m_direction = dir.normalized();

    m_verticalAngle = asin(m_direction.y());
    m_horizontalAngle = acos(m_direction.z() / cos(m_verticalAngle));

    m_right = QVector3D(
        sin(m_horizontalAngle - PI/2.0),
        0,
        cos(m_horizontalAngle - PI/2.0)
    ).normalized();
    m_compileViewMatrix();
}


void QGLXCamera::setUp(QVector3D up)
{
    m_up = up.normalized();
    m_compileViewMatrix();
}


void QGLXCamera::pan(GLfloat x, GLfloat y, GLfloat z)
{
    m_position += x * m_right ;
    m_position += z * m_direction ;
    m_position += y * m_up ;
    m_compileViewMatrix();
}

void QGLXCamera::translate(GLfloat x, GLfloat y, GLfloat z)
{
    m_position += QVector3D(x,y,z);
    m_compileViewMatrix();
}

QVector3D QGLXCamera::position()
{
    return m_position;
}
void QGLXCamera::setPosition(QVector3D pos)
{

    m_position  = pos;
    m_compileViewMatrix();
}

QVector3D QGLXCamera::forward()
{
    return m_direction;
}
QVector3D QGLXCamera::up()
{
    return m_up;
}

QVector3D QGLXCamera::right()
{
    return m_right;
}

GLfloat QGLXCamera::rotationX()
{
    return m_verticalAngle;
}

GLfloat QGLXCamera::rotationY()
{
    return m_horizontalAngle;
}


void QGLXCamera::rotate(GLfloat horizRot, GLfloat vertRot)
{
    m_horizontalAngle += horizRot;
    m_verticalAngle += vertRot;

    m_direction = QVector3D(
        cos(m_verticalAngle) * sin(m_horizontalAngle),
        sin(m_verticalAngle),
        cos(m_verticalAngle) * cos(m_horizontalAngle)
    ).normalized();

    m_right = QVector3D(
        sin(m_horizontalAngle - PI/2.0),
        0,
        cos(m_horizontalAngle - PI/2.0)
    ).normalized();
    m_up = QVector3D::crossProduct( m_right, m_direction ).normalized();

    m_compileViewMatrix();
}

// PROJECTION FUNCTIONS
QMatrix4x4 QGLXCamera::projection()
{
   return m_projectionMatrix;
}
void QGLXCamera::setProjection(GLfloat fov, GLfloat aspect, GLfloat nearP, GLfloat farP)
{
   m_fieldOfView = fov;
   m_aspectRatio = aspect;
   m_nearPlane = nearP;
   m_farPlane = farP;
   m_projectionMatrix.setToIdentity();
   m_projectionMatrix.perspective(m_fieldOfView,m_aspectRatio,m_nearPlane,m_farPlane);
}

GLfloat QGLXCamera::fieldOfView()
{
    return m_fieldOfView;
}

void QGLXCamera::setFieldOfView(GLfloat fov)
{
   m_fieldOfView = fov;
   m_projectionMatrix.setToIdentity();
   m_projectionMatrix.perspective(m_fieldOfView,m_aspectRatio,m_nearPlane,m_farPlane);
}

GLfloat QGLXCamera::aspectRatio( )
{
   return m_aspectRatio;
}
void QGLXCamera::setAspectRatio(GLfloat aspect)
{
   m_aspectRatio = aspect;
   m_projectionMatrix.setToIdentity();
   m_projectionMatrix.perspective(m_fieldOfView,m_aspectRatio,m_nearPlane,m_farPlane);
}

GLfloat QGLXCamera::nearPlane( )
{
   return m_nearPlane;
}
void QGLXCamera::setNearPlane(GLfloat nearP)
{
   m_nearPlane = nearP;
   m_projectionMatrix.setToIdentity();
   m_projectionMatrix.perspective(m_fieldOfView,m_aspectRatio,m_nearPlane,m_farPlane);
}

GLfloat QGLXCamera::farPlane( )
{
   return m_farPlane;
}

void QGLXCamera::setFarPlane(GLfloat farP)
{
   m_farPlane = farP;
   m_projectionMatrix.setToIdentity();
   m_projectionMatrix.perspective(m_fieldOfView,m_aspectRatio,m_nearPlane,m_farPlane);
}
