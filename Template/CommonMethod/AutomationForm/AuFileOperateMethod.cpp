#include "AuFileOperateMethod.h"


/**
 * @brief AuFileOperateMethod::AuFileOperateMethod
 * @param parent
 */
AuFileOperateMethod::AuFileOperateMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SICommonMethod 构造函数执行!");
}
