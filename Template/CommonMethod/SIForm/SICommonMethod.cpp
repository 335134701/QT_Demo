#include "SICommonMethod.h"

/**
 * @brief SICommonMethod::SICommonMethod
 * @param parent
 */
SICommonMethod::SICommonMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SICommonMethod 构造函数执行!");
}
