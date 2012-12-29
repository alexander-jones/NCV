/****************************************************************************
** Meta object code from reading C++ file 'collapsiblewidget.h'
**
** Created: Wed Dec 26 22:01:46 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../source/collapsiblewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'collapsiblewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CollapsibleWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      31,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      42,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CollapsibleWidget[] = {
    "CollapsibleWidget\0\0collapsed()\0"
    "expanded()\0m_collapseToolPressed()\0"
};

void CollapsibleWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CollapsibleWidget *_t = static_cast<CollapsibleWidget *>(_o);
        switch (_id) {
        case 0: _t->collapsed(); break;
        case 1: _t->expanded(); break;
        case 2: _t->m_collapseToolPressed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CollapsibleWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CollapsibleWidget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_CollapsibleWidget,
      qt_meta_data_CollapsibleWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CollapsibleWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CollapsibleWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CollapsibleWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CollapsibleWidget))
        return static_cast<void*>(const_cast< CollapsibleWidget*>(this));
    return QFrame::qt_metacast(_clname);
}

int CollapsibleWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void CollapsibleWidget::collapsed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CollapsibleWidget::expanded()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
