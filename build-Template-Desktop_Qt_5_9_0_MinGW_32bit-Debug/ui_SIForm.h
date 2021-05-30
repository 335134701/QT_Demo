/********************************************************************************
** Form generated from reading UI file 'SIForm.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIFORM_H
#define UI_SIFORM_H

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
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SIForm
{
public:
    QGridLayout *gridLayout;
    QPushButton *SIPretreatmentButton;
    QPushButton *SIFileCompressionButton;
    QPushButton *SIFileSearchButton;
    QWidget *SIMemuWidget;
    QGridLayout *gridLayout_2;
    QPushButton *SIOutputButton;
    QPushButton *SISVNButton;
    QLabel *SIIDLabel;
    QLineEdit *SIIDEdit;
    QLabel *SIRelyIDLabel;
    QLineEdit *SIRelyIDEdit;
    QLabel *SISVNLabel;
    QSpacerItem *SIverticalSpacer;
    QLabel *SIOutputLabel;
    QSpacerItem *SIhorizontalSpacer;
    QWidget *SITableWidget;

    void setupUi(QWidget *SIForm)
    {
        if (SIForm->objectName().isEmpty())
            SIForm->setObjectName(QStringLiteral("SIForm"));
        SIForm->resize(767, 460);
        gridLayout = new QGridLayout(SIForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(10);
        gridLayout->setContentsMargins(5, 5, 5, 5);
        SIPretreatmentButton = new QPushButton(SIForm);
        SIPretreatmentButton->setObjectName(QStringLiteral("SIPretreatmentButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SIPretreatmentButton->sizePolicy().hasHeightForWidth());
        SIPretreatmentButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(SIPretreatmentButton, 3, 0, 1, 1);

        SIFileCompressionButton = new QPushButton(SIForm);
        SIFileCompressionButton->setObjectName(QStringLiteral("SIFileCompressionButton"));
        sizePolicy.setHeightForWidth(SIFileCompressionButton->sizePolicy().hasHeightForWidth());
        SIFileCompressionButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(SIFileCompressionButton, 4, 0, 1, 1);

        SIFileSearchButton = new QPushButton(SIForm);
        SIFileSearchButton->setObjectName(QStringLiteral("SIFileSearchButton"));
        sizePolicy.setHeightForWidth(SIFileSearchButton->sizePolicy().hasHeightForWidth());
        SIFileSearchButton->setSizePolicy(sizePolicy);

        gridLayout->addWidget(SIFileSearchButton, 1, 0, 2, 1);

        SIMemuWidget = new QWidget(SIForm);
        SIMemuWidget->setObjectName(QStringLiteral("SIMemuWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(SIMemuWidget->sizePolicy().hasHeightForWidth());
        SIMemuWidget->setSizePolicy(sizePolicy1);
        SIMemuWidget->setMinimumSize(QSize(350, 0));
        SIMemuWidget->setMaximumSize(QSize(550, 16777215));
        gridLayout_2 = new QGridLayout(SIMemuWidget);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(10);
        gridLayout_2->setVerticalSpacing(20);
        gridLayout_2->setContentsMargins(20, 30, 10, 10);
        SIOutputButton = new QPushButton(SIMemuWidget);
        SIOutputButton->setObjectName(QStringLiteral("SIOutputButton"));

        gridLayout_2->addWidget(SIOutputButton, 3, 0, 1, 1);

        SISVNButton = new QPushButton(SIMemuWidget);
        SISVNButton->setObjectName(QStringLiteral("SISVNButton"));

        gridLayout_2->addWidget(SISVNButton, 2, 0, 1, 1);

        SIIDLabel = new QLabel(SIMemuWidget);
        SIIDLabel->setObjectName(QStringLiteral("SIIDLabel"));
        SIIDLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(SIIDLabel, 0, 0, 1, 1);

        SIIDEdit = new QLineEdit(SIMemuWidget);
        SIIDEdit->setObjectName(QStringLiteral("SIIDEdit"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(SIIDEdit->sizePolicy().hasHeightForWidth());
        SIIDEdit->setSizePolicy(sizePolicy2);
        SIIDEdit->setMaximumSize(QSize(120, 16777215));
        SIIDEdit->setStyleSheet(QStringLiteral(""));
        SIIDEdit->setMaxLength(8);

        gridLayout_2->addWidget(SIIDEdit, 0, 2, 1, 1);

        SIRelyIDLabel = new QLabel(SIMemuWidget);
        SIRelyIDLabel->setObjectName(QStringLiteral("SIRelyIDLabel"));
        SIRelyIDLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(SIRelyIDLabel, 1, 0, 1, 1);

        SIRelyIDEdit = new QLineEdit(SIMemuWidget);
        SIRelyIDEdit->setObjectName(QStringLiteral("SIRelyIDEdit"));
        SIRelyIDEdit->setMaximumSize(QSize(120, 16777215));
        SIRelyIDEdit->setMaxLength(8);

        gridLayout_2->addWidget(SIRelyIDEdit, 1, 2, 1, 1);

        SISVNLabel = new QLabel(SIMemuWidget);
        SISVNLabel->setObjectName(QStringLiteral("SISVNLabel"));

        gridLayout_2->addWidget(SISVNLabel, 2, 2, 1, 1);

        SIverticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(SIverticalSpacer, 4, 0, 1, 1);

        SIOutputLabel = new QLabel(SIMemuWidget);
        SIOutputLabel->setObjectName(QStringLiteral("SIOutputLabel"));

        gridLayout_2->addWidget(SIOutputLabel, 3, 2, 1, 1);

        SIhorizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(SIhorizontalSpacer, 2, 3, 1, 1);


        gridLayout->addWidget(SIMemuWidget, 0, 0, 1, 1);

        SITableWidget = new QWidget(SIForm);
        SITableWidget->setObjectName(QStringLiteral("SITableWidget"));
        sizePolicy1.setHeightForWidth(SITableWidget->sizePolicy().hasHeightForWidth());
        SITableWidget->setSizePolicy(sizePolicy1);
        SITableWidget->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(SITableWidget, 0, 1, 6, 1);


        retranslateUi(SIForm);

        QMetaObject::connectSlotsByName(SIForm);
    } // setupUi

    void retranslateUi(QWidget *SIForm)
    {
        SIForm->setWindowTitle(QApplication::translate("SIForm", "Form", Q_NULLPTR));
        SIPretreatmentButton->setText(QApplication::translate("SIForm", "SI\351\242\204\345\244\204\347\220\206", Q_NULLPTR));
        SIFileCompressionButton->setText(QApplication::translate("SIForm", "SI\346\226\207\344\273\266\345\216\213\347\274\251", Q_NULLPTR));
        SIFileSearchButton->setText(QApplication::translate("SIForm", "SI\346\226\207\344\273\266\346\243\200\347\264\242", Q_NULLPTR));
        SIOutputButton->setText(QApplication::translate("SIForm", "\350\276\223\345\207\272\350\267\257\345\276\204", Q_NULLPTR));
        SISVNButton->setText(QApplication::translate("SIForm", "SVN\350\267\257\345\276\204", Q_NULLPTR));
        SIIDLabel->setText(QApplication::translate("SIForm", "\346\234\272\347\247\215\347\225\252\345\217\267:", Q_NULLPTR));
        SIRelyIDLabel->setText(QApplication::translate("SIForm", "\344\276\235\350\265\226\346\234\272\347\247\215\347\225\252\345\217\267:", Q_NULLPTR));
        SISVNLabel->setText(QString());
        SIOutputLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SIForm: public Ui_SIForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIFORM_H
