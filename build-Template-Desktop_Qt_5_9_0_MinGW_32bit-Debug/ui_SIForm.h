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
    QWidget *SITableWidget;
    QWidget *SIMemuWidget;
    QGridLayout *gridLayout_2;
    QPushButton *OutputButton;
    QPushButton *SVNButton;
    QLabel *IDLabel;
    QLineEdit *IDEdit;
    QLabel *RelyIDLabel;
    QLineEdit *RelyIDEdit;
    QLabel *SVNLabel;
    QSpacerItem *verticalSpacer;
    QLabel *OutputLabel;
    QSpacerItem *horizontalSpacer;
    QPushButton *SIFileCompression;
    QPushButton *SIPretreatmentButton;
    QPushButton *SIFileSearchButton;

    void setupUi(QWidget *SIForm)
    {
        if (SIForm->objectName().isEmpty())
            SIForm->setObjectName(QStringLiteral("SIForm"));
        SIForm->resize(535, 397);
        gridLayout = new QGridLayout(SIForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(10);
        gridLayout->setContentsMargins(5, 5, 5, 5);
        SITableWidget = new QWidget(SIForm);
        SITableWidget->setObjectName(QStringLiteral("SITableWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SITableWidget->sizePolicy().hasHeightForWidth());
        SITableWidget->setSizePolicy(sizePolicy);
        SITableWidget->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(SITableWidget, 0, 1, 6, 1);

        SIMemuWidget = new QWidget(SIForm);
        SIMemuWidget->setObjectName(QStringLiteral("SIMemuWidget"));
        sizePolicy.setHeightForWidth(SIMemuWidget->sizePolicy().hasHeightForWidth());
        SIMemuWidget->setSizePolicy(sizePolicy);
        SIMemuWidget->setMinimumSize(QSize(200, 0));
        SIMemuWidget->setMaximumSize(QSize(400, 16777215));
        gridLayout_2 = new QGridLayout(SIMemuWidget);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(10);
        gridLayout_2->setVerticalSpacing(20);
        gridLayout_2->setContentsMargins(20, 30, 10, 10);
        OutputButton = new QPushButton(SIMemuWidget);
        OutputButton->setObjectName(QStringLiteral("OutputButton"));

        gridLayout_2->addWidget(OutputButton, 3, 0, 1, 1);

        SVNButton = new QPushButton(SIMemuWidget);
        SVNButton->setObjectName(QStringLiteral("SVNButton"));

        gridLayout_2->addWidget(SVNButton, 2, 0, 1, 1);

        IDLabel = new QLabel(SIMemuWidget);
        IDLabel->setObjectName(QStringLiteral("IDLabel"));
        IDLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(IDLabel, 0, 0, 1, 1);

        IDEdit = new QLineEdit(SIMemuWidget);
        IDEdit->setObjectName(QStringLiteral("IDEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(IDEdit->sizePolicy().hasHeightForWidth());
        IDEdit->setSizePolicy(sizePolicy1);
        IDEdit->setMaximumSize(QSize(120, 16777215));
        IDEdit->setStyleSheet(QStringLiteral(""));
        IDEdit->setMaxLength(8);

        gridLayout_2->addWidget(IDEdit, 0, 2, 1, 1);

        RelyIDLabel = new QLabel(SIMemuWidget);
        RelyIDLabel->setObjectName(QStringLiteral("RelyIDLabel"));
        RelyIDLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(RelyIDLabel, 1, 0, 1, 1);

        RelyIDEdit = new QLineEdit(SIMemuWidget);
        RelyIDEdit->setObjectName(QStringLiteral("RelyIDEdit"));
        RelyIDEdit->setMaximumSize(QSize(120, 16777215));
        RelyIDEdit->setMaxLength(8);

        gridLayout_2->addWidget(RelyIDEdit, 1, 2, 1, 1);

        SVNLabel = new QLabel(SIMemuWidget);
        SVNLabel->setObjectName(QStringLiteral("SVNLabel"));

        gridLayout_2->addWidget(SVNLabel, 2, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 4, 0, 1, 1);

        OutputLabel = new QLabel(SIMemuWidget);
        OutputLabel->setObjectName(QStringLiteral("OutputLabel"));

        gridLayout_2->addWidget(OutputLabel, 3, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 2, 3, 1, 1);


        gridLayout->addWidget(SIMemuWidget, 0, 0, 1, 1);

        SIFileCompression = new QPushButton(SIForm);
        SIFileCompression->setObjectName(QStringLiteral("SIFileCompression"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(SIFileCompression->sizePolicy().hasHeightForWidth());
        SIFileCompression->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(SIFileCompression, 4, 0, 1, 1);

        SIPretreatmentButton = new QPushButton(SIForm);
        SIPretreatmentButton->setObjectName(QStringLiteral("SIPretreatmentButton"));
        sizePolicy2.setHeightForWidth(SIPretreatmentButton->sizePolicy().hasHeightForWidth());
        SIPretreatmentButton->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(SIPretreatmentButton, 3, 0, 1, 1);

        SIFileSearchButton = new QPushButton(SIForm);
        SIFileSearchButton->setObjectName(QStringLiteral("SIFileSearchButton"));
        sizePolicy2.setHeightForWidth(SIFileSearchButton->sizePolicy().hasHeightForWidth());
        SIFileSearchButton->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(SIFileSearchButton, 1, 0, 2, 1);


        retranslateUi(SIForm);

        QMetaObject::connectSlotsByName(SIForm);
    } // setupUi

    void retranslateUi(QWidget *SIForm)
    {
        SIForm->setWindowTitle(QApplication::translate("SIForm", "Form", Q_NULLPTR));
        OutputButton->setText(QApplication::translate("SIForm", "\350\276\223\345\207\272\350\267\257\345\276\204", Q_NULLPTR));
        SVNButton->setText(QApplication::translate("SIForm", "SVN\350\267\257\345\276\204", Q_NULLPTR));
        IDLabel->setText(QApplication::translate("SIForm", "\346\234\272\347\247\215\347\225\252\345\217\267:", Q_NULLPTR));
        RelyIDLabel->setText(QApplication::translate("SIForm", "\344\276\235\350\265\226\346\234\272\347\247\215\347\225\252\345\217\267:", Q_NULLPTR));
        SVNLabel->setText(QString());
        OutputLabel->setText(QString());
        SIFileCompression->setText(QApplication::translate("SIForm", "SI\346\226\207\344\273\266\345\216\213\347\274\251", Q_NULLPTR));
        SIPretreatmentButton->setText(QApplication::translate("SIForm", "SI\351\242\204\345\244\204\347\220\206", Q_NULLPTR));
        SIFileSearchButton->setText(QApplication::translate("SIForm", "SI\346\226\207\344\273\266\346\243\200\347\264\242", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SIForm: public Ui_SIForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIFORM_H
