/********************************************************************************
** Form generated from reading UI file 'textfind.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTFIND_H
#define UI_TEXTFIND_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TextFind
{
public:

    void setupUi(QWidget *TextFind)
    {
        if (TextFind->objectName().isEmpty())
            TextFind->setObjectName(QStringLiteral("TextFind"));
        TextFind->resize(400, 300);

        retranslateUi(TextFind);

        QMetaObject::connectSlotsByName(TextFind);
    } // setupUi

    void retranslateUi(QWidget *TextFind)
    {
        TextFind->setWindowTitle(QApplication::translate("TextFind", "TextFind", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TextFind: public Ui_TextFind {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTFIND_H
