/********************************************************************************
** Form generated from reading UI file 'GTool.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GTOOL_H
#define UI_GTOOL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "FirstForm.h"
#include "LogForm.h"
#include "OtherForm.h"

QT_BEGIN_NAMESPACE

class Ui_GTool
{
public:
    QAction *actionInfo;
    QAction *actionDebug;
    QAction *actionWarn;
    QAction *actionError;
    QAction *actionOtherMessage;
    QAction *actionSingle;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *FunctionWgedit;
    FirstForm *First;
    OtherForm *Other;
    LogForm *Log;
    QMenuBar *menuBar;
    QMenu *menuLog;

    void setupUi(QMainWindow *GTool)
    {
        if (GTool->objectName().isEmpty())
            GTool->setObjectName(QStringLiteral("GTool"));
        GTool->resize(800, 600);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(GTool->sizePolicy().hasHeightForWidth());
        GTool->setSizePolicy(sizePolicy);
        GTool->setMinimumSize(QSize(800, 600));
        actionInfo = new QAction(GTool);
        actionInfo->setObjectName(QStringLiteral("actionInfo"));
        actionInfo->setCheckable(true);
        actionInfo->setChecked(false);
        actionDebug = new QAction(GTool);
        actionDebug->setObjectName(QStringLiteral("actionDebug"));
        actionDebug->setCheckable(true);
        actionDebug->setChecked(false);
        actionWarn = new QAction(GTool);
        actionWarn->setObjectName(QStringLiteral("actionWarn"));
        actionWarn->setCheckable(true);
        actionWarn->setChecked(false);
        actionError = new QAction(GTool);
        actionError->setObjectName(QStringLiteral("actionError"));
        actionError->setCheckable(true);
        actionError->setChecked(false);
        actionOtherMessage = new QAction(GTool);
        actionOtherMessage->setObjectName(QStringLiteral("actionOtherMessage"));
        actionOtherMessage->setCheckable(true);
        actionSingle = new QAction(GTool);
        actionSingle->setObjectName(QStringLiteral("actionSingle"));
        actionSingle->setCheckable(true);
        centralWidget = new QWidget(GTool);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(5);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        FunctionWgedit = new QTabWidget(centralWidget);
        FunctionWgedit->setObjectName(QStringLiteral("FunctionWgedit"));
        First = new FirstForm();
        First->setObjectName(QStringLiteral("First"));
        FunctionWgedit->addTab(First, QString());
        Other = new OtherForm();
        Other->setObjectName(QStringLiteral("Other"));
        FunctionWgedit->addTab(Other, QString());

        verticalLayout->addWidget(FunctionWgedit);

        Log = new LogForm(centralWidget);
        Log->setObjectName(QStringLiteral("Log"));
        Log->setEnabled(true);
        Log->setMinimumSize(QSize(0, 300));

        verticalLayout->addWidget(Log);

        GTool->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(GTool);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 22));
        menuLog = new QMenu(menuBar);
        menuLog->setObjectName(QStringLiteral("menuLog"));
        GTool->setMenuBar(menuBar);

        menuBar->addAction(menuLog->menuAction());
        menuLog->addAction(actionSingle);
        menuLog->addSeparator();
        menuLog->addAction(actionInfo);
        menuLog->addAction(actionWarn);
        menuLog->addAction(actionDebug);
        menuLog->addAction(actionError);
        menuLog->addSeparator();
        menuLog->addAction(actionOtherMessage);

        retranslateUi(GTool);

        FunctionWgedit->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(GTool);
    } // setupUi

    void retranslateUi(QMainWindow *GTool)
    {
        GTool->setWindowTitle(QApplication::translate("GTool", "GTool", Q_NULLPTR));
        actionInfo->setText(QApplication::translate("GTool", "Info", Q_NULLPTR));
        actionDebug->setText(QApplication::translate("GTool", "Debug", Q_NULLPTR));
        actionWarn->setText(QApplication::translate("GTool", "Warn", Q_NULLPTR));
        actionError->setText(QApplication::translate("GTool", "Error", Q_NULLPTR));
        actionOtherMessage->setText(QApplication::translate("GTool", "OtherMessage", Q_NULLPTR));
        actionSingle->setText(QApplication::translate("GTool", "Single", Q_NULLPTR));
        FunctionWgedit->setTabText(FunctionWgedit->indexOf(First), QApplication::translate("GTool", "First", Q_NULLPTR));
        FunctionWgedit->setTabText(FunctionWgedit->indexOf(Other), QApplication::translate("GTool", "Other", Q_NULLPTR));
        menuLog->setTitle(QApplication::translate("GTool", "Log", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GTool: public Ui_GTool {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GTOOL_H
