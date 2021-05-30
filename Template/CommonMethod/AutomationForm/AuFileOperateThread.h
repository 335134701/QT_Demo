#ifndef AUFILEOPERATETHREAD_H
#define AUFILEOPERATETHREAD_H

#include <QObject>

#include "COMMONDEFINE.h"

class AuFileOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit AuFileOperateThread(QObject *parent = nullptr);

signals:

public slots:
};

#endif // AUFILEOPERATETHREAD_H
