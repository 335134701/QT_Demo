#ifndef AUEXCELOPERATETHREAD_H
#define AUEXCELOPERATETHREAD_H

#include <QObject>

#include "COMMONDEFINE.h"

class AuExcelOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit AuExcelOperateThread(QObject *parent = nullptr);

signals:

public slots:
};

#endif // AUEXCELOPERATETHREAD_H
