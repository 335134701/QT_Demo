/****************************************************************************
** Meta object code from reading C++ file 'consoleappender.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Temple/QLog/log4qt/consoleappender.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'consoleappender.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Log4Qt__ConsoleAppender_t {
    QByteArrayData data[5];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Log4Qt__ConsoleAppender_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Log4Qt__ConsoleAppender_t qt_meta_stringdata_Log4Qt__ConsoleAppender = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Log4Qt::ConsoleAppender"
QT_MOC_LITERAL(1, 24, 6), // "target"
QT_MOC_LITERAL(2, 31, 6), // "Target"
QT_MOC_LITERAL(3, 38, 13), // "STDOUT_TARGET"
QT_MOC_LITERAL(4, 52, 13) // "STDERR_TARGET"

    },
    "Log4Qt::ConsoleAppender\0target\0Target\0"
    "STDOUT_TARGET\0STDERR_TARGET"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Log4Qt__ConsoleAppender[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   14, // properties
       1,   17, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QString, 0x00095103,

 // enums: name, flags, count, data
       2, 0x0,    2,   21,

 // enum data: key, value
       3, uint(Log4Qt::ConsoleAppender::STDOUT_TARGET),
       4, uint(Log4Qt::ConsoleAppender::STDERR_TARGET),

       0        // eod
};

void Log4Qt::ConsoleAppender::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        ConsoleAppender *_t = static_cast<ConsoleAppender *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->target(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        ConsoleAppender *_t = static_cast<ConsoleAppender *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setTarget(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Log4Qt::ConsoleAppender::staticMetaObject = {
    { &WriterAppender::staticMetaObject, qt_meta_stringdata_Log4Qt__ConsoleAppender.data,
      qt_meta_data_Log4Qt__ConsoleAppender,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Log4Qt::ConsoleAppender::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Log4Qt::ConsoleAppender::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Log4Qt__ConsoleAppender.stringdata0))
        return static_cast<void*>(const_cast< ConsoleAppender*>(this));
    return WriterAppender::qt_metacast(_clname);
}

int Log4Qt::ConsoleAppender::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WriterAppender::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
   if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
