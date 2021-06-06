#ifndef AUFILEOPERATETHREAD_H
#define AUFILEOPERATETHREAD_H

#include <QObject>

#include "COMMONDEFINE.h"
#include "AuFileOperateMethod.h"
#include "AuCommonMethod.h"

class AuFileOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit AuFileOperateThread(QObject *parent = nullptr);

signals:

    void EndFileSearcSignal(const QString filePath,unsigned int flag, bool isGoON);

public slots:

    void FileSearchSlot(const QString dirPath,const QStringList filters,const QString ID,QString IDType,unsigned int flag, bool isGoON);

private:

    AuFileOperateMethod *auFileOperateMethod;

    void Init();
};

#endif // AUFILEOPERATETHREAD_H
