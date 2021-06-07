#ifndef FILEOPERATETHREAD_H
#define FILEOPERATETHREAD_H

#include <QObject>
#include <QProcess>

#include "COMMONDEFINE.h"
#include "SICommonMethod.h"
#include "SIFileOperateMethod.h"

class SIFileOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit SIFileOperateThread(QObject *parent = nullptr);

    SIFileOperateMethod *getSiFileOperateMethod() const;
    void setSiFileOperateMethod(SIFileOperateMethod *value);

signals:
    void EndRunOrderSignal(const bool result,const unsigned int flag);

    void EndCopyFileSignal(const QString filePath,const unsigned int flag, const bool result);

    void EndFileSearcSignal(const QString filePath,unsigned int flag, bool isGoON);

    void EndCheckBAFileSignal(const bool result,QList<SI_ERRORTable> errList);

    void EndCheckCLFileSignal(const bool result,QList<SI_ERRORTable> errList);

public slots:

    void UpdateSVNSlot(const QString exeFilePath, const QString dirPath,const unsigned flag);

    void UNZipCodeFileSlot(const QString exeFilePath, const QString filePath,const QString desDirPath,const unsigned flag);

    void ZipCodeFileSlot(const QString exeFilePath, const QString dirPath,const QString IDType,const QString txt,QString APPVer,const unsigned flag);

    void FileSearchSlot(const QString dirPath,const QStringList filters,const QString ID,QString IDType,unsigned int flag, bool isGoON);

    void CheckBAFileSlot(const QString dirPath,const QString ID,const QString IDType,const QList<SI_SOFTNUMBERTable> softList,const unsigned int flag);

    void CheckCLFileSlot(const QString dirPath,const QString ID,const QList<SI_SOFTNUMBERTable> softList);

    void CopyFileSlot(const QString srcFilePath,const QString desFilePath,const unsigned int flag);
private :

    SIFileOperateMethod *siFileOperateMethod;

    CommonMethod *commonMethod;

    SICommonMethod *siCommonMethod;

    void Init();
};

#endif // FILEOPERATETHREAD_H
