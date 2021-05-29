#ifndef EXCELOPERATETHREAD_H
#define EXCELOPERATETHREAD_H

#include <QObject>

#include "COMMONDEFINE.h"

class ExcelOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit ExcelOperateThread(QObject *parent = nullptr);

signals:

public slots:
};

#endif // EXCELOPERATETHREAD_H
