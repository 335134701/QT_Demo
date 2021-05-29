#include "AutomationFormBean.h"

/**
 * @brief AutomationFormBean::AutomationFormBean
 * @param parent
 */
AutomationFormBean::AutomationFormBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("LogFormBean 构造函数执行!");
    this->Init();
}

/**
 * @brief AutomationFormBean::getCommonMethod
 * @return
 */
CommonMethod *AutomationFormBean::getCommonMethod() const
{
    return commonMethod;
}

/**
 * @brief AutomationFormBean::setCommonMethod
 * @param value
 */
void AutomationFormBean::setCommonMethod(CommonMethod *value)
{
    commonMethod = value;
}

void AutomationFormBean::Init()
{
    QLogHelper::instance()->LogInfo("AutomationFormBean->Init() 函数执行!");
}
