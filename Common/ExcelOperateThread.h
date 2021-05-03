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

public slots:
    void ExcelOperateThreadSlot(ExcelOperation *exl,const QString filePath,const QString ID,const QString IDType,unsigned int flag);

private :
    QList<SOFTNUMBERTable> DealSoftTable(QList<SOFTNUMBERTable> list,QString IDType);
};

#endif // EXCELOPERATETHREAD_H
