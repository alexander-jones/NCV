/****************************************************************************
** Meta object code from reading C++ file 'managementSidebar.h'
**
** Created: Fri Dec 28 15:10:50 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../source/managementSidebar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'managementSidebar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ManagementSidebar[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      30,   19,   18,   18, 0x0a,
      67,   51,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ManagementSidebar[] = {
    "ManagementSidebar\0\0multiplier\0"
    "updateTimeScale(int)\0framesPerSecond\0"
    "updateFPS(float)\0"
};

void ManagementSidebar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ManagementSidebar *_t = static_cast<ManagementSidebar *>(_o);
        switch (_id) {
        case 0: _t->updateTimeScale((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateFPS((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ManagementSidebar::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ManagementSidebar::staticMetaObject = {
    { &Sidebar::staticMetaObject, qt_meta_stringdata_ManagementSidebar,
      qt_meta_data_ManagementSidebar, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ManagementSidebar::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ManagementSidebar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ManagementSidebar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ManagementSidebar))
        return static_cast<void*>(const_cast< ManagementSidebar*>(this));
    return Sidebar::qt_metacast(_clname);
}

int ManagementSidebar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Sidebar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
