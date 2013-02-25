#ifndef NCVCONTINUOUSATTRIBUTE_H
#define NCVCONTINUOUSATTRIBUTE_H
#include "ncvcore.h"
#include "qglxtexture1d.h"

class NCVContinuousAttribute: public NCVAttribute
{
public:
    NCVContinuousAttribute( NCVElementType type, GLfloat minValue , GLfloat maxValue);
    void attachData(QVector<GLfloat> data);
    void attachColoration(QVector< QRgb> data);
    void attachColoration(QVector< QColor> data);
    QVector<GLfloat> data();
    QGLShaderProgram * program();
    void bind(QGLXCamera camera);
    void release();
    NCVElementType elementType();
    GLfloat minValue();
    GLfloat maxValue();
    QVector<QColor> coloration();
    NCVAttributeType type(){return Continuous;}
	QMap<QString,NCVAttribute *> attributes();
	bool dirty();
    void resolve();

private:
    NCVElementType m_type;
    QGLShaderProgram m_program;
    GLfloat m_minValue, m_maxValue;
    bool m_dataDirty,m_colorationDirty, m_shaderDirty,m_renderedNewData;
    QVector<GLfloat> m_data;
    QVector<QColor> m_coloration;
    QVector<QVector3D > m_colorationData;
    QGLXBuffer m_buffer;
    QGLXTexture1D m_colorTexture;
};

#endif // NCVCONTINUOUSATTRIBUTE_H
