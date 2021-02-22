/********************************************************************************
** Form generated from reading UI file 'analogclock.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ANALOGCLOCK_H
#define UI_ANALOGCLOCK_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AnalogClock
{
public:

    void setupUi(QWidget *AnalogClock)
    {
        if (AnalogClock->objectName().isEmpty())
            AnalogClock->setObjectName(QStringLiteral("AnalogClock"));
        AnalogClock->resize(200, 200);
        AnalogClock->setMinimumSize(QSize(200, 200));

        retranslateUi(AnalogClock);

        QMetaObject::connectSlotsByName(AnalogClock);
    } // setupUi

    void retranslateUi(QWidget *AnalogClock)
    {
        AnalogClock->setWindowTitle(QApplication::translate("AnalogClock", "AnalogClock", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AnalogClock: public Ui_AnalogClock {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ANALOGCLOCK_H
