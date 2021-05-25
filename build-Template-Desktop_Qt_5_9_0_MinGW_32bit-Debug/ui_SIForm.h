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
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SIForm
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;

    void setupUi(QWidget *SIForm)
    {
        if (SIForm->objectName().isEmpty())
            SIForm->setObjectName(QStringLiteral("SIForm"));
        SIForm->resize(400, 300);
        gridLayout = new QGridLayout(SIForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        frame = new QFrame(SIForm);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(SIForm);

        QMetaObject::connectSlotsByName(SIForm);
    } // setupUi

    void retranslateUi(QWidget *SIForm)
    {
        SIForm->setWindowTitle(QApplication::translate("SIForm", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SIForm: public Ui_SIForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIFORM_H
