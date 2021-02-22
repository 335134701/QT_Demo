/********************************************************************************
** Form generated from reading UI file 'downloadts.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOADTS_H
#define UI_DOWNLOADTS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DownloadTS
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_6;
    QLabel *LPathName;
    QLabel *currentFilePath;
    QPushButton *IFileButton;
    QListView *URLList;
    QListView *DownloadList;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_4;
    QLabel *LRateName;
    QLabel *LRate;
    QHBoxLayout *horizontalLayout;
    QLabel *LDownStatusName;
    QLabel *LDownStatus;
    QHBoxLayout *horizontalLayout_2;
    QLabel *LCDownloadName;
    QLabel *LCDownload;
    QHBoxLayout *horizontalLayout_3;
    QLabel *LScheduleName;
    QProgressBar *LScheduleBar;
    QListView *FinishDownloadList;
    QGridLayout *gridLayout;
    QPushButton *StartDownload;
    QPushButton *PauseDownload;
    QPushButton *StopDownload;
    QPushButton *DirDownload;

    void setupUi(QWidget *DownloadTS)
    {
        if (DownloadTS->objectName().isEmpty())
            DownloadTS->setObjectName(QStringLiteral("DownloadTS"));
        DownloadTS->resize(500, 500);
        DownloadTS->setMinimumSize(QSize(500, 500));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        DownloadTS->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Images/Images/Download.ico"), QSize(), QIcon::Normal, QIcon::Off);
        DownloadTS->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(DownloadTS);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        LPathName = new QLabel(DownloadTS);
        LPathName->setObjectName(QStringLiteral("LPathName"));
        LPathName->setMinimumSize(QSize(51, 30));
        LPathName->setMaximumSize(QSize(51, 30));
        LPathName->setFont(font);
        LPathName->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_6->addWidget(LPathName);

        currentFilePath = new QLabel(DownloadTS);
        currentFilePath->setObjectName(QStringLiteral("currentFilePath"));
        QFont font1;
        font1.setFamily(QStringLiteral("Consolas"));
        font1.setPointSize(12);
        font1.setBold(false);
        font1.setWeight(50);
        currentFilePath->setFont(font1);

        horizontalLayout_6->addWidget(currentFilePath);

        IFileButton = new QPushButton(DownloadTS);
        IFileButton->setObjectName(QStringLiteral("IFileButton"));
        IFileButton->setMinimumSize(QSize(120, 30));
        IFileButton->setMaximumSize(QSize(120, 30));
        IFileButton->setFont(font);

        horizontalLayout_6->addWidget(IFileButton);


        verticalLayout->addLayout(horizontalLayout_6);

        URLList = new QListView(DownloadTS);
        URLList->setObjectName(QStringLiteral("URLList"));
        URLList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        URLList->setSelectionMode(QAbstractItemView::ExtendedSelection);

        verticalLayout->addWidget(URLList);

        DownloadList = new QListView(DownloadTS);
        DownloadList->setObjectName(QStringLiteral("DownloadList"));
        DownloadList->setMaximumSize(QSize(16777215, 150));
        DownloadList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        DownloadList->setSelectionMode(QAbstractItemView::NoSelection);

        verticalLayout->addWidget(DownloadList);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        LRateName = new QLabel(DownloadTS);
        LRateName->setObjectName(QStringLiteral("LRateName"));
        LRateName->setMinimumSize(QSize(80, 30));
        LRateName->setMaximumSize(QSize(80, 30));
        LRateName->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(LRateName);

        LRate = new QLabel(DownloadTS);
        LRate->setObjectName(QStringLiteral("LRate"));
        LRate->setMinimumSize(QSize(0, 30));
        LRate->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_4->addWidget(LRate);


        horizontalLayout_5->addLayout(horizontalLayout_4);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        LDownStatusName = new QLabel(DownloadTS);
        LDownStatusName->setObjectName(QStringLiteral("LDownStatusName"));
        LDownStatusName->setMinimumSize(QSize(80, 30));
        LDownStatusName->setMaximumSize(QSize(80, 30));
        LDownStatusName->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(LDownStatusName);

        LDownStatus = new QLabel(DownloadTS);
        LDownStatus->setObjectName(QStringLiteral("LDownStatus"));
        LDownStatus->setMinimumSize(QSize(0, 30));
        LDownStatus->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(LDownStatus);


        horizontalLayout_5->addLayout(horizontalLayout);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        LCDownloadName = new QLabel(DownloadTS);
        LCDownloadName->setObjectName(QStringLiteral("LCDownloadName"));
        LCDownloadName->setMinimumSize(QSize(80, 30));
        LCDownloadName->setMaximumSize(QSize(80, 30));
        LCDownloadName->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(LCDownloadName);

        LCDownload = new QLabel(DownloadTS);
        LCDownload->setObjectName(QStringLiteral("LCDownload"));

        horizontalLayout_2->addWidget(LCDownload);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        LScheduleName = new QLabel(DownloadTS);
        LScheduleName->setObjectName(QStringLiteral("LScheduleName"));
        LScheduleName->setMinimumSize(QSize(80, 30));
        LScheduleName->setMaximumSize(QSize(80, 30));
        LScheduleName->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(LScheduleName);

        LScheduleBar = new QProgressBar(DownloadTS);
        LScheduleBar->setObjectName(QStringLiteral("LScheduleBar"));
        LScheduleBar->setMinimumSize(QSize(0, 30));
        LScheduleBar->setMaximumSize(QSize(16777215, 16777215));
        LScheduleBar->setValue(0);

        horizontalLayout_3->addWidget(LScheduleBar);


        verticalLayout->addLayout(horizontalLayout_3);

        FinishDownloadList = new QListView(DownloadTS);
        FinishDownloadList->setObjectName(QStringLiteral("FinishDownloadList"));
        FinishDownloadList->setMaximumSize(QSize(16777215, 120));
        FinishDownloadList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        FinishDownloadList->setSelectionMode(QAbstractItemView::NoSelection);

        verticalLayout->addWidget(FinishDownloadList);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        StartDownload = new QPushButton(DownloadTS);
        StartDownload->setObjectName(QStringLiteral("StartDownload"));
        StartDownload->setMinimumSize(QSize(120, 30));
        StartDownload->setMaximumSize(QSize(16777215, 16777215));
        StartDownload->setFont(font);

        gridLayout->addWidget(StartDownload, 0, 0, 1, 1);

        PauseDownload = new QPushButton(DownloadTS);
        PauseDownload->setObjectName(QStringLiteral("PauseDownload"));
        PauseDownload->setMinimumSize(QSize(120, 30));
        PauseDownload->setMaximumSize(QSize(16777215, 16777215));
        PauseDownload->setFont(font);

        gridLayout->addWidget(PauseDownload, 0, 1, 1, 1);

        StopDownload = new QPushButton(DownloadTS);
        StopDownload->setObjectName(QStringLiteral("StopDownload"));
        StopDownload->setMinimumSize(QSize(120, 30));
        StopDownload->setMaximumSize(QSize(16777215, 16777215));
        StopDownload->setFont(font);

        gridLayout->addWidget(StopDownload, 1, 0, 1, 1);

        DirDownload = new QPushButton(DownloadTS);
        DirDownload->setObjectName(QStringLiteral("DirDownload"));
        DirDownload->setMinimumSize(QSize(120, 30));
        DirDownload->setMaximumSize(QSize(16777215, 16777215));
        DirDownload->setFont(font);

        gridLayout->addWidget(DirDownload, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);


        retranslateUi(DownloadTS);

        QMetaObject::connectSlotsByName(DownloadTS);
    } // setupUi

    void retranslateUi(QWidget *DownloadTS)
    {
        DownloadTS->setWindowTitle(QApplication::translate("DownloadTS", "DownloadTS", Q_NULLPTR));
        LPathName->setText(QApplication::translate("DownloadTS", "\350\267\257\345\276\204\357\274\232", Q_NULLPTR));
        currentFilePath->setText(QString());
        IFileButton->setText(QApplication::translate("DownloadTS", "\345\257\274\345\205\245\346\226\207\344\273\266", Q_NULLPTR));
        LRateName->setText(QApplication::translate("DownloadTS", "\344\270\213\350\275\275\351\200\237\345\272\246\357\274\232", Q_NULLPTR));
        LRate->setText(QString());
        LDownStatusName->setText(QApplication::translate("DownloadTS", "\344\270\213\350\275\275\347\212\266\346\200\201\357\274\232", Q_NULLPTR));
        LDownStatus->setText(QApplication::translate("DownloadTS", "0/0", Q_NULLPTR));
        LCDownloadName->setText(QApplication::translate("DownloadTS", "\346\255\243\345\234\250\344\270\213\350\275\275\357\274\232", Q_NULLPTR));
        LCDownload->setText(QString());
        LScheduleName->setText(QApplication::translate("DownloadTS", "\344\270\213\350\275\275\350\277\233\345\272\246\357\274\232", Q_NULLPTR));
        StartDownload->setText(QApplication::translate("DownloadTS", "\345\274\200\345\247\213\344\270\213\350\275\275", Q_NULLPTR));
        PauseDownload->setText(QApplication::translate("DownloadTS", "\346\232\202\345\201\234\344\270\213\350\275\275", Q_NULLPTR));
        StopDownload->setText(QApplication::translate("DownloadTS", "\347\273\210\346\255\242\344\270\213\350\275\275", Q_NULLPTR));
        DirDownload->setText(QApplication::translate("DownloadTS", "\346\211\223\345\274\200\344\270\213\350\275\275\347\233\256\345\275\225", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DownloadTS: public Ui_DownloadTS {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOWNLOADTS_H
