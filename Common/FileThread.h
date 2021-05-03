#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include "CommonMethod.h"
#include "DEFINE.h"

class FileThread : public QObject
{
    Q_OBJECT
public:
    explicit FileThread(QObject *parent = nullptr);

signals:
    void EndFindFileThreadSignal(QStringList st,unsigned int flag, bool goOn);

public slots:
    void FindFileThreadSlot(const QString dirPath,CommonMethod *commonMethod,const QStringList filters,unsigned int flag, bool goOn);
};

#endif // MYTHREAD_H
