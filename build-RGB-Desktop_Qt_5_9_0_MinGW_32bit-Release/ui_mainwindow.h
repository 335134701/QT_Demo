/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *RED_Widget;
    QGridLayout *gridLayout;
    QLabel *RED_label;
    QSpinBox *RED_spinBox;
    QSlider *RED_verticalSlider;
    QWidget *GREEN_Widget;
    QGridLayout *gridLayout_4;
    QLabel *GREEN_label;
    QSpinBox *GREEN_spinBox;
    QSlider *GREEN_verticalSlider;
    QWidget *BLUE_Widget;
    QGridLayout *gridLayout_5;
    QLabel *BLUE_label;
    QSpinBox *BLUE_spinBox;
    QSlider *BLUE_verticalSlider;
    QStatusBar *statusBar;
    QMenuBar *menuBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(274, 275);
        MainWindow->setMinimumSize(QSize(200, 200));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        RED_Widget = new QWidget(centralWidget);
        RED_Widget->setObjectName(QStringLiteral("RED_Widget"));
        RED_Widget->setAutoFillBackground(false);
        gridLayout = new QGridLayout(RED_Widget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        RED_label = new QLabel(RED_Widget);
        RED_label->setObjectName(QStringLiteral("RED_label"));

        gridLayout->addWidget(RED_label, 0, 0, 1, 1);

        RED_spinBox = new QSpinBox(RED_Widget);
        RED_spinBox->setObjectName(QStringLiteral("RED_spinBox"));
        RED_spinBox->setMaximum(255);

        gridLayout->addWidget(RED_spinBox, 1, 0, 1, 1);

        RED_verticalSlider = new QSlider(RED_Widget);
        RED_verticalSlider->setObjectName(QStringLiteral("RED_verticalSlider"));
        RED_verticalSlider->setOrientation(Qt::Vertical);

        gridLayout->addWidget(RED_verticalSlider, 2, 0, 1, 1);


        horizontalLayout->addWidget(RED_Widget);

        GREEN_Widget = new QWidget(centralWidget);
        GREEN_Widget->setObjectName(QStringLiteral("GREEN_Widget"));
        GREEN_Widget->setAutoFillBackground(false);
        gridLayout_4 = new QGridLayout(GREEN_Widget);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        GREEN_label = new QLabel(GREEN_Widget);
        GREEN_label->setObjectName(QStringLiteral("GREEN_label"));

        gridLayout_4->addWidget(GREEN_label, 0, 0, 1, 1);

        GREEN_spinBox = new QSpinBox(GREEN_Widget);
        GREEN_spinBox->setObjectName(QStringLiteral("GREEN_spinBox"));
        GREEN_spinBox->setMaximum(255);

        gridLayout_4->addWidget(GREEN_spinBox, 1, 0, 1, 1);

        GREEN_verticalSlider = new QSlider(GREEN_Widget);
        GREEN_verticalSlider->setObjectName(QStringLiteral("GREEN_verticalSlider"));
        GREEN_verticalSlider->setOrientation(Qt::Vertical);

        gridLayout_4->addWidget(GREEN_verticalSlider, 2, 0, 1, 1);


        horizontalLayout->addWidget(GREEN_Widget);

        BLUE_Widget = new QWidget(centralWidget);
        BLUE_Widget->setObjectName(QStringLiteral("BLUE_Widget"));
        BLUE_Widget->setAutoFillBackground(false);
        gridLayout_5 = new QGridLayout(BLUE_Widget);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        BLUE_label = new QLabel(BLUE_Widget);
        BLUE_label->setObjectName(QStringLiteral("BLUE_label"));

        gridLayout_5->addWidget(BLUE_label, 0, 0, 1, 1);

        BLUE_spinBox = new QSpinBox(BLUE_Widget);
        BLUE_spinBox->setObjectName(QStringLiteral("BLUE_spinBox"));
        BLUE_spinBox->setMaximum(255);

        gridLayout_5->addWidget(BLUE_spinBox, 1, 0, 1, 1);

        BLUE_verticalSlider = new QSlider(BLUE_Widget);
        BLUE_verticalSlider->setObjectName(QStringLiteral("BLUE_verticalSlider"));
        BLUE_verticalSlider->setOrientation(Qt::Vertical);

        gridLayout_5->addWidget(BLUE_verticalSlider, 2, 0, 1, 1);


        horizontalLayout->addWidget(BLUE_Widget);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 274, 22));
        MainWindow->setMenuBar(menuBar);

        retranslateUi(MainWindow);
        QObject::connect(RED_spinBox, SIGNAL(valueChanged(int)), RED_verticalSlider, SLOT(setValue(int)));
        QObject::connect(BLUE_spinBox, SIGNAL(valueChanged(int)), BLUE_verticalSlider, SLOT(setValue(int)));
        QObject::connect(GREEN_spinBox, SIGNAL(valueChanged(int)), GREEN_verticalSlider, SLOT(setValue(int)));
        QObject::connect(BLUE_verticalSlider, SIGNAL(valueChanged(int)), BLUE_spinBox, SLOT(setValue(int)));
        QObject::connect(GREEN_verticalSlider, SIGNAL(valueChanged(int)), GREEN_spinBox, SLOT(setValue(int)));
        QObject::connect(RED_verticalSlider, SIGNAL(valueChanged(int)), RED_spinBox, SLOT(setValue(int)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        RED_label->setText(QApplication::translate("MainWindow", "RED", Q_NULLPTR));
        GREEN_label->setText(QApplication::translate("MainWindow", "GREEN", Q_NULLPTR));
        BLUE_label->setText(QApplication::translate("MainWindow", "BLUE", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
