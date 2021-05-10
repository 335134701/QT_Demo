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

    void ExcelOperateThreadSignal(ExcelOperation *exl,const QString filePath,const QString ID,const QString IDType,const QString RelyID,unsigned int flag);

    void EEExcelWriteSignal(ExcelOperation *exl, const QString filePath,const QString ID,const QString IDType,const QString RelyID, QList<SOFTNUMBERTable> *softNumberTable);

    void ReadyExcelWriteSignal(ExcelOperation *exl, const QString filePath, QList<SOFTNUMBERTable> *softNumberTable,QList<CONFIGTable> *configTable,QStringList DefineConfigList,const QString RelyID,const QString IDType);

    void EEExcelReadSignal(ExcelOperation *exl, const QString filePath,const QString ID,const QString IDType,const QString RelyID, QList<SOFTNUMBERTable> *softNumberTable);

    void ReadyExcelReadSignal(ExcelOperation *exl, const QString filePath, QList<SOFTNUMBERTable> *softNumberTable,QList<CONFIGTable> *configTable,QStringList DefineConfigList,const QString RelyID,const QString IDType);

    void ShowIDmessageSignal(int flag);

public slots:
    void JudgeIDSlot(QLineEdit *Edit,QString *objectID);

    void JudgeIDTypeSlot(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowIDmessageSlot(int flag);

    void SelectDirSlot(QLabel *label,QString *objectID);

    void SelectFileSlot(QString dirPath,unsigned int flag, bool goOn);

    void EndFindFileThreadSlot(QStringList st,unsigned int flag, bool goOn);

    void EndExcelOperateThreadSoftSlot(QList<SOFTNUMBERTable> list);

    void EndExcelOperateThreadConfSlot(QList<CONFIGTable> list);

    void EndEEExcelWriteSlot(QList<ERRORTable> *errTableList);

    void EndReadyExcelWriteSlot(QList<ERRORTable> *errTableList);

    void EndEEExcelReadSlot(QList<ERRORTable> *errTableList);

    void EndReadyExcelReadSlot(QList<ERRORTable> *errTableList);

    void MessageViewModelEditedSlot(QStandardItem *item);

    void CreateSlot();

    void CheckSlot();

private :
    void Init();

    QString JudgeIDType(const QString ID);

    QTextEdit *textEdit;

    CommonBean *comBean;

    FileThread *fileThread;

    ExcelOperateThread *excelOperateThread;

    QThread *dealFileFileThread;

    QThread *excelThread;

    QString carTmpPath;

    QList<ERRORTable> *errList;
};

#endif // UIMETHOD_H
