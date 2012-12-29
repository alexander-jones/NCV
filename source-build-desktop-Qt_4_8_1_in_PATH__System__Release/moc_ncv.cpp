/****************************************************************************
** Meta object code from reading C++ file 'ncv.h'
**
** Created: Fri Dec 28 16:48:18 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../source/ncv.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ncv.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NCV[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      34,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: signature, parameters, type, tag, flags
      21,    5,    4,    4, 0x05,
      47,   42,    4,    4, 0x05,
      85,   73,    4,    4, 0x05,
     116,   42,    4,    4, 0x05,
     139,   42,    4,    4, 0x05,
     173,  162,    4,    4, 0x05,
     202,  162,    4,    4, 0x05,
     235,    4,    4,    4, 0x05,
     293,  274,    4,    4, 0x05,
     360,  338,    4,    4, 0x05,
     405,  274,    4,    4, 0x05,
     454,  338,    4,    4, 0x05,

 // slots: signature, parameters, type, tag, flags
     531,  503,    4,    4, 0x0a,
     579,  570,    4,    4, 0x0a,
     623,  606,    4,    4, 0x0a,
     653,  570,    4,    4, 0x0a,
     676,   73,    4,    4, 0x0a,
     707,   42,    4,    4, 0x0a,
     729,   42,    4,    4, 0x0a,
     751,  162,    4,    4, 0x0a,
     780,   42,    4,    4, 0x0a,
     801,    4,    4,    4, 0x0a,
     861,  828,    4,    4, 0x0a,
     970,  942,    4,    4, 0x2a,
    1060, 1028,    4,    4, 0x0a,
    1170, 1143,    4,    4, 0x2a,
    1231,  942,    4,    4, 0x0a,
    1293, 1143,    4,    4, 0x0a,
    1363, 1358,    4,    4, 0x0a,
    1422, 1405,    4,    4, 0x0a,
    1471, 1358,    4,    4, 0x0a,
    1517, 1405,    4,    4, 0x0a,
    1570,   42,    4,    4, 0x0a,
    1606,   42,    4,    4, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NCV[] = {
    "NCV\0\0framesPerSecond\0newFPSReading(float)\0"
    "name\0newCurrentCamera(QString)\0"
    "camera,name\0newCamera(QGLXCamera*,QString)\0"
    "cameraDeleted(QString)\0cameraUpdated(QString)\0"
    "light,name\0newLight(QGLXLight*,QString)\0"
    "lightDeleted(QGLXLight*,QString)\0"
    "invalidGraphicsConfigurationDetected()\0"
    "name,minVal,maxVal\0"
    "newNeuronRangeAttribute(QString,float,float)\0"
    "name,offColor,onColor\0"
    "newNeuronBitAttribute(QString,QColor,QColor)\0"
    "newConnectionRangeAttribute(QString,float,float)\0"
    "newConnectionBitAttribute(QString,QColor,QColor)\0"
    "number,inNeurons,outNeurons\0"
    "createConnections(int,GLuint*,GLuint*)\0"
    "filename\0createConnections(QString)\0"
    "number,positions\0createNeurons(int,QVector3D*)\0"
    "createNeurons(QString)\0"
    "addCamera(QGLXCamera*,QString)\0"
    "switchCamera(QString)\0deleteCamera(QString)\0"
    "addLight(QGLXLight*,QString)\0"
    "deleteLight(QString)\0setMaterial(QGLXMaterial*)\0"
    "name,data,minValue,maxValue,func\0"
    "setNeuronRangeAttribute(QString,GLfloat*,GLfloat,GLfloat,RangeInferenc"
    "eFunction)\0"
    "name,data,minValue,maxValue\0"
    "setNeuronRangeAttribute(QString,GLfloat*,GLfloat,GLfloat)\0"
    "name,data,onColor,offColor,func\0"
    "setNeuronFlagAttribute(QString,GLubyte*,QVector3D,QVector3D,FlagInfere"
    "nceFunction)\0"
    "name,data,onColor,offColor\0"
    "setNeuronFlagAttribute(QString,GLubyte*,QVector3D,QVector3D)\0"
    "setConnectionRangeAttribute(QString,GLfloat*,GLfloat,GLfloat)\0"
    "setConnectionFlagAttribute(QString,GLubyte*,QVector3D,QVector3D)\0"
    "data\0changeCurrentNeuronRangeColoration(QRgb*)\0"
    "offColor,onColor\0"
    "changeCurrentNeuronFlagColoration(QColor,QColor)\0"
    "changeCurrentConnectionRangeColoration(QRgb*)\0"
    "changeCurrentConnectionFlagColoration(QColor,QColor)\0"
    "setNeuronAttributeToRender(QString)\0"
    "setConnectionAttributeToRender(QString)\0"
};

void NCV::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NCV *_t = static_cast<NCV *>(_o);
        switch (_id) {
        case 0: _t->newFPSReading((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 1: _t->newCurrentCamera((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->newCamera((*reinterpret_cast< QGLXCamera*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: _t->cameraDeleted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->cameraUpdated((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->newLight((*reinterpret_cast< QGLXLight*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: _t->lightDeleted((*reinterpret_cast< QGLXLight*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 7: _t->invalidGraphicsConfigurationDetected(); break;
        case 8: _t->newNeuronRangeAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 9: _t->newNeuronBitAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2])),(*reinterpret_cast< QColor(*)>(_a[3]))); break;
        case 10: _t->newConnectionRangeAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 11: _t->newConnectionBitAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2])),(*reinterpret_cast< QColor(*)>(_a[3]))); break;
        case 12: _t->createConnections((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< GLuint*(*)>(_a[2])),(*reinterpret_cast< GLuint*(*)>(_a[3]))); break;
        case 13: _t->createConnections((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->createNeurons((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVector3D*(*)>(_a[2]))); break;
        case 15: _t->createNeurons((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->addCamera((*reinterpret_cast< QGLXCamera*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 17: _t->switchCamera((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 18: _t->deleteCamera((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 19: _t->addLight((*reinterpret_cast< QGLXLight*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 20: _t->deleteLight((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 21: _t->setMaterial((*reinterpret_cast< QGLXMaterial*(*)>(_a[1]))); break;
        case 22: _t->setNeuronRangeAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< GLfloat*(*)>(_a[2])),(*reinterpret_cast< GLfloat(*)>(_a[3])),(*reinterpret_cast< GLfloat(*)>(_a[4])),(*reinterpret_cast< RangeInferenceFunction(*)>(_a[5]))); break;
        case 23: _t->setNeuronRangeAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< GLfloat*(*)>(_a[2])),(*reinterpret_cast< GLfloat(*)>(_a[3])),(*reinterpret_cast< GLfloat(*)>(_a[4]))); break;
        case 24: _t->setNeuronFlagAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< GLubyte*(*)>(_a[2])),(*reinterpret_cast< QVector3D(*)>(_a[3])),(*reinterpret_cast< QVector3D(*)>(_a[4])),(*reinterpret_cast< FlagInferenceFunction(*)>(_a[5]))); break;
        case 25: _t->setNeuronFlagAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< GLubyte*(*)>(_a[2])),(*reinterpret_cast< QVector3D(*)>(_a[3])),(*reinterpret_cast< QVector3D(*)>(_a[4]))); break;
        case 26: _t->setConnectionRangeAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< GLfloat*(*)>(_a[2])),(*reinterpret_cast< GLfloat(*)>(_a[3])),(*reinterpret_cast< GLfloat(*)>(_a[4]))); break;
        case 27: _t->setConnectionFlagAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< GLubyte*(*)>(_a[2])),(*reinterpret_cast< QVector3D(*)>(_a[3])),(*reinterpret_cast< QVector3D(*)>(_a[4]))); break;
        case 28: _t->changeCurrentNeuronRangeColoration((*reinterpret_cast< QRgb*(*)>(_a[1]))); break;
        case 29: _t->changeCurrentNeuronFlagColoration((*reinterpret_cast< QColor(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 30: _t->changeCurrentConnectionRangeColoration((*reinterpret_cast< QRgb*(*)>(_a[1]))); break;
        case 31: _t->changeCurrentConnectionFlagColoration((*reinterpret_cast< QColor(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 32: _t->setNeuronAttributeToRender((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 33: _t->setConnectionAttributeToRender((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NCV::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NCV::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_NCV,
      qt_meta_data_NCV, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NCV::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NCV::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NCV::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NCV))
        return static_cast<void*>(const_cast< NCV*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int NCV::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 34)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 34;
    }
    return _id;
}

// SIGNAL 0
void NCV::newFPSReading(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NCV::newCurrentCamera(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NCV::newCamera(QGLXCamera * _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NCV::cameraDeleted(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NCV::cameraUpdated(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void NCV::newLight(QGLXLight * _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void NCV::lightDeleted(QGLXLight * _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void NCV::invalidGraphicsConfigurationDetected()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void NCV::newNeuronRangeAttribute(QString _t1, float _t2, float _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void NCV::newNeuronBitAttribute(QString _t1, QColor _t2, QColor _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void NCV::newConnectionRangeAttribute(QString _t1, float _t2, float _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void NCV::newConnectionBitAttribute(QString _t1, QColor _t2, QColor _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_END_MOC_NAMESPACE
