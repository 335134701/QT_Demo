/********************************************************************************
** Form generated from reading UI file 'MessageForm.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MESSAGEFORM_H
#define UI_MESSAGEFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MessageForm
{
public:
    QVBoxLayout *verticalLayout;
    QListView *listView;

    void setupUi(QWidget *MessageForm)
    {
        if (MessageForm->objectName().isEmpty())
            MessageForm->setObjectName(QStringLiteral("MessageForm"));
        MessageForm->resize(400, 300);
        verticalLayout = new QVBoxLayout(MessageForm);
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        listView = new QListView(MessageForm);
        listView->setObjectName(QStringLiteral("listView"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listView->sizePolicy().hasHeightForWidth());
        listView->setSizePolicy(sizePolicy);
        listView->setMinimumSize(QSize(0, 0));
        listView->setMaximumSize(QSize(16777215, 16777215));
        listView->setStyleSheet(QStringLiteral("background-color: rgb(85, 255, 0);"));

        verticalLayout->addWidget(listView);


        retranslateUi(MessageForm);

        QMetaObject::connectSlotsByName(MessageForm);
    } // setupUi

    void retranslateUi(QWidget *MessageForm)
    {
        MessageForm->setWindowTitle(QApplication::translate("MessageForm", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MessageForm: public Ui_MessageForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MESSAGEFORM_H
