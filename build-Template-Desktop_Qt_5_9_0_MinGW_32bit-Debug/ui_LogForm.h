/********************************************************************************
** Form generated from reading UI file 'LogForm.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGFORM_H
#define UI_LOGFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LogForm
{
public:
    QGridLayout *gridLayout;
    QTextEdit *LogView;

    void setupUi(QWidget *LogForm)
    {
        if (LogForm->objectName().isEmpty())
            LogForm->setObjectName(QStringLiteral("LogForm"));
        LogForm->resize(400, 300);
        gridLayout = new QGridLayout(LogForm);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        LogView = new QTextEdit(LogForm);
        LogView->setObjectName(QStringLiteral("LogView"));
        LogView->setFocusPolicy(Qt::StrongFocus);
        LogView->setContextMenuPolicy(Qt::ActionsContextMenu);
        LogView->setLineWrapMode(QTextEdit::WidgetWidth);
        LogView->setReadOnly(true);

        gridLayout->addWidget(LogView, 0, 0, 1, 1);


        retranslateUi(LogForm);

        QMetaObject::connectSlotsByName(LogForm);
    } // setupUi

    void retranslateUi(QWidget *LogForm)
    {
        LogForm->setWindowTitle(QApplication::translate("LogForm", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class LogForm: public Ui_LogForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGFORM_H
