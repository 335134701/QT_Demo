/********************************************************************************
** Form generated from reading UI file 'AutomationForm.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTOMATIONFORM_H
#define UI_AUTOMATIONFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AutomationForm
{
public:

    void setupUi(QWidget *AutomationForm)
    {
        if (AutomationForm->objectName().isEmpty())
            AutomationForm->setObjectName(QStringLiteral("AutomationForm"));
        AutomationForm->resize(400, 300);

        retranslateUi(AutomationForm);

        QMetaObject::connectSlotsByName(AutomationForm);
    } // setupUi

    void retranslateUi(QWidget *AutomationForm)
    {
        AutomationForm->setWindowTitle(QApplication::translate("AutomationForm", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AutomationForm: public Ui_AutomationForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTOMATIONFORM_H
