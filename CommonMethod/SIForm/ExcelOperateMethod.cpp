#include "ExcelOperateMethod.h"

/**
 * @brief ExcelOperateMethod::ExcelOperateMethod
 * @param parent
 */
ExcelOperateMethod::ExcelOperateMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("ExcelOperateMethod 构造函数执行!");
}
