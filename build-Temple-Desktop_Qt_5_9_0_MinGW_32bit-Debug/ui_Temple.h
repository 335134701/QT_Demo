/********************************************************************************
** Form generated from reading UI file 'Temple.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEMPLE_H
#define UI_TEMPLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Temple
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Temple)
    {
        if (Temple->objectName().isEmpty())
            Temple->setObjectName(QStringLiteral("Temple"));
        Temple->resize(400, 300);
        menuBar = new QMenuBar(Temple);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        Temple->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Temple);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Temple->addToolBar(mainToolBar);
        centralWidget = new QWidget(Temple);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        Temple->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(Temple);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Temple->setStatusBar(statusBar);

        retranslateUi(Temple);

        QMetaObject::connectSlotsByName(Temple);
    } // setupUi

    void retranslateUi(QMainWindow *Temple)
    {
        Temple->setWindowTitle(QApplication::translate("Temple", "Temple", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Temple: public Ui_Temple {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEMPLE_H
