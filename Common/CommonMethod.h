#ifndef COMMONMETHOD_H
#define COMMONMETHOD_H

#include <QObject>
#include "XMLOperate.h"
#include "QLoghelper.h"

class CommonMethod : public QObject
{
    Q_OBJECT
public:
    explicit CommonMethod(QObject *parent = nullptr);

    QString JudgeIDType(const QString ID);

    QString AnalyzePath(const QString dirPath);

    void ErrorCodeDeal(QMap<QString,ERRCODETYPE> *errCode,QMap<QString,ERRCODETYPE> errCodeType,const QString objectName,const QString condition,bool flag);

    QStringList FindFile(const QString dirPath,QStringList filters);

    QString AnalyzeRelyFilePath(const QStringList filePath);


signals:

public slots:

};

#endif // COMMONMETHOD_H
