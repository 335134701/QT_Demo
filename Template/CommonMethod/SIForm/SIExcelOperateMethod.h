#ifndef EXCELOPERATEMETHOD_H
#define EXCELOPERATEMETHOD_H

#include <QObject>
#include <QTextCodec>

#include "libxl.h"
#include "COMMONDEFINE.h"
#include "SIFormBean.h"
#include "SICommonMethod.h"

using namespace libxl;

class SIExcelOperateMethod : public QObject
{
    Q_OBJECT
public:
    explicit SIExcelOperateMethod(QObject *parent = nullptr);

    bool InitLIBXL(const QString filePath);

    QList<SI_SOFTNUMBERTable> ReadSoftExcel(const QString filePath,const QString ID, const QString IDType,QList<SI_ERRORTable> *errList);

    QList<SI_SOFTNUMBERTable> ReadSoftExcel(const QString filePath, const QString IDType, const QString condition);

    void WriteDefineExcel(const QString filePath,const QString ID, const QString IDType,const QString CarModels,QList<SI_ERRORTable> *errList);

    QList<SI_DEFINEMESSAGE> ReadDefineExcel(const QString filePath, const QString ID, const QString IDType,QList<SI_ERRORTable> *errList);

    QList<SI_SOFTNUMBERTable> DealSoftTable(QList<SI_SOFTNUMBERTable> softList, const QString condition);

signals:

public slots:

private:
    QTextCodec *codec;

    Book *book;

    void Init();
};

#endif // EXCELOPERATEMETHOD_H
