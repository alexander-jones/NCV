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

void QGLXCamera::setViewMatrix(QVector3D pos, GLfloat horizAngle, GLfloat vertAngle)
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

void QGLXCamera::pan(GLfloat x, GLfloat y, GLfloat z)
{
    position += x * right ;
    position += z * direction ;
    position += y * up ;
    compileViewMatrix();
}

void QGLXCamera::translate(GLfloat x, GLfloat y, GLfloat z)
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

GLfloat QGLXCamera::getRotationX()
{
    return verticalAngle;
}

GLfloat QGLXCamera::getRotationY()
{
    return horizontalAngle;
}


void QGLXCamera::rotate(GLfloat horizRot, GLfloat vertRot)
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
void QGLXCamera::setProjectionMatrix(GLfloat fov, GLfloat aspect, GLfloat nearP, GLfloat farP)
{
   fieldOfView = fov;
   aspectRatio = aspect;
   nearPlane = nearP;
   farPlane = farP;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

GLfloat QGLXCamera::getFieldOfView( )
{
    return fieldOfView;
}

void QGLXCamera::setFieldOfView(GLfloat fov)
{
   fieldOfView = fov;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

GLfloat QGLXCamera::getAspectRatio( )
{
   return aspectRatio;
}
void QGLXCamera::setAspectRatio(GLfloat aspect)
{
   aspectRatio = aspect;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

GLfloat QGLXCamera::getNearPlane( )
{
   return nearPlane;
}
void QGLXCamera::setNearPlane(GLfloat nearP)
{
   nearPlane = nearP;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

GLfloat QGLXCamera::getFarPlane( )
{
   return farPlane;
}

void QGLXCamera::setFarPlane(GLfloat farP)
{
   farPlane = farP;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}
