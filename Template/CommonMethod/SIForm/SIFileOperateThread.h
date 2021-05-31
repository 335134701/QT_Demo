#ifndef FILEOPERATETHREAD_H
#define FILEOPERATETHREAD_H

#include <QObject>
#include <QProcess>

#include "COMMONDEFINE.h"
#include "SIFileOperateMethod.h"

class SIFileOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit SIFileOperateThread(QObject *parent = nullptr);

signals:
    void EndUpdateSVNSignal(const bool result);

    void EndFileSearcSignal(const QString filePath,unsigned int flag, bool isGoON);

public slots:

    void UpdateSVNSlot(const QString exeFilePath,const QString dirPath);

    void FileSearchSlot(const QString dirPath,const QStringList filters,const QString ID,QString IDType,unsigned int flag, bool isGoON);

private :

    SIFileOperateMethod *siFileOperateMethod;

    void Init();
};

#endif // FILEOPERATETHREAD_H
