#ifndef UIMETHOD_H
#define UIMETHOD_H

#include <QObject>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QTextEdit>
#include <QThread>
#include <QDir>
#include <QMetaType>
#include <Qmessagebox>
#include <QFileDialog>
#include "QLoghelper.h"
#include "CommonBean.h"
#include "FileThread.h"
#include "CommonMethod.h"
#include "ExcelOperateThread.h"
#include "ExcelOperation.h"
#include "DEFINE.h"


class UIMethod : public QObject
{
    Q_OBJECT

    #define errFontColor "color:red;"
    #define nomFontColor "color:white;"
public:
    explicit UIMethod(QObject *parent = nullptr);

    void setComBean(CommonBean *value);

    QTextEdit *getTextEdit() const;
    void setTextEdit(QTextEdit *value);

signals:
    void FindFileThreadSignal(const QString dirPath,CommonMethod *commonMethod,const QStringList filters,unsigned int flag, bool goOn);

    void ActiveThreadSignal(QString dirPath,unsigned int flag, bool goOn);

    void ExcelOperateThreadSignal(ExcelOperation *exl,const QString filePath,const QString ID,const QString IDType,unsigned int flag);

    void ShowIDmessageSignal(int flag);

public slots:
    void JudgeIDSlot(QLineEdit *Edit,QString *objectID);

    void JudgeIDTypeSlot(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowIDmessageSlot(int flag);

    void SelectDirSlot(QLabel *label,QString *objectID,const QString errName);

    void SelectFileSlot(QString dirPath,unsigned int flag, bool goOn);

    void EndFindFileThreadSlot(QStringList st,unsigned int flag, bool goOn);

    void EndExcelOperateThreadSoftSlot(QList<SOFTNUMBERTable> list);
    void EndExcelOperateThreadConfSlot(QList<CONFIGTable> list);

    void MessageViewModelEditedSlot(const QStandardItem *item);

    void CreateSlot();

private :
    void Init();

    QString JudgeIDType(const QString ID);

    QTextEdit *textEdit;

    CommonBean *comBean;

    FileThread *fileThread;

    ExcelOperateThread *excelOperateThread;

    QThread *dealFileFileThread;

    QThread *excelThread;

};

#endif // UIMETHOD_H
