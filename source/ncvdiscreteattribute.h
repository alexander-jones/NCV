#ifndef NCVDISCRETEATTRIBUTE_H
#define NCVDISCRETEATTRIBUTE_H
#include "ncvcore.h"
#include "qglxtexture2d.h"

class NCVDiscreteAttribute:public NCVAttribute
{
public:
    NCVDiscreteAttribute(NCVElementType type, QVector<QString> states);
    void attachData(QVector<GLubyte> data, int bitsPerValue = 8);
    void attachColoration(QMap<QString,QColor> coloration);
    void attachColoration(QMap<QString,QRgb> coloration);
    QVector<GLubyte> data();
    QVector<QString> stateValues();
    QVector<QString> stateValues(int startElement, int count);
    QGLShaderProgram * program();
    void bind(QGLXCamera camera);
	void release();
	bool dirty();
    NCVElementType elementType();
    QMap<QString,QColor> coloration();
    NCVAttributeType type(){return Discrete;}
    void resolve();

private:

    NCVElementType m_type;
    QGLShaderProgram m_program;
    bool m_dataDirty,m_colorationDirty, m_shaderDirty,m_renderedNewData;
    int m_bitsPerValue, m_bitsVal;
    QVector<GLubyte> m_data;
    QVector<QString> m_states;
    QMap<QString,QColor> m_coloration;
    QVector<QVector3D > m_colorationData;
    QGLXBuffer m_buffer, m_colorBuffer;
};


#endif // NCVDISCRETEATTRIBUTE_H
