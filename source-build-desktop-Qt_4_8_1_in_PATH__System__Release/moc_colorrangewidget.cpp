/****************************************************************************
** Meta object code from reading C++ file 'colorrangewidget.h'
**
** Created: Fri Dec 28 15:10:51 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../source/colorrangewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'colorrangewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ColorRangeWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   18,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      52,   17,   17,   17, 0x08,
      78,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ColorRangeWidget[] = {
    "ColorRangeWidget\0\0newRange\0"
    "colorRangeChanged(QRgb*)\0"
    "m_gradientButtonClicked()\0"
    "m_colorButtonClicked()\0"
};

void ColorRangeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ColorRangeWidget *_t = static_cast<ColorRangeWidget *>(_o);
        switch (_id) {
        case 0: _t->colorRangeChanged((*reinterpret_cast< QRgb*(*)>(_a[1]))); break;
        case 1: _t->m_gradientButtonClicked(); break;
        case 2: _t->m_colorButtonClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ColorRangeWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ColorRangeWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ColorRangeWidget,
      qt_meta_data_ColorRangeWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ColorRangeWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ColorRangeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ColorRangeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ColorRangeWidget))
        return static_cast<void*>(const_cast< ColorRangeWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ColorRangeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void ColorRangeWidget::colorRangeChanged(QRgb * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
