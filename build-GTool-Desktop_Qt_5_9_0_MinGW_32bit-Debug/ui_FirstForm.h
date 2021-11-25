/********************************************************************************
** Form generated from reading UI file 'FirstForm.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FIRSTFORM_H
#define UI_FIRSTFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FirstForm
{
public:

    void setupUi(QWidget *FirstForm)
    {
        if (FirstForm->objectName().isEmpty())
            FirstForm->setObjectName(QStringLiteral("FirstForm"));
        FirstForm->resize(400, 300);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FirstForm->sizePolicy().hasHeightForWidth());
        FirstForm->setSizePolicy(sizePolicy);

        retranslateUi(FirstForm);

        QMetaObject::connectSlotsByName(FirstForm);
    } // setupUi

    void retranslateUi(QWidget *FirstForm)
    {
        FirstForm->setWindowTitle(QApplication::translate("FirstForm", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FirstForm: public Ui_FirstForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FIRSTFORM_H
