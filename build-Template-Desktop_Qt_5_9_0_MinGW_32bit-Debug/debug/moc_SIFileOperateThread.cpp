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
#include <QtCore/QList>
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
    QByteArrayData data[32];
    char stringdata0[410];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SIFileOperateThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SIFileOperateThread_t qt_meta_stringdata_SIFileOperateThread = {
    {
QT_MOC_LITERAL(0, 0, 19), // "SIFileOperateThread"
QT_MOC_LITERAL(1, 20, 17), // "EndRunOrderSignal"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 6), // "result"
QT_MOC_LITERAL(4, 46, 4), // "flag"
QT_MOC_LITERAL(5, 51, 21), // "EndCopyCodeFileSignal"
QT_MOC_LITERAL(6, 73, 8), // "filePath"
QT_MOC_LITERAL(7, 82, 18), // "EndFileSearcSignal"
QT_MOC_LITERAL(8, 101, 6), // "isGoON"
QT_MOC_LITERAL(9, 108, 20), // "EndCheckBAFileSignal"
QT_MOC_LITERAL(10, 129, 20), // "QList<SI_ERRORTable>"
QT_MOC_LITERAL(11, 150, 7), // "errList"
QT_MOC_LITERAL(12, 158, 20), // "EndCheckCLFileSignal"
QT_MOC_LITERAL(13, 179, 13), // "UpdateSVNSlot"
QT_MOC_LITERAL(14, 193, 11), // "exeFilePath"
QT_MOC_LITERAL(15, 205, 7), // "dirPath"
QT_MOC_LITERAL(16, 213, 17), // "UNZipCodeFileSlot"
QT_MOC_LITERAL(17, 231, 10), // "desDirPath"
QT_MOC_LITERAL(18, 242, 15), // "ZipCodeFileSlot"
QT_MOC_LITERAL(19, 258, 6), // "IDType"
QT_MOC_LITERAL(20, 265, 3), // "txt"
QT_MOC_LITERAL(21, 269, 6), // "APPVer"
QT_MOC_LITERAL(22, 276, 14), // "FileSearchSlot"
QT_MOC_LITERAL(23, 291, 7), // "filters"
QT_MOC_LITERAL(24, 299, 2), // "ID"
QT_MOC_LITERAL(25, 302, 15), // "CheckBAFileSlot"
QT_MOC_LITERAL(26, 318, 25), // "QList<SI_SOFTNUMBERTable>"
QT_MOC_LITERAL(27, 344, 8), // "softList"
QT_MOC_LITERAL(28, 353, 15), // "CheckCLFileSlot"
QT_MOC_LITERAL(29, 369, 16), // "CopyCodeFileSlot"
QT_MOC_LITERAL(30, 386, 11), // "srcFilePath"
QT_MOC_LITERAL(31, 398, 11) // "desFilePath"

    },
    "SIFileOperateThread\0EndRunOrderSignal\0"
    "\0result\0flag\0EndCopyCodeFileSignal\0"
    "filePath\0EndFileSearcSignal\0isGoON\0"
    "EndCheckBAFileSignal\0QList<SI_ERRORTable>\0"
    "errList\0EndCheckCLFileSignal\0UpdateSVNSlot\0"
    "exeFilePath\0dirPath\0UNZipCodeFileSlot\0"
    "desDirPath\0ZipCodeFileSlot\0IDType\0txt\0"
    "APPVer\0FileSearchSlot\0filters\0ID\0"
    "CheckBAFileSlot\0QList<SI_SOFTNUMBERTable>\0"
    "softList\0CheckCLFileSlot\0CopyCodeFileSlot\0"
    "srcFilePath\0desFilePath"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SIFileOperateThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   74,    2, 0x06 /* Public */,
       5,    2,   79,    2, 0x06 /* Public */,
       7,    3,   84,    2, 0x06 /* Public */,
       9,    2,   91,    2, 0x06 /* Public */,
      12,    2,   96,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    3,  101,    2, 0x0a /* Public */,
      16,    4,  108,    2, 0x0a /* Public */,
      18,    6,  117,    2, 0x0a /* Public */,
      22,    6,  130,    2, 0x0a /* Public */,
      25,    5,  143,    2, 0x0a /* Public */,
      28,    3,  154,    2, 0x0a /* Public */,
      29,    2,  161,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::UInt,    3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    6,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::UInt, QMetaType::Bool,    6,    4,    8,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 10,    3,   11,
    QMetaType::Void, QMetaType::Bool, 0x80000000 | 10,    3,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::UInt,   14,   15,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::UInt,   14,    6,   17,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::UInt,   14,   15,   19,   20,   21,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList, QMetaType::QString, QMetaType::QString, QMetaType::UInt, QMetaType::Bool,   15,   23,   24,   19,    4,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, 0x80000000 | 26, QMetaType::UInt,   15,   24,   19,   27,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, 0x80000000 | 26,   15,   24,   27,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   30,   31,

       0        // eod
};

void SIFileOperateThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SIFileOperateThread *_t = static_cast<SIFileOperateThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->EndRunOrderSignal((*reinterpret_cast< const bool(*)>(_a[1])),(*reinterpret_cast< const uint(*)>(_a[2]))); break;
        case 1: _t->EndCopyCodeFileSignal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const bool(*)>(_a[2]))); break;
        case 2: _t->EndFileSearcSignal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 3: _t->EndCheckBAFileSignal((*reinterpret_cast< const bool(*)>(_a[1])),(*reinterpret_cast< QList<SI_ERRORTable>(*)>(_a[2]))); break;
        case 4: _t->EndCheckCLFileSignal((*reinterpret_cast< const bool(*)>(_a[1])),(*reinterpret_cast< QList<SI_ERRORTable>(*)>(_a[2]))); break;
        case 5: _t->UpdateSVNSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const uint(*)>(_a[3]))); break;
        case 6: _t->UNZipCodeFileSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const uint(*)>(_a[4]))); break;
        case 7: _t->ZipCodeFileSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< const uint(*)>(_a[6]))); break;
        case 8: _t->FileSearchSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< uint(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6]))); break;
        case 9: _t->CheckBAFileSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QList<SI_SOFTNUMBERTable>(*)>(_a[4])),(*reinterpret_cast< const uint(*)>(_a[5]))); break;
        case 10: _t->CheckCLFileSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QList<SI_SOFTNUMBERTable>(*)>(_a[3]))); break;
        case 11: _t->CopyCodeFileSlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SIFileOperateThread::*_t)(const bool , const unsigned int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIFileOperateThread::EndRunOrderSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SIFileOperateThread::*_t)(const QString , const bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIFileOperateThread::EndCopyCodeFileSignal)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SIFileOperateThread::*_t)(const QString , unsigned int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIFileOperateThread::EndFileSearcSignal)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (SIFileOperateThread::*_t)(const bool , QList<SI_ERRORTable> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIFileOperateThread::EndCheckBAFileSignal)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (SIFileOperateThread::*_t)(const bool , QList<SI_ERRORTable> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIFileOperateThread::EndCheckCLFileSignal)) {
                *result = 4;
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
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void SIFileOperateThread::EndRunOrderSignal(const bool _t1, const unsigned int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SIFileOperateThread::EndCopyCodeFileSignal(const QString _t1, const bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SIFileOperateThread::EndFileSearcSignal(const QString _t1, unsigned int _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SIFileOperateThread::EndCheckBAFileSignal(const bool _t1, QList<SI_ERRORTable> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SIFileOperateThread::EndCheckCLFileSignal(const bool _t1, QList<SI_ERRORTable> _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
