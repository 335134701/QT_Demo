#ifndef FILEOPERATETHREAD_H
#define FILEOPERATETHREAD_H

#include <QObject>

#include "COMMONDEFINE.h"

class FileOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit FileOperateThread(QObject *parent = nullptr);

signals:

public slots:
};

#endif // FILEOPERATETHREAD_H
