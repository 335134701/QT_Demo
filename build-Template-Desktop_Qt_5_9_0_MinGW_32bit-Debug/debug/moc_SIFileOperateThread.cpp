/****************************************************************************
** Meta object code from reading C++ file 'SIFileOperateThread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Template/CommonMethod/SIForm/SIFileOperateThread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SIFileOperateThread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SIFileOperateThread_t {
    QByteArrayData data[15];
    char stringdata0[154];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SIFileOperateThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SIFileOperateThread_t qt_meta_stringdata_SIFileOperateThread = {
    {
QT_MOC_LITERAL(0, 0, 19), // "SIFileOperateThread"
QT_MOC_LITERAL(1, 20, 18), // "EndUpdateSVNSignal"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 6), // "result"
QT_MOC_LITERAL(4, 47, 18), // "EndFileSearcSignal"
QT_MOC_LITERAL(5, 66, 8), // "filePath"
QT_MOC_LITERAL(6, 75, 4), // "flag"
QT_MOC_LITERAL(7, 80, 6), // "isGoON"
QT_MOC_LITERAL(8, 87, 13), // "UpdateSVNSlot"
QT_MOC_LITERAL(9, 101, 11), // "exeFilePath"
QT_MOC_LITERAL(10, 113, 7), // "dirPath"
QT_MOC_LITERAL(11, 121, 14), // "FileSearchSlot"
QT_MOC_LITERAL(12, 136, 7), // "filters"
QT_MOC_LITERAL(13, 144, 2), // "ID"
QT_MOC_LITERAL(14, 147, 6) // "IDType"

    },
    "SIFileOperateThread\0EndUpdateSVNSignal\0"
    "\0result\0EndFileSearcSignal\0filePath\0"
    "flag\0isGoON\0UpdateSVNSlot\0exeFilePath\0"
    "dirPath\0FileSearchSlot\0filters\0ID\0"
    "IDType"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SIFileOperateThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    3,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    2,   44,    2, 0x0a /* Public */,
      11,    6,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::UInt, QMetaType::Bool,    5,    6,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    9,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList, QMetaType::QString, QMetaType::QString, QMetaType::UInt, QMetaType::Bool,   10,   12,   13,   14,    6,    7,

       0        // eod
};

void SIFileOperateThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SIFileOperateThread *_t = static_cast<SIFileOperateThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->EndUpdateSVNSignal((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 1: _t->EndFileSearcSignal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: _t->UpdateSVNSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->FileSearchSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< uint(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SIFileOperateThread::*_t)(const bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIFileOperateThread::EndUpdateSVNSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SIFileOperateThread::*_t)(const QString , unsigned int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIFileOperateThread::EndFileSearcSignal)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject SIFileOperateThread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SIFileOperateThread.data,
      qt_meta_data_SIFileOperateThread,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SIFileOperateThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SIFileOperateThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SIFileOperateThread.stringdata0))
        return static_cast<void*>(const_cast< SIFileOperateThread*>(this));
    return QObject::qt_metacast(_clname);
}

int SIFileOperateThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SIFileOperateThread::EndUpdateSVNSignal(const bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SIFileOperateThread::EndFileSearcSignal(const QString _t1, unsigned int _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
