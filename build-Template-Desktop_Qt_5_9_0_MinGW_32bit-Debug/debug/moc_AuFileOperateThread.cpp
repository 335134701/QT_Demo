/****************************************************************************
** Meta object code from reading C++ file 'AuFileOperateThread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Template/CommonMethod/AutomationForm/AuFileOperateThread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AuFileOperateThread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AuFileOperateThread_t {
    QByteArrayData data[11];
    char stringdata0[102];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AuFileOperateThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AuFileOperateThread_t qt_meta_stringdata_AuFileOperateThread = {
    {
QT_MOC_LITERAL(0, 0, 19), // "AuFileOperateThread"
QT_MOC_LITERAL(1, 20, 18), // "EndFileSearcSignal"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 8), // "filePath"
QT_MOC_LITERAL(4, 49, 4), // "flag"
QT_MOC_LITERAL(5, 54, 6), // "isGoON"
QT_MOC_LITERAL(6, 61, 14), // "FileSearchSlot"
QT_MOC_LITERAL(7, 76, 7), // "dirPath"
QT_MOC_LITERAL(8, 84, 7), // "filters"
QT_MOC_LITERAL(9, 92, 2), // "ID"
QT_MOC_LITERAL(10, 95, 6) // "IDType"

    },
    "AuFileOperateThread\0EndFileSearcSignal\0"
    "\0filePath\0flag\0isGoON\0FileSearchSlot\0"
    "dirPath\0filters\0ID\0IDType"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AuFileOperateThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    6,   31,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::UInt, QMetaType::Bool,    3,    4,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList, QMetaType::QString, QMetaType::QString, QMetaType::UInt, QMetaType::Bool,    7,    8,    9,   10,    4,    5,

       0        // eod
};

void AuFileOperateThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AuFileOperateThread *_t = static_cast<AuFileOperateThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->EndFileSearcSignal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->FileSearchSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< uint(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AuFileOperateThread::*_t)(const QString , unsigned int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AuFileOperateThread::EndFileSearcSignal)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject AuFileOperateThread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AuFileOperateThread.data,
      qt_meta_data_AuFileOperateThread,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AuFileOperateThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AuFileOperateThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AuFileOperateThread.stringdata0))
        return static_cast<void*>(const_cast< AuFileOperateThread*>(this));
    return QObject::qt_metacast(_clname);
}

int AuFileOperateThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void AuFileOperateThread::EndFileSearcSignal(const QString _t1, unsigned int _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
