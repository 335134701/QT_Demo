/****************************************************************************
** Meta object code from reading C++ file 'Log.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../QLog/Log.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Log.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Log_t {
    QByteArrayData data[11];
    char stringdata0[65];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Log_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Log_t qt_meta_stringdata_Log = {
    {
QT_MOC_LITERAL(0, 0, 3), // "Log"
QT_MOC_LITERAL(1, 4, 8), // "instance"
QT_MOC_LITERAL(2, 13, 4), // "Log*"
QT_MOC_LITERAL(3, 18, 0), // ""
QT_MOC_LITERAL(4, 19, 4), // "init"
QT_MOC_LITERAL(5, 24, 14), // "configFilePath"
QT_MOC_LITERAL(6, 39, 5), // "debug"
QT_MOC_LITERAL(7, 45, 3), // "msg"
QT_MOC_LITERAL(8, 49, 4), // "info"
QT_MOC_LITERAL(9, 54, 4), // "warn"
QT_MOC_LITERAL(10, 59, 5) // "error"

    },
    "Log\0instance\0Log*\0\0init\0configFilePath\0"
    "debug\0msg\0info\0warn\0error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Log[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    3, 0x0a /* Public */,
       4,    1,   45,    3, 0x0a /* Public */,
       6,    1,   48,    3, 0x0a /* Public */,
       8,    1,   51,    3, 0x0a /* Public */,
       9,    1,   54,    3, 0x0a /* Public */,
      10,    1,   57,    3, 0x0a /* Public */,

 // slots: parameters
    0x80000000 | 2,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    7,

       0        // eod
};

void Log::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Log *_t = static_cast<Log *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { Log* _r = _t->instance();
            if (_a[0]) *reinterpret_cast< Log**>(_a[0]) = std::move(_r); }  break;
        case 1: _t->init((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->debug((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->info((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->warn((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Log::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Log.data,
      qt_meta_data_Log,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Log::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Log::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Log.stringdata0))
        return static_cast<void*>(const_cast< Log*>(this));
    return QObject::qt_metacast(_clname);
}

int Log::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
