/****************************************************************************
** Meta object code from reading C++ file 'SIFormMethod.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Template/Forms/LogicCode/SIFormMethod.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SIFormMethod.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SIFormMethod_t {
    QByteArrayData data[21];
    char stringdata0[225];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SIFormMethod_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SIFormMethod_t qt_meta_stringdata_SIFormMethod = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SIFormMethod"
QT_MOC_LITERAL(1, 13, 20), // "ShowLogMessageSignal"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 7), // "message"
QT_MOC_LITERAL(4, 43, 5), // "level"
QT_MOC_LITERAL(5, 49, 24), // "ShowMessageProcessSignal"
QT_MOC_LITERAL(6, 74, 4), // "flag"
QT_MOC_LITERAL(7, 79, 8), // "Log_Flag"
QT_MOC_LITERAL(8, 88, 11), // "JudgeIDSlot"
QT_MOC_LITERAL(9, 100, 10), // "QLineEdit*"
QT_MOC_LITERAL(10, 111, 4), // "Edit"
QT_MOC_LITERAL(11, 116, 8), // "QString*"
QT_MOC_LITERAL(12, 125, 2), // "ID"
QT_MOC_LITERAL(13, 128, 15), // "JudgeIDTypeSlot"
QT_MOC_LITERAL(14, 144, 9), // "srcobject"
QT_MOC_LITERAL(15, 154, 9), // "desobject"
QT_MOC_LITERAL(16, 164, 22), // "ShowMessageProcessSlot"
QT_MOC_LITERAL(17, 187, 13), // "SelectDirSlot"
QT_MOC_LITERAL(18, 201, 7), // "QLabel*"
QT_MOC_LITERAL(19, 209, 5), // "label"
QT_MOC_LITERAL(20, 215, 9) // "objectDir"

    },
    "SIFormMethod\0ShowLogMessageSignal\0\0"
    "message\0level\0ShowMessageProcessSignal\0"
    "flag\0Log_Flag\0JudgeIDSlot\0QLineEdit*\0"
    "Edit\0QString*\0ID\0JudgeIDTypeSlot\0"
    "srcobject\0desobject\0ShowMessageProcessSlot\0"
    "SelectDirSlot\0QLabel*\0label\0objectDir"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SIFormMethod[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x06 /* Public */,
       5,    2,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    2,   54,    2, 0x0a /* Public */,
      13,    3,   59,    2, 0x0a /* Public */,
      16,    2,   66,    2, 0x0a /* Public */,
      17,    2,   71,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList, QMetaType::UInt,    3,    4,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,    6,    7,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 11,   10,   12,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 11, 0x80000000 | 11,   10,   14,   15,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,    6,    7,
    QMetaType::Void, 0x80000000 | 18, 0x80000000 | 11,   19,   20,

       0        // eod
};

void SIFormMethod::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SIFormMethod *_t = static_cast<SIFormMethod *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ShowLogMessageSignal((*reinterpret_cast< const QStringList(*)>(_a[1])),(*reinterpret_cast< const uint(*)>(_a[2]))); break;
        case 1: _t->ShowMessageProcessSignal((*reinterpret_cast< const uint(*)>(_a[1])),(*reinterpret_cast< const uint(*)>(_a[2]))); break;
        case 2: _t->JudgeIDSlot((*reinterpret_cast< QLineEdit*(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 3: _t->JudgeIDTypeSlot((*reinterpret_cast< QLineEdit*(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2])),(*reinterpret_cast< QString*(*)>(_a[3]))); break;
        case 4: _t->ShowMessageProcessSlot((*reinterpret_cast< const uint(*)>(_a[1])),(*reinterpret_cast< const uint(*)>(_a[2]))); break;
        case 5: _t->SelectDirSlot((*reinterpret_cast< QLabel*(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLineEdit* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLineEdit* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLabel* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SIFormMethod::*_t)(const QStringList , const unsigned int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIFormMethod::ShowLogMessageSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SIFormMethod::*_t)(const unsigned int , const unsigned int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIFormMethod::ShowMessageProcessSignal)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject SIFormMethod::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SIFormMethod.data,
      qt_meta_data_SIFormMethod,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SIFormMethod::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SIFormMethod::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SIFormMethod.stringdata0))
        return static_cast<void*>(const_cast< SIFormMethod*>(this));
    return QObject::qt_metacast(_clname);
}

int SIFormMethod::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SIFormMethod::ShowLogMessageSignal(const QStringList _t1, const unsigned int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SIFormMethod::ShowMessageProcessSignal(const unsigned int _t1, const unsigned int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
