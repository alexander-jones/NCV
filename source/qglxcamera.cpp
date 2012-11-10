#include "qglxcamera.h"

QGLXCamera::QGLXCamera(void)
{
    viewMatrix = QMatrix4x4();
    projectionMatrix = QMatrix4x4();

}

void QGLXCamera::compileViewMatrix()
{
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(position,position + direction,up);
}

// VIEW FUNCTIONS

QMatrix4x4 QGLXCamera::getViewMatrix()
{
   return viewMatrix;
}

void QGLXCamera::setViewMatrix(QVector3D pos, qreal horizAngle, qreal vertAngle)
{
   position = pos;
   horizontalAngle = horizAngle;
   verticalAngle = vertAngle;
   direction = QVector3D(
       cos(verticalAngle) * sin(horizontalAngle),
       sin(verticalAngle),
       cos(verticalAngle) * cos(horizontalAngle)
   );

   right = QVector3D(
       sin(horizontalAngle - PI/2.0),
       0,
       cos(horizontalAngle - PI/2.0)
   );
   up = QVector3D::crossProduct( right, direction );

   compileViewMatrix();
}

void QGLXCamera::setViewMatrix(QVector3D pos, QVector3D target, QVector3D up)
{

   position = pos;
   direction = target - pos;
   this->up = up;

   verticalAngle = asin(direction.y());
   horizontalAngle = acos(direction.z() / cos(verticalAngle));

   compileViewMatrix();
}

void QGLXCamera::pan(qreal x, qreal y, qreal z)
{
    position += x * right ;
    position += z * direction ;
    position += y * up ;
    compileViewMatrix();
}

void QGLXCamera::translate(qreal x, qreal y, qreal z)
{
    position += QVector3D(x,y,z);
    compileViewMatrix();
}

QVector3D QGLXCamera::getPosition()
{
    return position;
}
void QGLXCamera::setPosition(QVector3D pos)
{

    position  = pos;
    compileViewMatrix();
}


QVector3D QGLXCamera::getForward()
{
    return direction;
}
QVector3D QGLXCamera::getUp()
{
    return up;
}

QVector3D QGLXCamera::getRight()
{
    return right;
}

qreal QGLXCamera::getRotationX()
{
    return verticalAngle;
}

qreal QGLXCamera::getRotationY()
{
    return horizontalAngle;
}


void QGLXCamera::rotate(qreal horizRot, qreal vertRot)
{
    horizontalAngle += horizRot;
    verticalAngle += vertRot;

    direction = QVector3D(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    right = QVector3D(
        sin(horizontalAngle - PI/2.0),
        0,
        cos(horizontalAngle - PI/2.0)
    );
    up = QVector3D::crossProduct( right, direction );

    compileViewMatrix();
}

// PROJECTION FUNCTIONS
QMatrix4x4 QGLXCamera::getProjectionMatrix()
{
   return projectionMatrix;
}
void QGLXCamera::setProjectionMatrix(qreal fov, qreal aspect, qreal near, qreal far)
{
   fieldOfView = fov;
   aspectRatio = aspect;
   nearPlane = near;
   farPlane = far;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

qreal QGLXCamera::getFieldOfView( )
{
    return fieldOfView;
}

void QGLXCamera::setFieldOfView(qreal fov)
{
   fieldOfView = fov;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

qreal QGLXCamera::getAspectRatio( )
{
   return aspectRatio;
}
void QGLXCamera::setAspectRatio(qreal aspect)
{
   aspectRatio = aspect;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

qreal QGLXCamera::getNearPlane( )
{
   return nearPlane;
}
void QGLXCamera::setNearPlane(qreal near)
{
   nearPlane = near;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

qreal QGLXCamera::getFarPlane( )
{
   return farPlane;
}

void QGLXCamera::setFarPlane(qreal far)
{
   farPlane = far;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}
