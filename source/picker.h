#ifndef PICKER_H
#define PICKER_H

#include <QVector2D>
#include <QMatrix4x4>
#include <QImage>
#include <QGLFramebufferObject>
#include "glhelper.h"

struct PickResult {
    qreal ObjectID;
    qreal DrawID;
    qreal PrimID;
};

// This class triggers a render pass that allows you to pick
// objects in 3D.
class Picker
{
public:
    Picker();
    ~Picker();

    void Init();
    void Size(unsigned int WindowWidth, unsigned int WindowHeight);

    void Start(QMatrix4x4 wvp);
    void End();

    int PickAt(unsigned int x, unsigned int y);

private:
    QGLShaderProgram * m_program;
    QImage m_pickingTexture;
    QGLFramebufferObject * m_pickingTarget;
    QGLFramebufferObjectFormat m_pickingFormat;
};

#endif // PICKER_H
