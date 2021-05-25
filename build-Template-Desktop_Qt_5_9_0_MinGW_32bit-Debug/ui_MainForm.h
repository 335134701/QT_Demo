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
#include "MessageForm.h"
#include "SIForm.h"

QT_BEGIN_NAMESPACE

class Ui_MainForm
{
public:
    QGridLayout *gridLayout;
    MessageForm *widget_2;
    QTabWidget *FunctionWidget;
    SIForm *SI;
    QWidget *tab_2;

    void setupUi(QWidget *MainForm)
    {
        if (MainForm->objectName().isEmpty())
            MainForm->setObjectName(QStringLiteral("MainForm"));
        MainForm->resize(800, 600);
        MainForm->setMinimumSize(QSize(800, 600));
        gridLayout = new QGridLayout(MainForm);
        gridLayout->setSpacing(3);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(6, 6, 6, 6);
        widget_2 = new MessageForm(MainForm);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy);
        widget_2->setMinimumSize(QSize(0, 200));
        widget_2->setMaximumSize(QSize(16777215, 400));
        widget_2->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(widget_2, 1, 0, 1, 1);

        FunctionWidget = new QTabWidget(MainForm);
        FunctionWidget->setObjectName(QStringLiteral("FunctionWidget"));
        SI = new SIForm();
        SI->setObjectName(QStringLiteral("SI"));
        FunctionWidget->addTab(SI, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        FunctionWidget->addTab(tab_2, QString());

        gridLayout->addWidget(FunctionWidget, 0, 0, 1, 1);


        retranslateUi(MainForm);

        QMetaObject::connectSlotsByName(MainForm);
    } // setupUi

    void retranslateUi(QWidget *MainForm)
    {
        MainForm->setWindowTitle(QApplication::translate("MainForm", "Form", Q_NULLPTR));
        FunctionWidget->setTabText(FunctionWidget->indexOf(SI), QApplication::translate("MainForm", "SI", Q_NULLPTR));
        FunctionWidget->setTabText(FunctionWidget->indexOf(tab_2), QApplication::translate("MainForm", "Tab 2", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainForm: public Ui_MainForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFORM_H
