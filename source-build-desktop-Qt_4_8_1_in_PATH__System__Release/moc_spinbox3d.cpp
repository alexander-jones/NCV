/****************************************************************************
** Meta object code from reading C++ file 'spinbox3d.h'
**
** Created: Wed Dec 26 22:01:44 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../source/spinbox3d.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spinbox3d.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ExpandableSpinBox[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ExpandableSpinBox[] = {
    "ExpandableSpinBox\0\0entered()\0"
};

void ExpandableSpinBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ExpandableSpinBox *_t = static_cast<ExpandableSpinBox *>(_o);
        switch (_id) {
        case 0: _t->entered(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ExpandableSpinBox::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ExpandableSpinBox::staticMetaObject = {
    { &QDoubleSpinBox::staticMetaObject, qt_meta_stringdata_ExpandableSpinBox,
      qt_meta_data_ExpandableSpinBox, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ExpandableSpinBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ExpandableSpinBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ExpandableSpinBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ExpandableSpinBox))
        return static_cast<void*>(const_cast< ExpandableSpinBox*>(this));
    return QDoubleSpinBox::qt_metacast(_clname);
}

int ExpandableSpinBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDoubleSpinBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void ExpandableSpinBox::entered()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_SpinBox3D[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      25,   10,   10,   10, 0x08,
      47,   10,   10,   10, 0x08,
      59,   10,   10,   10, 0x08,
      71,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SpinBox3D[] = {
    "SpinBox3D\0\0valueEdited()\0m_subValueSubmitted()\0"
    "m_expandX()\0m_expandY()\0m_expandZ()\0"
};

void SpinBox3D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpinBox3D *_t = static_cast<SpinBox3D *>(_o);
        switch (_id) {
        case 0: _t->valueEdited(); break;
        case 1: _t->m_subValueSubmitted(); break;
        case 2: _t->m_expandX(); break;
        case 3: _t->m_expandY(); break;
        case 4: _t->m_expandZ(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SpinBox3D::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpinBox3D::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SpinBox3D,
      qt_meta_data_SpinBox3D, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpinBox3D::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpinBox3D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpinBox3D::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpinBox3D))
        return static_cast<void*>(const_cast< SpinBox3D*>(this));
    return QWidget::qt_metacast(_clname);
}

int SpinBox3D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void SpinBox3D::valueEdited()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
