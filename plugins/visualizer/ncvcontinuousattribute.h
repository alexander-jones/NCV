#ifndef NCVCONTINUOUSATTRIBUTE_H
#define NCVCONTINUOUSATTRIBUTE_H
#include "ncvcore.h"
#include "qglx/qglxtexture1d.h"
#include "core/ncscontinuousattribute.h"
#include "core/gui/utilities/continuouscolorselector.h"

class NCVContinuousAttribute: public NCVAttribute
{
    Q_OBJECT
public:
    NCVContinuousAttribute( NCSContinuousAttribute * attrib);
    void attachColoration(QVector< QRgb> data);
    void attachColoration(QVector< QColor> data);
    QVector<float> data();
    QGLXBuffer attributeBuffer();
    QGLXTexture1D colorationTexture();
    float minValue();
    float maxValue();
    QVector<QColor> coloration();
    void destroy();
    bool dirty();

private slots:
    void m_parentChanged();

private:
    NCSContinuousAttribute * m_parent;
    bool m_dataDirty,m_colorationDirty;
    QVector<QColor> m_coloration;
    QVector<QVector3D > m_colorationData;
    QGLXBuffer m_buffer;
    QGLXTexture1D m_colorTexture;
};

#endif // NCVCONTINUOUSATTRIBUTE_H
