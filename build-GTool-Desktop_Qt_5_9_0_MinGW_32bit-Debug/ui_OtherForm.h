/********************************************************************************
** Form generated from reading UI file 'OtherForm.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OTHERFORM_H
#define UI_OTHERFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OtherForm
{
public:
    QPushButton *pushButton;

    void setupUi(QWidget *OtherForm)
    {
        if (OtherForm->objectName().isEmpty())
            OtherForm->setObjectName(QStringLiteral("OtherForm"));
        OtherForm->resize(400, 300);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(OtherForm->sizePolicy().hasHeightForWidth());
        OtherForm->setSizePolicy(sizePolicy);
        OtherForm->setStyleSheet(QStringLiteral(""));
        pushButton = new QPushButton(OtherForm);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(150, 160, 89, 24));

        retranslateUi(OtherForm);

        QMetaObject::connectSlotsByName(OtherForm);
    } // setupUi

    void retranslateUi(QWidget *OtherForm)
    {
        OtherForm->setWindowTitle(QApplication::translate("OtherForm", "Form", Q_NULLPTR));
        pushButton->setText(QApplication::translate("OtherForm", "PushButton", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class OtherForm: public Ui_OtherForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OTHERFORM_H
