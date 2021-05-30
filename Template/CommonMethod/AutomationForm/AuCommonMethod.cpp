#include "AuCommonMethod.h"

/**
 * @brief AuCommonMethod::AuCommonMethod
 * @param parent
 */
AuCommonMethod::AuCommonMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("AuCommonMethod 构造函数执行!");
}
