#ifndef EXCELOPERATION_H
#define EXCELOPERATION_H

#include <QObject>
#include <QDateTime>
#include <QTextCodec>
#include "CommonMethod.h"
#include "libxl.h"
#include "QLoghelper.h"
#include "DEFINE.h"

using namespace libxl;

class ExcelOperation : public QObject
{
    Q_OBJECT
public:
    QTextCodec *codec = QTextCodec::codecForName("GBK") ;

    explicit ExcelOperation(QObject *parent = nullptr);

    bool Init(const QString filePath);

    QList<SOFTNUMBERTable> ReadSoftExcel(const QString filePath,const QString ID,const QString IDType);

    QList<CONFIGTable> ReadConfExcel(const QString filePath,const QString ID);

    void EEFileWrite(const QString filePath,const QString ID,const QString IDType,const QString RelyID, QList<SOFTNUMBERTable> *softNumberTable,QList<ERRORTable> *errTableList);

    void ReadyFileWrite(const QString filePath,QList<SOFTNUMBERTable> *softNumberTable,QList<CONFIGTable> *configTable,QStringList DefineConfigList,const QString IDType,const QString RelyID,QList<ERRORTable> *errTableList);
signals:

public slots:

private :
   Book *book;
   void SetPatternForegroundColor(Sheet *sheetread,const int row,const int col,const Color color);
   void ReadyFileFirstSheet(const QString filePath, QList<SOFTNUMBERTable> *softNumberTable,QList<CONFIGTable> *configTable,QList<ERRORTable> *errTableList);
   void ReadyFileSecondSheet(const QString filePath,QList<CONFIGTable> *configTable,QList<ERRORTable> *errTableList);
   void ReadyFileThirdSheet(const QString filePath,QList<SOFTNUMBERTable> *softNumberTable,QStringList DefineConfigList,const QString IDType,bool flag,QList<ERRORTable> *errTableList);
};

#endif // EXCELOPERATION_H
