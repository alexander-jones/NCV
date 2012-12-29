/****************************************************************************
** Meta object code from reading C++ file 'camerasidebar.h'
**
** Created: Fri Dec 28 14:46:14 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../source/camerasidebar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camerasidebar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CameraSidebar[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   15,   14,   14, 0x05,
      64,   59,   14,   14, 0x05,
      88,   59,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     123,  111,   14,   14, 0x0a,
     154,   59,   14,   14, 0x0a,
     176,   59,   14,   14, 0x0a,
     198,   14,   14,   14, 0x08,
     213,   14,   14,   14, 0x08,
     231,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CameraSidebar[] = {
    "CameraSidebar\0\0cam,name\0"
    "cameraCreated(QGLXCamera*,QString)\0"
    "name\0cameraSwitched(QString)\0"
    "cameraDeleted(QString)\0camera,name\0"
    "addCamera(QGLXCamera*,QString)\0"
    "switchCamera(QString)\0updateCamera(QString)\0"
    "m_addPressed()\0m_deletePressed()\0"
    "m_attribChanged()\0"
};

void CameraSidebar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CameraSidebar *_t = static_cast<CameraSidebar *>(_o);
        switch (_id) {
        case 0: _t->cameraCreated((*reinterpret_cast< QGLXCamera*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->cameraSwitched((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->cameraDeleted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->addCamera((*reinterpret_cast< QGLXCamera*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: _t->switchCamera((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->updateCamera((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->m_addPressed(); break;
        case 7: _t->m_deletePressed(); break;
        case 8: _t->m_attribChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CameraSidebar::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CameraSidebar::staticMetaObject = {
    { &CollapsibleWidget::staticMetaObject, qt_meta_stringdata_CameraSidebar,
      qt_meta_data_CameraSidebar, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CameraSidebar::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CameraSidebar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CameraSidebar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CameraSidebar))
        return static_cast<void*>(const_cast< CameraSidebar*>(this));
    return CollapsibleWidget::qt_metacast(_clname);
}

int CameraSidebar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CollapsibleWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void CameraSidebar::cameraCreated(QGLXCamera * _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CameraSidebar::cameraSwitched(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CameraSidebar::cameraDeleted(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
