#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include "CommonMethod.h"

class MyThread : public QObject
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);

signals:
    void EndFindFileThreadSignal(QStringList st,QString dirPath,unsigned int flag);

public slots:
    void FindFileThreadSlot(const QString dirPath,CommonMethod *commonMethod,const QStringList filters,unsigned int flag);
};

#endif // MYTHREAD_H
