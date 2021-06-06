#ifndef AUFILEOPERATEMETHOD_H
#define AUFILEOPERATEMETHOD_H

#include <QObject>

#include "COMMONDEFINE.h"
#include "AuCommonMethod.h"

class AuFileOperateMethod : public QObject
{
    Q_OBJECT
public:
    explicit AuFileOperateMethod(QObject *parent = nullptr);

    QString AnalyzePath(const QString dirPath,const QString ID,QString IDType,unsigned int flag);

    QStringList SearchFile(const QString dirPath,QStringList filters);

    QString AnalyzeFilePath(const QStringList filePaths,const unsigned int flag,const QString ID);


signals:

public slots:
};

#endif // AUFILEOPERATEMETHOD_H
