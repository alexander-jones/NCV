#ifndef NCVATTRIBUTE_H
#define NCVATTRIBUTE_H
#include "qglxcore.h"
#include "qglxbuffer.h"

class NCVAttribute
{
public:
    enum ElementType
    {
        Neuron,
        Connection
    };
    static ElementType other(ElementType elementType)
    {
        if (m_elementType ==Neuron)
            return Connection;
        else
            return Neuron;
    }

    NCVAttribute(ElementType elementType = Neuron)
    {
        m_elementType = elementType;
        m_buffer = NULL;
        m_dirty = false;
    }

    NCVAttribute(NCVAttribute * dependency,ElementType elementType = Neuron)
    {
        m_elementType = elementType;
        m_buffer = NULL;
        m_dirty = false;
    }
    void bind()
    {
        m_buffer->bind();
    }

    void release()
    {
        m_buffer->release();
    }

    ElementType elementType()
    {
        return m_elementType ;
    }

private:

    ElementType m_elementType;
    bool m_dirty;
    QGLXBuffer * m_buffer;
};

class NCVRangeAttribute:NCVAttribute
{
public:

    struct ColorRange
    {

        ColorRange()
        {
            width = 0;
            data = NULL;
        }
        ColorRange(int textureWidth, QRgb * textureData)
        {
            width = textureWidth;
            data = textureData;
        }

        int width;
        QRgb * data;
    };

    NCVRangeAttribute(ElementType elementType = Neuron, GLfloat minimum, GLfloat maximum)
        :NCVAttribute(elementType)
    {
        m_buffer = new QGLXBuffer();
        m_minimum = minimum;
        m_maximum = maximum;
        m_data = NULL;
        m_colorRange = ColorRange();
    }

    void attachData(GLfloat * data)
    {
        m_data = data;
        m_dirty = true;
    }
    void setColorRange(int width, QRgb * range)
    {
        m_colorRange = ColorRange(width,range);
    }
    ColorRange colorRange()
    {
        return m_colorRange;
    }


    GLfloat * unboundData()
    {
        return m_data;
    }

    GLfloat minimum()
    {
        return m_minimum;
    }
    GLfloat maximum()
    {
        return m_maximum;
    }

private:
    GLfloat m_minimum, m_maximum;
    GLfloat * m_data;
    ColorRange m_colorRange;
};

class NCVFlagAttribute
{
public:
    enum InferenceFunction
    {
        Or,
        Xor,
        Xnor,
        And
    };

    NCVFlagAttribute(GLubyte * data = NULL)
    {
        m_offColor = QColor(0,0,0);
        m_onColor = QColor(255,255,255);
        m_data = NULL;
    }

    void setData(GLubyte * data)
    {
        m_data = data;
    }
    GLubyte * data()
    {
        return m_data;
    }

    void setOffColor(QColor color)
    {
        m_offColor = color;
    }
    void setOnColor(QColor color)
    {
        m_onColor = color;
    }
    QColor offColor()
    {
        return m_offColor;
    }
    QColor onColor()
    {
        return m_onColor;
    }


private:
    QColor m_offColor, m_onColor;
    GLubyte * m_data;
};



#endif // NCVATTRIBUTE_H
