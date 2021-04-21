#ifndef EXCELOPERATETHREAD_H
#define EXCELOPERATETHREAD_H

#include <QObject>
#include "QLoghelper.h"
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
    void ExcelOperateThreadSlot(const QString filePath,const QString ID,const QString IDType,unsigned int flag);
};

#endif // EXCELOPERATETHREAD_H
