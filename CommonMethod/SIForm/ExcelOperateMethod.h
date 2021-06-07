#ifndef EXCELOPERATEMETHOD_H
#define EXCELOPERATEMETHOD_H

#include <QObject>
#include "libxl.h"
#include "COMMONDEFINE.h"

class ExcelOperateMethod : public QObject
{
    Q_OBJECT
public:
    explicit ExcelOperateMethod(QObject *parent = nullptr);

signals:

public slots:
};

#endif // EXCELOPERATEMETHOD_H
