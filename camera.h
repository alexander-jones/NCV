#include <QMatrix4x4>
#include <QVector3D>
class Camera
{
public:
    Camera(void);

    // view functions
    QMatrix4x4 GetViewMatrix();
    void SetViewMatrix(QVector3D pos, qreal horizAngle, qreal vertAngle);
    void SetViewMatrix(QVector3D pos, QVector3D target, QVector3D up);

    QVector3D GetPosition();
    void SetPosition(QVector3D pos);

    void Rotate(qreal horizAngle, qreal vertAngle);
    void Translate(QVector3D amt);
    void Strafe(qreal horizontal, qreal forward, qreal vertical);

    // projection functions
    QMatrix4x4 GetProjectionMatrix();
    void SetProjectionMatrix(qreal fov, qreal aspect, qreal near, qreal far);

    qreal GetFieldOfView();
    void SetFieldOfView(qreal fov);

    qreal GetAspectRatio( );
    void SetAspectRatio(qreal aspect);

    qreal GetNearPlane( );
    void SetNearPlane(qreal near);

    qreal GetFarPlane( );
    void SetFarPlane(qreal far);

private:
    void CompileViewMatrix();
    // projection info
    QMatrix4x4  projectionMatrix;
    qreal fieldOfView;
    qreal aspectRatio;
    qreal nearPlane;
    qreal farPlane;

    // view matrix info
    QMatrix4x4 viewMatrix;
    qreal verticalAngle, horizontalAngle;
    QVector3D position, direction, right, up;
};

