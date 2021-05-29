#include "AutomationFormMethod.h"

/**
 * @brief AutomationFormMethod::AutomationFormMethod
 * @param parent
 */
AutomationFormMethod::AutomationFormMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod 构造函数执行!");
}
