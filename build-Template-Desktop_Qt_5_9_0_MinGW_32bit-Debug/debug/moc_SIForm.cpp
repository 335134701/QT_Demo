/****************************************************************************
** Meta object code from reading C++ file 'SIForm.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Template/Forms/UICode/SIForm.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SIForm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SIForm_t {
    QByteArrayData data[21];
    char stringdata0[276];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SIForm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SIForm_t qt_meta_stringdata_SIForm = {
    {
QT_MOC_LITERAL(0, 0, 6), // "SIForm"
QT_MOC_LITERAL(1, 7, 13), // "JudgeIDSignal"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 10), // "QLineEdit*"
QT_MOC_LITERAL(4, 33, 4), // "Edit"
QT_MOC_LITERAL(5, 38, 8), // "QString*"
QT_MOC_LITERAL(6, 47, 2), // "ID"
QT_MOC_LITERAL(7, 50, 17), // "JudgeIDTypeSignal"
QT_MOC_LITERAL(8, 68, 9), // "srcobject"
QT_MOC_LITERAL(9, 78, 9), // "desobject"
QT_MOC_LITERAL(10, 88, 24), // "ShowMessageProcessSignal"
QT_MOC_LITERAL(11, 113, 4), // "flag"
QT_MOC_LITERAL(12, 118, 8), // "Log_Flag"
QT_MOC_LITERAL(13, 127, 15), // "SelectDirSignal"
QT_MOC_LITERAL(14, 143, 7), // "QLabel*"
QT_MOC_LITERAL(15, 151, 5), // "label"
QT_MOC_LITERAL(16, 157, 9), // "objectDir"
QT_MOC_LITERAL(17, 167, 27), // "on_SIIDEdit_editingFinished"
QT_MOC_LITERAL(18, 195, 31), // "on_SIRelyIDEdit_editingFinished"
QT_MOC_LITERAL(19, 227, 22), // "on_SISVNButton_clicked"
QT_MOC_LITERAL(20, 250, 25) // "on_SIOutputButton_clicked"

    },
    "SIForm\0JudgeIDSignal\0\0QLineEdit*\0Edit\0"
    "QString*\0ID\0JudgeIDTypeSignal\0srcobject\0"
    "desobject\0ShowMessageProcessSignal\0"
    "flag\0Log_Flag\0SelectDirSignal\0QLabel*\0"
    "label\0objectDir\0on_SIIDEdit_editingFinished\0"
    "on_SIRelyIDEdit_editingFinished\0"
    "on_SISVNButton_clicked\0on_SIOutputButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SIForm[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   54,    2, 0x06 /* Public */,
       7,    3,   59,    2, 0x06 /* Public */,
      10,    2,   66,    2, 0x06 /* Public */,
      13,    2,   71,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    0,   76,    2, 0x08 /* Private */,
      18,    0,   77,    2, 0x08 /* Private */,
      19,    0,   78,    2, 0x08 /* Private */,
      20,    0,   79,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    4,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5, 0x80000000 | 5,    4,    8,    9,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt,   11,   12,
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 5,   15,   16,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SIForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SIForm *_t = static_cast<SIForm *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->JudgeIDSignal((*reinterpret_cast< QLineEdit*(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 1: _t->JudgeIDTypeSignal((*reinterpret_cast< QLineEdit*(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2])),(*reinterpret_cast< QString*(*)>(_a[3]))); break;
        case 2: _t->ShowMessageProcessSignal((*reinterpret_cast< const uint(*)>(_a[1])),(*reinterpret_cast< const uint(*)>(_a[2]))); break;
        case 3: _t->SelectDirSignal((*reinterpret_cast< QLabel*(*)>(_a[1])),(*reinterpret_cast< QString*(*)>(_a[2]))); break;
        case 4: _t->on_SIIDEdit_editingFinished(); break;
        case 5: _t->on_SIRelyIDEdit_editingFinished(); break;
        case 6: _t->on_SISVNButton_clicked(); break;
        case 7: _t->on_SIOutputButton_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLineEdit* >(); break;
            }
            break;
        case 1:
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
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLabel* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SIForm::*_t)(QLineEdit * , QString * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIForm::JudgeIDSignal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (SIForm::*_t)(QLineEdit * , QString * , QString * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIForm::JudgeIDTypeSignal)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (SIForm::*_t)(const unsigned int , const unsigned int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIForm::ShowMessageProcessSignal)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (SIForm::*_t)(QLabel * , QString * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SIForm::SelectDirSignal)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject SIForm::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SIForm.data,
      qt_meta_data_SIForm,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SIForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SIForm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SIForm.stringdata0))
        return static_cast<void*>(const_cast< SIForm*>(this));
    return QWidget::qt_metacast(_clname);
}

int SIForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void SIForm::JudgeIDSignal(QLineEdit * _t1, QString * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SIForm::JudgeIDTypeSignal(QLineEdit * _t1, QString * _t2, QString * _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SIForm::ShowMessageProcessSignal(const unsigned int _t1, const unsigned int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SIForm::SelectDirSignal(QLabel * _t1, QString * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
