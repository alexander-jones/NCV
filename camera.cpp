#include "camera.h"
#include "math.h"
#include <QVector4D>
#define PI 3.141592653589793238462643383

Camera::Camera(void)
{
    viewMatrix = QMatrix4x4();
    projectionMatrix = QMatrix4x4();
    SetViewMatrix(QVector3D(0,0,0), 0.0, 0.0);
    SetProjectionMatrix(90.0, 4.0/3.0, 0.1, 10000.0);

}

// VIEW FUNCTIONS

QMatrix4x4 Camera::GetViewMatrix()
{
   return viewMatrix;
}

void Camera::SetViewMatrix(QVector3D pos, qreal horizAngle, qreal vertAngle)
{
   position = pos;
   horizontalAngle = horizAngle;
   verticalAngle = vertAngle;

   CompileViewMatrix();
}

void Camera::CompileViewMatrix()
{
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
    viewMatrix.setToIdentity();
    viewMatrix.lookAt(position,position + direction,up);

}

void Camera::SetViewMatrix(QVector3D pos, QVector3D target, QVector3D up)
{

   position = pos;
   direction = target - pos;
   this->up = up;
   viewMatrix.setToIdentity();
   viewMatrix.lookAt(position,target,up);
}

void Camera::Strafe(qreal horizontal, qreal forward, qreal vertical)
{
    position += horizontal * right ;
    position += forward * direction ;
    position += vertical * up ;
    CompileViewMatrix();
}

void Camera::Translate(QVector3D amt)
{
    position += amt;
    CompileViewMatrix();
}

QVector3D Camera::GetPosition()
{
    return position;
}
void Camera::SetPosition(QVector3D pos)
{

    position  = pos;
    CompileViewMatrix();
}

void Camera::Rotate(qreal horizRot, qreal vertRot)
{
    horizontalAngle += horizRot;
    verticalAngle += vertRot;
    CompileViewMatrix();
}

// PROJECTION FUNCTIONS

QMatrix4x4 Camera::GetProjectionMatrix()
{
   return projectionMatrix;
}
void Camera::SetProjectionMatrix(qreal fov, qreal aspect, qreal near, qreal far)
{
   fieldOfView = fov;
   aspectRatio = aspect;
   nearPlane = near;
   farPlane = far;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

qreal Camera::GetFieldOfView( )
{
    return fieldOfView;
}

void Camera::SetFieldOfView(qreal fov)
{
   fieldOfView = fov;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

qreal Camera::GetAspectRatio( )
{
   return aspectRatio;
}
void Camera::SetAspectRatio(qreal aspect)
{
   aspectRatio = aspect;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

qreal Camera::GetNearPlane( )
{
   return nearPlane;
}
void Camera::SetNearPlane(qreal near)
{
   nearPlane = near;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}

qreal Camera::GetFarPlane( )
{
   return farPlane;
}

void Camera::SetFarPlane(qreal far)
{
   farPlane = far;
   projectionMatrix.setToIdentity();
   projectionMatrix.perspective(fieldOfView,aspectRatio,nearPlane,farPlane);
}
