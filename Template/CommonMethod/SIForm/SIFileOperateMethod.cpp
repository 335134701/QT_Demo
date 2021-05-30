#include "SIFileOperateMethod.h"

/**
 * @brief SIFileOperateMethod::SIFileOperateMethod
 * @param parent
 */
SIFileOperateMethod::SIFileOperateMethod(QObject *parent) : QObject(parent)
{
     QLogHelper::instance()->LogInfo("SIFileOperateMethod 构造函数执行!");
}
