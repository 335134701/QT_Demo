#ifndef FILEOPERATETHREAD_H
#define FILEOPERATETHREAD_H

#include <QObject>

#include "COMMONDEFINE.h"

class SIFileOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit SIFileOperateThread(QObject *parent = nullptr);

signals:

public slots:
};

#endif // FILEOPERATETHREAD_H
