#ifndef COMMONMETHOD_H
#define COMMONMETHOD_H

#include <QObject>
#include <QTextCodec>
#include <QStandardItem>
#include "XMLOperate.h"
#include "QLoghelper.h"
#include "ExcelOperation.h"
#include "DEFINE.h"

class CommonMethod : public QObject
{
    Q_OBJECT
public:
    explicit CommonMethod(QObject *parent = nullptr);

    QTextCodec *codec= QTextCodec::codecForName("GBK");

    QString JudgeIDType(const QString ID);

    QString AnalyzePath(const QString dirPath,const QString ID,QString IDType,unsigned int flag);

    void ErrorCodeDeal(QMap<QString,ERRCODETYPE> *errCode,QMap<QString,ERRCODETYPE> errCodeType,const QString objectName,const QString condition,bool flag);

    QStringList FindFile(const QString dirPath,QStringList filters);

    void AnalyzeFilePath(const QStringList filePaths,QString *filePath,unsigned int flag);

    void INIFileWrite(const QString filePath,const QString PartNumber,const QString DiagnosticCode);

    bool INIFileRead(const QString filePath,const QString PartNumber,const QString DiagnosticCode);

    void MessageFileTableChangeDeal(QStandardItem *item,QString *filePath);

    SOFTNUMBERTable MessageSoftTableChangeDeal(const QStandardItem *item,const SOFTNUMBERTable soft,int index);

signals:

public slots:

};

#endif // COMMONMETHOD_H
