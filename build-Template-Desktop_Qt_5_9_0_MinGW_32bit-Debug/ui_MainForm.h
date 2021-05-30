/********************************************************************************
** Form generated from reading UI file 'MainForm.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINFORM_H
#define UI_MAINFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "AutomationForm.h"
#include "LogForm.h"
#include "SIForm.h"

QT_BEGIN_NAMESPACE

class Ui_MainForm
{
public:
    QGridLayout *gridLayout;
    LogForm *LogWgedit;
    QTabWidget *FunctionWgedit;
    SIForm *SI;
    AutomationForm *Automation;

    void setupUi(QWidget *MainForm)
    {
        if (MainForm->objectName().isEmpty())
            MainForm->setObjectName(QStringLiteral("MainForm"));
        MainForm->setEnabled(true);
        MainForm->resize(900, 700);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainForm->sizePolicy().hasHeightForWidth());
        MainForm->setSizePolicy(sizePolicy);
        MainForm->setMinimumSize(QSize(900, 700));
        gridLayout = new QGridLayout(MainForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(5);
        gridLayout->setVerticalSpacing(3);
        gridLayout->setContentsMargins(5, 5, 5, 5);
        LogWgedit = new LogForm(MainForm);
        LogWgedit->setObjectName(QStringLiteral("LogWgedit"));
        sizePolicy.setHeightForWidth(LogWgedit->sizePolicy().hasHeightForWidth());
        LogWgedit->setSizePolicy(sizePolicy);
        LogWgedit->setMinimumSize(QSize(0, 150));
        LogWgedit->setMaximumSize(QSize(16777215, 300));

        gridLayout->addWidget(LogWgedit, 1, 0, 1, 1);

        FunctionWgedit = new QTabWidget(MainForm);
        FunctionWgedit->setObjectName(QStringLiteral("FunctionWgedit"));
        FunctionWgedit->setStyleSheet(QStringLiteral(""));
        SI = new SIForm();
        SI->setObjectName(QStringLiteral("SI"));
        FunctionWgedit->addTab(SI, QString());
        Automation = new AutomationForm();
        Automation->setObjectName(QStringLiteral("Automation"));
        FunctionWgedit->addTab(Automation, QString());

        gridLayout->addWidget(FunctionWgedit, 0, 0, 1, 1);


        retranslateUi(MainForm);

        FunctionWgedit->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainForm);
    } // setupUi

    void retranslateUi(QWidget *MainForm)
    {
        MainForm->setWindowTitle(QApplication::translate("MainForm", "Tool", Q_NULLPTR));
        FunctionWgedit->setTabText(FunctionWgedit->indexOf(SI), QApplication::translate("MainForm", "SI\345\267\245\345\205\267", Q_NULLPTR));
        FunctionWgedit->setTabText(FunctionWgedit->indexOf(Automation), QApplication::translate("MainForm", "\345\205\245\346\243\200\350\265\204\346\226\231\350\207\252\345\212\250\345\214\226\345\267\245\345\205\267", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainForm: public Ui_MainForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFORM_H
