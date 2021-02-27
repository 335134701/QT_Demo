/********************************************************************************
** Form generated from reading UI file 'Qlogger.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QLOGGER_H
#define UI_QLOGGER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Qlogger
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QPushButton *InfoButton;
    QPushButton *WarnButton;
    QPushButton *DebugButton;
    QPushButton *ErrorButton;
    QTextEdit *textEdit;

    void setupUi(QMainWindow *Qlogger)
    {
        if (Qlogger->objectName().isEmpty())
            Qlogger->setObjectName(QStringLiteral("Qlogger"));
        Qlogger->resize(400, 300);
        QIcon icon;
        icon.addFile(QStringLiteral(":/ICO/QLog.ico"), QSize(), QIcon::Normal, QIcon::Off);
        Qlogger->setWindowIcon(icon);
        centralWidget = new QWidget(Qlogger);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout_3 = new QHBoxLayout(centralWidget);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        InfoButton = new QPushButton(centralWidget);
        InfoButton->setObjectName(QStringLiteral("InfoButton"));

        verticalLayout->addWidget(InfoButton);

        WarnButton = new QPushButton(centralWidget);
        WarnButton->setObjectName(QStringLiteral("WarnButton"));

        verticalLayout->addWidget(WarnButton);

        DebugButton = new QPushButton(centralWidget);
        DebugButton->setObjectName(QStringLiteral("DebugButton"));

        verticalLayout->addWidget(DebugButton);

        ErrorButton = new QPushButton(centralWidget);
        ErrorButton->setObjectName(QStringLiteral("ErrorButton"));

        verticalLayout->addWidget(ErrorButton);


        horizontalLayout_2->addLayout(verticalLayout);

        textEdit = new QTextEdit(centralWidget);
        textEdit->setObjectName(QStringLiteral("textEdit"));

        horizontalLayout_2->addWidget(textEdit);


        horizontalLayout_3->addLayout(horizontalLayout_2);

        Qlogger->setCentralWidget(centralWidget);

        retranslateUi(Qlogger);

        QMetaObject::connectSlotsByName(Qlogger);
    } // setupUi

    void retranslateUi(QMainWindow *Qlogger)
    {
        Qlogger->setWindowTitle(QApplication::translate("Qlogger", "Qlogger\346\265\213\350\257\225\347\225\214\351\235\242", Q_NULLPTR));
        InfoButton->setText(QApplication::translate("Qlogger", "Info", Q_NULLPTR));
        WarnButton->setText(QApplication::translate("Qlogger", "Warn", Q_NULLPTR));
        DebugButton->setText(QApplication::translate("Qlogger", "Debug", Q_NULLPTR));
        ErrorButton->setText(QApplication::translate("Qlogger", "Error", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Qlogger: public Ui_Qlogger {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QLOGGER_H
