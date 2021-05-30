/********************************************************************************
** Form generated from reading UI file 'AutomationForm.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTOMATIONFORM_H
#define UI_AUTOMATIONFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AutomationForm
{
public:
    QGridLayout *gridLayout;
    QPushButton *AuFileSearchButton;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QLabel *AuIDLabel;
    QLabel *AuRelyIDLabel;
    QLabel *AuSVNLabel;
    QPushButton *AuSVNButton;
    QPushButton *AuOutputButton;
    QLineEdit *AuIDEdit;
    QLabel *AuOutputLabel;
    QSpacerItem *verticalSpacer;
    QLineEdit *AuRelyIDEdit;
    QSpacerItem *horizontalSpacer;
    QPushButton *AuCreateButton;
    QTableView *AuTableWidget;

    void setupUi(QWidget *AutomationForm)
    {
        if (AutomationForm->objectName().isEmpty())
            AutomationForm->setObjectName(QStringLiteral("AutomationForm"));
        AutomationForm->resize(738, 445);
        gridLayout = new QGridLayout(AutomationForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(10);
        gridLayout->setContentsMargins(5, 5, 5, 5);
        AuFileSearchButton = new QPushButton(AutomationForm);
        AuFileSearchButton->setObjectName(QStringLiteral("AuFileSearchButton"));

        gridLayout->addWidget(AuFileSearchButton, 1, 1, 1, 1);

        widget = new QWidget(AutomationForm);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        widget->setMinimumSize(QSize(350, 0));
        widget->setMaximumSize(QSize(16777215, 550));
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(10);
        gridLayout_2->setVerticalSpacing(20);
        gridLayout_2->setContentsMargins(20, 30, 10, 10);
        AuIDLabel = new QLabel(widget);
        AuIDLabel->setObjectName(QStringLiteral("AuIDLabel"));
        AuIDLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(AuIDLabel, 1, 1, 1, 1);

        AuRelyIDLabel = new QLabel(widget);
        AuRelyIDLabel->setObjectName(QStringLiteral("AuRelyIDLabel"));
        AuRelyIDLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(AuRelyIDLabel, 2, 1, 1, 1);

        AuSVNLabel = new QLabel(widget);
        AuSVNLabel->setObjectName(QStringLiteral("AuSVNLabel"));

        gridLayout_2->addWidget(AuSVNLabel, 4, 2, 1, 1);

        AuSVNButton = new QPushButton(widget);
        AuSVNButton->setObjectName(QStringLiteral("AuSVNButton"));

        gridLayout_2->addWidget(AuSVNButton, 4, 1, 1, 1);

        AuOutputButton = new QPushButton(widget);
        AuOutputButton->setObjectName(QStringLiteral("AuOutputButton"));

        gridLayout_2->addWidget(AuOutputButton, 5, 1, 1, 1);

        AuIDEdit = new QLineEdit(widget);
        AuIDEdit->setObjectName(QStringLiteral("AuIDEdit"));
        AuIDEdit->setMaximumSize(QSize(120, 16777215));

        gridLayout_2->addWidget(AuIDEdit, 1, 2, 1, 1);

        AuOutputLabel = new QLabel(widget);
        AuOutputLabel->setObjectName(QStringLiteral("AuOutputLabel"));

        gridLayout_2->addWidget(AuOutputLabel, 5, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 6, 2, 1, 1);

        AuRelyIDEdit = new QLineEdit(widget);
        AuRelyIDEdit->setObjectName(QStringLiteral("AuRelyIDEdit"));
        AuRelyIDEdit->setMaximumSize(QSize(120, 16777215));

        gridLayout_2->addWidget(AuRelyIDEdit, 2, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 6, 3, 1, 1);


        gridLayout->addWidget(widget, 0, 1, 1, 1);

        AuCreateButton = new QPushButton(AutomationForm);
        AuCreateButton->setObjectName(QStringLiteral("AuCreateButton"));

        gridLayout->addWidget(AuCreateButton, 2, 1, 1, 1);

        AuTableWidget = new QTableView(AutomationForm);
        AuTableWidget->setObjectName(QStringLiteral("AuTableWidget"));

        gridLayout->addWidget(AuTableWidget, 0, 2, 3, 1);


        retranslateUi(AutomationForm);

        QMetaObject::connectSlotsByName(AutomationForm);
    } // setupUi

    void retranslateUi(QWidget *AutomationForm)
    {
        AutomationForm->setWindowTitle(QApplication::translate("AutomationForm", "Form", Q_NULLPTR));
        AuFileSearchButton->setText(QApplication::translate("AutomationForm", "\346\226\207\344\273\266\346\243\200\347\264\242", Q_NULLPTR));
        AuIDLabel->setText(QApplication::translate("AutomationForm", "\346\234\272\347\247\215\347\225\252\345\217\267:", Q_NULLPTR));
        AuRelyIDLabel->setText(QApplication::translate("AutomationForm", "\344\276\235\350\265\226\346\234\272\347\247\215\347\225\252\345\217\267:", Q_NULLPTR));
        AuSVNLabel->setText(QString());
        AuSVNButton->setText(QApplication::translate("AutomationForm", "SVN\350\267\257\345\276\204", Q_NULLPTR));
        AuOutputButton->setText(QApplication::translate("AutomationForm", "\350\276\223\345\207\272\350\267\257\345\276\204", Q_NULLPTR));
        AuOutputLabel->setText(QString());
        AuCreateButton->setText(QApplication::translate("AutomationForm", "\350\207\252\345\212\250\347\224\237\346\210\220", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AutomationForm: public Ui_AutomationForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTOMATIONFORM_H
