#ifndef EXCELOPERATETHREAD_H
#define EXCELOPERATETHREAD_H

#include <QObject>

#include "COMMONDEFINE.h"
#include "SIExcelOperateMethod.h"
#include "SIFormBean.h"
#include "SICommonMethod.h"

class SIExcelOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit SIExcelOperateThread(QObject *parent = nullptr);

    SIExcelOperateMethod *getSiExcelOperateMethod() const;
    void setSiExcelOperateMethod(SIExcelOperateMethod *value);

signals:
    void EndReadSoftExcelSignal(const QList<SI_SOFTNUMBERTable> softList, const QList<SI_ERRORTable> errList);

    void EndReadDefineFileExcelSignal(const QList<SI_DEFINEMESSAGE> defineList,const QList<SI_ERRORTable> errList);

public slots:
    void ReadExcelThreadSlot(const QString filePath,const QString ID,const QString IDType,const unsigned int flag);

private :
    SIExcelOperateMethod *siExcelOperateMethod;

    void Init();
};

#endif // EXCELOPERATETHREAD_H
