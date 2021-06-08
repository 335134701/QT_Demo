/****************************************************************************
** Meta object code from reading C++ file 'SIExcelOperateThread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Template/CommonMethod/SIForm/SIExcelOperateThread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SIExcelOperateThread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SIExcelOperateThread_t {
    QByteArrayData data[20];
    char stringdata0[306];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SIExcelOperateThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SIExcelOperateThread_t qt_meta_stringdata_SIExcelOperateThread = {
    {
QT_MOC_LITERAL(0, 0, 20), // "SIExcelOperateThread"
QT_MOC_LITERAL(1, 21, 22), // "EndReadSoftExcelSignal"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 25), // "QList<SI_SOFTNUMBERTable>"
QT_MOC_LITERAL(4, 71, 8), // "softList"
QT_MOC_LITERAL(5, 80, 20), // "QList<SI_ERRORTable>"
QT_MOC_LITERAL(6, 101, 7), // "errList"
QT_MOC_LITERAL(7, 109, 28), // "EndReadDefineFileExcelSignal"
QT_MOC_LITERAL(8, 138, 23), // "QList<SI_DEFINEMESSAGE>"
QT_MOC_LITERAL(9, 162, 10), // "defineList"
QT_MOC_LITERAL(10, 173, 27), // "EndInferRelyIDProcessSignal"
QT_MOC_LITERAL(11, 201, 4), // "flag"
QT_MOC_LITERAL(12, 206, 19), // "ReadExcelThreadSlot"
QT_MOC_LITERAL(13, 226, 8), // "filePath"
QT_MOC_LITERAL(14, 235, 2), // "ID"
QT_MOC_LITERAL(15, 238, 6), // "IDType"
QT_MOC_LITERAL(16, 245, 22), // "InferRelyIDProcessSlot"
QT_MOC_LITERAL(17, 268, 12), // "relyFilePath"
QT_MOC_LITERAL(18, 281, 14), // "defineFilePath"
QT_MOC_LITERAL(19, 296, 9) // "condition"

    },
    "SIExcelOperateThread\0EndReadSoftExcelSignal\0"
    "\0QList<SI_SOFTNUMBERTable>\0softList\0"
    "QList<SI_ERRORTable>\0errList\0"
    "EndReadDefineFileExcelSignal\0"
    "QList<SI_DEFINEMESSAGE>\0defineList\0"
    "EndInferRelyIDProcessSignal\0flag\0"
    "ReadExcelThreadSlot\0filePath\0ID\0IDType\0"
    "InferRelyIDProcessSlot\0relyFilePath\0"
    "defineFilePath\0condition"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SIExcelOperateThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       7,    2,   44,    2, 0x06 /* Public */,
      10,    4,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    4,   58,    2, 0x0a /* Public */,
      16,    6,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 8, 0x80000000 | 5,    9,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 8, 0x80000000 | 5, QMetaType::UInt,    4,    9,    6,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::UInt,   13,   14,   15,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::UInt,   17,   18,   14,   15,   19,   11,

       0        // eod
};

void SIExcelOperateThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SIExcelOperateThread *_t = static_cast<SIExcelOperateThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->EndReadSoftExcelSignal((*reinterpret_cast< const QList<SI_SOFTNUMBERTable>(*)>(_a[1])),(*reinterpret_cast< const QList<SI_ERRORTable>(*)>(_a[2]))); break;
        case 1: _t->EndReadDefineFileExcelSignal((*reinterpret_cast< const QList<SI_DEFINEMESSAGE>(*)>(_a[1])),(*reinterpret_cast< const QList<SI_ERRORTable>(*)>(_a[2]))); break;
        case 2: _t->EndInferRelyIDProcessSignal((*reinterpret_cast< const QList<SI_SOFTNUMBERTable>(*)>(_a[1])),(*reinterpret_cast< const QList<SI_DEFINEMESSAGE>(*)>(_a[2])),(*reinterpret_cast< const QList<SI_ERRORTable>(*)>(_a[3])),(*reinterpret_cast< const uint(*)>(_a[4]))); break;
        case 3: _t->ReadExcelThreadSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const uint(*)>(_a[4]))); break;
        case 4: _t->InferRelyIDProcessSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const uint(*)>(_a[6]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SIExcelOperateThread::*_t)(const QList<SI_SOFTNUMBERTable> , const QList<SI_ERRORTable> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIExcelOperateThread::EndReadSoftExcelSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SIExcelOperateThread::*_t)(const QList<SI_DEFINEMESSAGE> , const QList<SI_ERRORTable> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIExcelOperateThread::EndReadDefineFileExcelSignal)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SIExcelOperateThread::*_t)(const QList<SI_SOFTNUMBERTable> , const QList<SI_DEFINEMESSAGE> , const QList<SI_ERRORTable> , const unsigned int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIExcelOperateThread::EndInferRelyIDProcessSignal)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject SIExcelOperateThread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SIExcelOperateThread.data,
      qt_meta_data_SIExcelOperateThread,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SIExcelOperateThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SIExcelOperateThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SIExcelOperateThread.stringdata0))
        return static_cast<void*>(const_cast< SIExcelOperateThread*>(this));
    return QObject::qt_metacast(_clname);
}

int SIExcelOperateThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void SIExcelOperateThread::EndReadSoftExcelSignal(const QList<SI_SOFTNUMBERTable> _t1, const QList<SI_ERRORTable> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SIExcelOperateThread::EndReadDefineFileExcelSignal(const QList<SI_DEFINEMESSAGE> _t1, const QList<SI_ERRORTable> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SIExcelOperateThread::EndInferRelyIDProcessSignal(const QList<SI_SOFTNUMBERTable> _t1, const QList<SI_DEFINEMESSAGE> _t2, const QList<SI_ERRORTable> _t3, const unsigned int _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
