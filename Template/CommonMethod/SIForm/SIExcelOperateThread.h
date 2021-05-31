#ifndef EXCELOPERATETHREAD_H
#define EXCELOPERATETHREAD_H

#include <QObject>

#include "COMMONDEFINE.h"

class SIExcelOperateThread : public QObject
{
    Q_OBJECT
public:
    explicit SIExcelOperateThread(QObject *parent = nullptr);

signals:

public slots:

private :
    void Init();
};

#endif // EXCELOPERATETHREAD_H
