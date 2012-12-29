/****************************************************************************
** Meta object code from reading C++ file 'lightingsidebar.h'
**
** Created: Fri Dec 28 14:46:16 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../source/lightingsidebar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lightingsidebar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LightingSidebar[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      28,   17,   16,   16, 0x05,
      66,   61,   16,   16, 0x05,
      88,   61,   16,   16, 0x05,
     111,   61,   16,   16, 0x05,
     133,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
     160,   61,   16,   16, 0x0a,
     187,  181,   16,   16, 0x0a,
     215,   16,   16,   16, 0x08,
     230,   16,   16,   16, 0x08,
     248,   16,   16,   16, 0x08,
     277,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LightingSidebar[] = {
    "LightingSidebar\0\0light,name\0"
    "lightCreated(QGLXLight*,QString)\0name\0"
    "lightDeleted(QString)\0lightSwitched(QString)\0"
    "lightUpdated(QString)\0newMaterial(QGLXMaterial*)\0"
    "updateLight(QString)\0light\0"
    "setLightDefault(QGLXLight*)\0m_addPressed()\0"
    "m_deletePressed()\0m_lightingAttributeChanged()\0"
    "m_materialPropertyChanged()\0"
};

void LightingSidebar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LightingSidebar *_t = static_cast<LightingSidebar *>(_o);
        switch (_id) {
        case 0: _t->lightCreated((*reinterpret_cast< QGLXLight*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->lightDeleted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->lightSwitched((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->lightUpdated((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->newMaterial((*reinterpret_cast< QGLXMaterial*(*)>(_a[1]))); break;
        case 5: _t->updateLight((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->setLightDefault((*reinterpret_cast< QGLXLight*(*)>(_a[1]))); break;
        case 7: _t->m_addPressed(); break;
        case 8: _t->m_deletePressed(); break;
        case 9: _t->m_lightingAttributeChanged(); break;
        case 10: _t->m_materialPropertyChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData LightingSidebar::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LightingSidebar::staticMetaObject = {
    { &CollapsibleWidget::staticMetaObject, qt_meta_stringdata_LightingSidebar,
      qt_meta_data_LightingSidebar, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LightingSidebar::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LightingSidebar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LightingSidebar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LightingSidebar))
        return static_cast<void*>(const_cast< LightingSidebar*>(this));
    return CollapsibleWidget::qt_metacast(_clname);
}

int LightingSidebar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CollapsibleWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void LightingSidebar::lightCreated(QGLXLight * _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LightingSidebar::lightDeleted(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LightingSidebar::lightSwitched(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void LightingSidebar::lightUpdated(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void LightingSidebar::newMaterial(QGLXMaterial * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
