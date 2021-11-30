/****************************************************************************
** Meta object code from reading C++ file 'GTool.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../GTool/Forms/UI/GTool.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GTool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GTool_t {
    QByteArrayData data[7];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GTool_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GTool_t qt_meta_stringdata_GTool = {
    {
QT_MOC_LITERAL(0, 0, 5), // "GTool"
QT_MOC_LITERAL(1, 6, 22), // "Set_ShowMenuLog_Signal"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 10), // "QList<int>"
QT_MOC_LITERAL(4, 41, 7), // "menuLog"
QT_MOC_LITERAL(5, 49, 18), // "onLogTriggred_Slot"
QT_MOC_LITERAL(6, 68, 20) // "Set_ShowMenuLog_Slot"

    },
    "GTool\0Set_ShowMenuLog_Signal\0\0QList<int>\0"
    "menuLog\0onLogTriggred_Slot\0"
    "Set_ShowMenuLog_Slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GTool[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   32,    2, 0x08 /* Private */,
       6,    1,   33,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void GTool::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GTool *_t = static_cast<GTool *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Set_ShowMenuLog_Signal((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        case 1: _t->onLogTriggred_Slot(); break;
        case 2: _t->Set_ShowMenuLog_Slot((*reinterpret_cast< QList<int>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GTool::*_t)(QList<int> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GTool::Set_ShowMenuLog_Signal)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject GTool::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_GTool.data,
      qt_meta_data_GTool,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *GTool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GTool::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GTool.stringdata0))
        return static_cast<void*>(const_cast< GTool*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int GTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void GTool::Set_ShowMenuLog_Signal(QList<int> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
