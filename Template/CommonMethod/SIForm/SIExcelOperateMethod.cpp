#include "SIExcelOperateMethod.h"

/**
 * @brief SIExcelOperateMethod::SIExcelOperateMethod
 * @param parent
 */
SIExcelOperateMethod::SIExcelOperateMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIExcelOperateMethod 构造函数执行!");
}
