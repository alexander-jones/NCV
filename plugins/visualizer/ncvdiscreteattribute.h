#ifndef NCVDISCRETEATTRIBUTE_H
#define NCVDISCRETEATTRIBUTE_H
#include "qglx/qglxtexture2d.h"
#include "qglx/qglxbuffertexture.h"
#include "core/ncsdiscreteattribute.h"
#include "discretecolorselector.h"
#include "ncvattribute.h"

class NCVDiscreteAttribute:public NCVAttribute
{
    Q_OBJECT
public:
    NCVDiscreteAttribute(NCSDiscreteAttribute * parent);
    void attachColoration(QMap<QString,QColor> coloration);
    void attachColoration(QMap<QString,QRgb> coloration);
    QVector<unsigned char> data();
    QVector<QString> stateValues();
    QVector<QString> stateValues(int startElement, int count);
    QGLXBufferTexture attributeTexture();
    QGLXBufferTexture colorationTexture();
	bool dirty();
    QMap<QString,QColor> coloration();
    void destroy();
    int bitMask();
    int bitsPerValue();
    QVector<QString> states();

    void resolve();


private slots:
    void m_parentChanged();

private:
    NCSDiscreteAttribute * m_parent;
    bool m_dataDirty,m_colorationDirty;
    QMap<QString,QColor> m_coloration;
    QVector<QVector3D > m_colorationData;
    QGLXBuffer m_buffer, m_colorBuffer;
    QGLXBufferTexture m_bufferTexture,m_colorationTexture;
};


#endif // NCVDISCRETEATTRIBUTE_H
