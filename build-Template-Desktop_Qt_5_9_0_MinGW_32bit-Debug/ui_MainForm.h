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
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "MessageForm.h"
#include "SIForm.h"

QT_BEGIN_NAMESPACE

class Ui_MainForm
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    SIForm *tab;
    QWidget *tab_2;
    MessageForm *widget;

    void setupUi(QWidget *MainForm)
    {
        if (MainForm->objectName().isEmpty())
            MainForm->setObjectName(QStringLiteral("MainForm"));
        MainForm->resize(779, 439);
        verticalLayout = new QVBoxLayout(MainForm);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(MainForm);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new SIForm();
        tab->setObjectName(QStringLiteral("tab"));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);

        widget = new MessageForm(MainForm);
        widget->setObjectName(QStringLiteral("widget"));

        verticalLayout->addWidget(widget);


        retranslateUi(MainForm);

        QMetaObject::connectSlotsByName(MainForm);
    } // setupUi

    void retranslateUi(QWidget *MainForm)
    {
        MainForm->setWindowTitle(QApplication::translate("MainForm", "Form", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainForm", "Tab 1", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainForm", "Tab 2", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainForm: public Ui_MainForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFORM_H
