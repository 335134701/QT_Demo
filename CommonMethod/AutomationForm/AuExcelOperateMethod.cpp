#include "AuExcelOperateMethod.h"

/**
 * @brief AuExcelOperateMethod::AuExcelOperateMethod
 * @param parent
 */
AuExcelOperateMethod::AuExcelOperateMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("AuExcelOperateMethod 构造函数执行!");
}
