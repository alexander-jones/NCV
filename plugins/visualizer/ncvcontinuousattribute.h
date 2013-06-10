#ifndef NCVCONTINUOUSATTRIBUTE_H
#define NCVCONTINUOUSATTRIBUTE_H
#include "qglx/qglxtexture1d.h"
#include "qglx/qglxbuffertexture.h"
#include "ncvattribute.h"
#include "core/ncscontinuousattribute.h"
#include "gui/utilities/continuouscolorselector.h"

class NCVContinuousAttribute: public NCVAttribute
{
    Q_OBJECT
public:
    NCVContinuousAttribute( NCSContinuousAttribute * attrib);
    void attachColoration(QVector< QRgb> data);
    void attachColoration(QVector< QColor> data);
    QVector<float> data();
    QGLXBufferTexture attributeTexture();
    QGLXTexture1D colorationTexture();
    float minValue();
    float maxValue();
    QVector<QColor> coloration();
    void destroy();
    bool dirty();

    void resolve();

private slots:
    void m_parentChanged();

private:
    NCSContinuousAttribute * m_parent;
    bool m_dataDirty,m_colorationDirty;
    QVector<QColor> m_coloration;
    QVector<QVector3D > m_colorationData;
    QGLXBuffer m_buffer;
    QGLXTexture1D m_colorTexture;
    QGLXBufferTexture m_bufferTexture;
};

#endif // NCVCONTINUOUSATTRIBUTE_H
