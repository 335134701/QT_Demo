#include "AuFileOperateMethod.h"


/**
 * @brief AuFileOperateMethod::AuFileOperateMethod
 * @param parent
 */
AuFileOperateMethod::AuFileOperateMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("AuFileOperateMethod 构造函数执行!");
}
