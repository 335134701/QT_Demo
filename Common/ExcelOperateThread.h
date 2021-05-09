#ifndef EXCELOPERATETHREAD_H
#define EXCELOPERATETHREAD_H

#include <QObject>
#include "QLoghelper.h"
#include "DEFINE.h"
#include "ExcelOperation.h"

class ExcelOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit ExcelOperateThread(QObject *parent = nullptr);

signals:
    void EndExcelOperateThreadSoftSignal(QList<SOFTNUMBERTable> list);
    void EndExcelOperateThreadConfSignal(QList<CONFIGTable> list);
    void EndEEExcelWriteSignal(bool flag);
    void EndReadyExcelWriteSignal(bool flag);

public slots:
    void ExcelOperateThreadSlot(ExcelOperation *exl,const QString filePath,const QString ID,const QString IDType,const QString RelyID,unsigned int flag);
    void EEExcelWriteSlot(ExcelOperation *exl, const QString filePath,const QString ID,const QString IDType, QList<SOFTNUMBERTable> *softNumberTable);
    void ReadyExcelWriteSlot(ExcelOperation *exl, const QString filePath, QList<SOFTNUMBERTable> *softNumberTable,QList<CONFIGTable> *configTable,QStringList DefineConfigList);

private :
    QList<SOFTNUMBERTable> DealSoftTable(QList<SOFTNUMBERTable> list,QString IDType);
};

#endif // EXCELOPERATETHREAD_H
