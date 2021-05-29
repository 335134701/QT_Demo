#ifndef COMMONMETHOD_H
#define COMMONMETHOD_H

#include <QObject>
#include <QTextCodec>
#include <QStandardItem>
#include <QProcess>
#include <QSettings>
#include <QDateTime>
#include <QMessageBox>
#include "QLoghelper.h"
#include "DEFINE.h"

class CommonMethod : public QObject
{
    Q_OBJECT
public:
    explicit CommonMethod(QObject *parent = nullptr);

    QTextCodec *codec= QTextCodec::codecForName("GBK");

    QString JudgeIDType(const QString ID);

    QString AnalyzePath(const QString dirPath,const QString ID,QString IDType,const QString RelyID,unsigned int flag);

    QStringList FindFile(const QString dirPath,QStringList filters);

    void AnalyzeFilePath(const QStringList filePaths,QString *filePath,unsigned int flag,const QString ID);

    void INIFileWrite(const QString filePath,const QString PartNumber,const QString DiagnosticCode);

    bool INIFileRead(const QString filePath,const QString PartNumber,const QString DiagnosticCode);

    void MessageFileTableChangeDeal(QStandardItem *item,QString *filePath);

    SOFTNUMBERTable MessageSoftTableChangeDeal(const QStandardItem *item,const SOFTNUMBERTable soft,int index);

    static QStringList GetDate();

    static void SetErrorTable(QList<ERRORTable> *errTableList,const QString fileName,const QString sheetName,const unsigned int row,const unsigned int col,const QString errMessage);

    QString Get7zInstallPath();

    QString StringALLReplace(const QString str,const QString befstr,const QString aftstr);
signals:

public slots:

};

#endif // COMMONMETHOD_H
