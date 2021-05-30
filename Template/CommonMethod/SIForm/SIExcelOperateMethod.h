#ifndef EXCELOPERATEMETHOD_H
#define EXCELOPERATEMETHOD_H

#include <QObject>
#include "libxl.h"
#include "COMMONDEFINE.h"

class SIExcelOperateMethod : public QObject
{
    Q_OBJECT
public:
    explicit SIExcelOperateMethod(QObject *parent = nullptr);

signals:

public slots:
};

#endif // EXCELOPERATEMETHOD_H
