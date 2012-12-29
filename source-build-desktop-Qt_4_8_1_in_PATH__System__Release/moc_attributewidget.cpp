/****************************************************************************
** Meta object code from reading C++ file 'attributewidget.h'
**
** Created: Fri Dec 28 15:10:52 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../source/attributewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'attributewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AttributeWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   17,   16,   16, 0x05,
      57,   17,   16,   16, 0x05,
     102,   96,   16,   16, 0x05,
     155,  138,   16,   16, 0x05,
     197,   96,   16,   16, 0x05,
     237,  138,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
     302,  283,   16,   16, 0x0a,
     369,  347,   16,   16, 0x0a,
     414,  283,   16,   16, 0x0a,
     463,  347,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AttributeWidget[] = {
    "AttributeWidget\0\0name\0"
    "currentNeuronAttributeSet(QString)\0"
    "currentConnectionAttributeSet(QString)\0"
    "range\0neuronRangeColorationChanged(QRgb*)\0"
    "offColor,onColor\0"
    "neuronBitColorationChanged(QColor,QColor)\0"
    "connectionRangeColorationChanged(QRgb*)\0"
    "connectionBitColorationChanged(QColor,QColor)\0"
    "name,minVal,maxVal\0"
    "addNeuronRangeAttribute(QString,float,float)\0"
    "name,offColor,onColor\0"
    "addNeuronBitAttribute(QString,QColor,QColor)\0"
    "addConnectionRangeAttribute(QString,float,float)\0"
    "addConnectionBitAttribute(QString,QColor,QColor)\0"
};

void AttributeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AttributeWidget *_t = static_cast<AttributeWidget *>(_o);
        switch (_id) {
        case 0: _t->currentNeuronAttributeSet((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->currentConnectionAttributeSet((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->neuronRangeColorationChanged((*reinterpret_cast< QRgb*(*)>(_a[1]))); break;
        case 3: _t->neuronBitColorationChanged((*reinterpret_cast< QColor(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 4: _t->connectionRangeColorationChanged((*reinterpret_cast< QRgb*(*)>(_a[1]))); break;
        case 5: _t->connectionBitColorationChanged((*reinterpret_cast< QColor(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 6: _t->addNeuronRangeAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 7: _t->addNeuronBitAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2])),(*reinterpret_cast< QColor(*)>(_a[3]))); break;
        case 8: _t->addConnectionRangeAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 9: _t->addConnectionBitAttribute((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2])),(*reinterpret_cast< QColor(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData AttributeWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AttributeWidget::staticMetaObject = {
    { &CollapsibleWidget::staticMetaObject, qt_meta_stringdata_AttributeWidget,
      qt_meta_data_AttributeWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AttributeWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AttributeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AttributeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AttributeWidget))
        return static_cast<void*>(const_cast< AttributeWidget*>(this));
    return CollapsibleWidget::qt_metacast(_clname);
}

int AttributeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CollapsibleWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void AttributeWidget::currentNeuronAttributeSet(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AttributeWidget::currentConnectionAttributeSet(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AttributeWidget::neuronRangeColorationChanged(QRgb * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AttributeWidget::neuronBitColorationChanged(QColor _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void AttributeWidget::connectionRangeColorationChanged(QRgb * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void AttributeWidget::connectionBitColorationChanged(QColor _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
