/****************************************************************************
** Meta object code from reading C++ file 'downloadts.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../DownloadTS/downloadts.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'downloadts.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DownloadTS_t {
    QByteArrayData data[16];
    char stringdata0[251];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DownloadTS_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DownloadTS_t qt_meta_stringdata_DownloadTS = {
    {
QT_MOC_LITERAL(0, 0, 10), // "DownloadTS"
QT_MOC_LITERAL(1, 11, 22), // "on_IFileButton_clicked"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 22), // "on_DirDownload_clicked"
QT_MOC_LITERAL(4, 58, 8), // "lookedUp"
QT_MOC_LITERAL(5, 67, 9), // "QHostInfo"
QT_MOC_LITERAL(6, 77, 4), // "host"
QT_MOC_LITERAL(7, 82, 24), // "on_StartDownload_clicked"
QT_MOC_LITERAL(8, 107, 17), // "startNextDownload"
QT_MOC_LITERAL(9, 125, 16), // "downloadProgress"
QT_MOC_LITERAL(10, 142, 13), // "bytesReceived"
QT_MOC_LITERAL(11, 156, 10), // "bytesTotal"
QT_MOC_LITERAL(12, 167, 16), // "downloadFinished"
QT_MOC_LITERAL(13, 184, 17), // "downloadReadyRead"
QT_MOC_LITERAL(14, 202, 23), // "on_StopDownload_clicked"
QT_MOC_LITERAL(15, 226, 24) // "on_PauseDownload_clicked"

    },
    "DownloadTS\0on_IFileButton_clicked\0\0"
    "on_DirDownload_clicked\0lookedUp\0"
    "QHostInfo\0host\0on_StartDownload_clicked\0"
    "startNextDownload\0downloadProgress\0"
    "bytesReceived\0bytesTotal\0downloadFinished\0"
    "downloadReadyRead\0on_StopDownload_clicked\0"
    "on_PauseDownload_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DownloadTS[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    0,   65,    2, 0x08 /* Private */,
       4,    1,   66,    2, 0x08 /* Private */,
       7,    0,   69,    2, 0x08 /* Private */,
       8,    0,   70,    2, 0x08 /* Private */,
       9,    2,   71,    2, 0x08 /* Private */,
      12,    0,   76,    2, 0x08 /* Private */,
      13,    0,   77,    2, 0x08 /* Private */,
      14,    0,   78,    2, 0x08 /* Private */,
      15,    0,   79,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,   10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DownloadTS::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DownloadTS *_t = static_cast<DownloadTS *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_IFileButton_clicked(); break;
        case 1: _t->on_DirDownload_clicked(); break;
        case 2: _t->lookedUp((*reinterpret_cast< QHostInfo(*)>(_a[1]))); break;
        case 3: _t->on_StartDownload_clicked(); break;
        case 4: _t->startNextDownload(); break;
        case 5: _t->downloadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 6: _t->downloadFinished(); break;
        case 7: _t->downloadReadyRead(); break;
        case 8: _t->on_StopDownload_clicked(); break;
        case 9: _t->on_PauseDownload_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QHostInfo >(); break;
            }
            break;
        }
    }
}

const QMetaObject DownloadTS::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DownloadTS.data,
      qt_meta_data_DownloadTS,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DownloadTS::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DownloadTS::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DownloadTS.stringdata0))
        return static_cast<void*>(const_cast< DownloadTS*>(this));
    return QWidget::qt_metacast(_clname);
}

int DownloadTS::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
