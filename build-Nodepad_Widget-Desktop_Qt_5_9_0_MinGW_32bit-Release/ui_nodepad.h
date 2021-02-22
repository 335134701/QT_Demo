/********************************************************************************
** Form generated from reading UI file 'nodepad.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NODEPAD_H
#define UI_NODEPAD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Nodepad
{
public:
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionPrint;
    QAction *actionExit;
    QAction *actionNew_1;
    QAction *actionSave_1;
    QAction *actionSaveAs_1;
    QAction *actionPrint_1;
    QAction *actionExit_1;
    QAction *actionOpen_1;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuFile;

    void setupUi(QMainWindow *Nodepad)
    {
        if (Nodepad->objectName().isEmpty())
            Nodepad->setObjectName(QStringLiteral("Nodepad"));
        Nodepad->resize(400, 300);
        Nodepad->setMinimumSize(QSize(300, 300));
        QIcon icon;
        icon.addFile(QStringLiteral(":/ICO/images/TXT.ico"), QSize(), QIcon::Normal, QIcon::Off);
        Nodepad->setWindowIcon(icon);
        actionNew = new QAction(Nodepad);
        actionNew->setObjectName(QStringLiteral("actionNew"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/ICO/images/new.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew->setIcon(icon1);
        actionOpen = new QAction(Nodepad);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/ICO/images/open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon2);
        actionSave = new QAction(Nodepad);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/ICO/images/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon3);
        actionSaveAs = new QAction(Nodepad);
        actionSaveAs->setObjectName(QStringLiteral("actionSaveAs"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/ICO/images/saveas.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSaveAs->setIcon(icon4);
        actionPrint = new QAction(Nodepad);
        actionPrint->setObjectName(QStringLiteral("actionPrint"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/ICO/images/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPrint->setIcon(icon5);
        actionExit = new QAction(Nodepad);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/ICO/images/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon6);
        actionNew_1 = new QAction(Nodepad);
        actionNew_1->setObjectName(QStringLiteral("actionNew_1"));
        actionNew_1->setIcon(icon1);
        actionSave_1 = new QAction(Nodepad);
        actionSave_1->setObjectName(QStringLiteral("actionSave_1"));
        actionSave_1->setIcon(icon3);
        actionSaveAs_1 = new QAction(Nodepad);
        actionSaveAs_1->setObjectName(QStringLiteral("actionSaveAs_1"));
        actionSaveAs_1->setIcon(icon4);
        actionPrint_1 = new QAction(Nodepad);
        actionPrint_1->setObjectName(QStringLiteral("actionPrint_1"));
        actionPrint_1->setIcon(icon5);
        actionExit_1 = new QAction(Nodepad);
        actionExit_1->setObjectName(QStringLiteral("actionExit_1"));
        actionExit_1->setIcon(icon6);
        actionOpen_1 = new QAction(Nodepad);
        actionOpen_1->setObjectName(QStringLiteral("actionOpen_1"));
        actionOpen_1->setIcon(icon2);
        centralWidget = new QWidget(Nodepad);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        textEdit = new QTextEdit(centralWidget);
        textEdit->setObjectName(QStringLiteral("textEdit"));

        verticalLayout->addWidget(textEdit);

        Nodepad->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(Nodepad);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Nodepad->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Nodepad);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Nodepad->setStatusBar(statusBar);
        menuBar = new QMenuBar(Nodepad);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        Nodepad->setMenuBar(menuBar);

        mainToolBar->addAction(actionNew);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionSave);
        mainToolBar->addAction(actionSaveAs);
        mainToolBar->addAction(actionPrint);
        mainToolBar->addAction(actionExit);
        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionNew_1);
        menuFile->addAction(actionOpen_1);
        menuFile->addAction(actionSave_1);
        menuFile->addAction(actionSaveAs_1);
        menuFile->addSeparator();
        menuFile->addAction(actionPrint_1);
        menuFile->addSeparator();
        menuFile->addAction(actionExit_1);

        retranslateUi(Nodepad);
        QObject::connect(actionNew_1, SIGNAL(triggered()), actionNew, SLOT(trigger()));
        QObject::connect(actionOpen_1, SIGNAL(triggered()), actionOpen, SLOT(trigger()));
        QObject::connect(actionPrint_1, SIGNAL(triggered()), actionPrint, SLOT(trigger()));
        QObject::connect(actionSave_1, SIGNAL(triggered()), actionSave, SLOT(trigger()));
        QObject::connect(actionSaveAs_1, SIGNAL(triggered()), actionSaveAs, SLOT(trigger()));
        QObject::connect(actionPrint_1, SIGNAL(triggered()), actionPrint, SLOT(trigger()));

        QMetaObject::connectSlotsByName(Nodepad);
    } // setupUi

    void retranslateUi(QMainWindow *Nodepad)
    {
        Nodepad->setWindowTitle(QApplication::translate("Nodepad", "Nodepad", Q_NULLPTR));
        actionNew->setText(QApplication::translate("Nodepad", "New", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("Nodepad", "Open", Q_NULLPTR));
        actionSave->setText(QApplication::translate("Nodepad", "Save", Q_NULLPTR));
        actionSaveAs->setText(QApplication::translate("Nodepad", "Save as", Q_NULLPTR));
        actionPrint->setText(QApplication::translate("Nodepad", "Print", Q_NULLPTR));
        actionExit->setText(QApplication::translate("Nodepad", "Exit", Q_NULLPTR));
        actionNew_1->setText(QApplication::translate("Nodepad", "New", Q_NULLPTR));
        actionSave_1->setText(QApplication::translate("Nodepad", "Save", Q_NULLPTR));
        actionSaveAs_1->setText(QApplication::translate("Nodepad", "SaveAs", Q_NULLPTR));
        actionPrint_1->setText(QApplication::translate("Nodepad", "Print", Q_NULLPTR));
        actionExit_1->setText(QApplication::translate("Nodepad", "Exit", Q_NULLPTR));
        actionOpen_1->setText(QApplication::translate("Nodepad", "Open", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("Nodepad", "File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Nodepad: public Ui_Nodepad {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NODEPAD_H
