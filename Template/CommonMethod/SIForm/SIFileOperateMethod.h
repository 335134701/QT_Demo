#ifndef FILEOPERATEMETHOD_H
#define FILEOPERATEMETHOD_H

#include <QObject>

#include "COMMONDEFINE.h"
#include "SICommonMethod.h"
#include "CommonMethod.h"

class SIFileOperateMethod : public QObject
{
    Q_OBJECT
public:
    explicit SIFileOperateMethod(QObject *parent = nullptr);

    SICommonMethod *getSiCommonMethod() const;
    void setSiCommonMethod(SICommonMethod *value);

    CommonMethod *getCommonMethod() const;
    void setCommonMethod(CommonMethod *value);

    QString AnalyzePath(const QString dirPath,const QString ID,const QString IDType,unsigned int flag);

    QStringList SearchFile(const QString dirPath,const QStringList filters);

    QString AnalyzeFilePath(const QStringList filePaths,const unsigned int flag,const QString ID,const QString IDType);

    bool UNZipCodeDearDirSignal(const QString dirPath);

    QList<SI_ERRORTable> CheckBAFileExist(const QString dirPath,const QString IDType);

signals:

public slots:

private:

    SICommonMethod *siCommonMethod;

    CommonMethod *commonMethod;

    void Init();
};

#endif // FILEOPERATEMETHOD_H
