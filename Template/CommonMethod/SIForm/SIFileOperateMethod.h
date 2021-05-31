#ifndef FILEOPERATEMETHOD_H
#define FILEOPERATEMETHOD_H

#include <QObject>

#include "COMMONDEFINE.h"

class SIFileOperateMethod : public QObject
{
    Q_OBJECT
public:
    explicit SIFileOperateMethod(QObject *parent = nullptr);

    QString AnalyzePath(const QString dirPath,const QString ID,QString IDType,unsigned int flag);

    QStringList SearchFile(const QString dirPath,QStringList filters);

    QString AnalyzeFilePath(const QStringList filePaths,const unsigned int flag,const QString ID);


signals:

public slots:
};

#endif // FILEOPERATEMETHOD_H
