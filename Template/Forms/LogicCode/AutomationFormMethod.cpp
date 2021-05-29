#include "AutomationFormMethod.h"

/**
 * @brief AutomationFormMethod::AutomationFormMethod
 * @param parent
 */
AutomationFormMethod::AutomationFormMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod 构造函数执行!");
    this->Init();
    this->ConnectSlot();
}

AutomationFormBean *AutomationFormMethod::getAutomationFormBean() const
{
    return automationFormBean;
}

void AutomationFormMethod::setAutomationFormBean(AutomationFormBean *value)
{
    automationFormBean = value;
}

LogFormMetod *AutomationFormMethod::getLogFormMethod() const
{
    return logFormMethod;
}

void AutomationFormMethod::setLogFormMethod(LogFormMetod *value)
{
    logFormMethod = value;
}

/**
 * @brief AutomationFormMethod::ConnectOtherUISlot
 */
void AutomationFormMethod::ConnectOtherUISlot()
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod->ConnectOtherUISlot() 函数执行!");
    //connect(this,&AutomationFormMethod,this->logFormMethod,&LogFormMetod::ShowLogMessageSlot);
}

/**
 * @brief AutomationFormMethod::Init
 */
void AutomationFormMethod::Init()
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod->Init() 函数执行!");
}

/**
 * @brief AutomationFormMethod::ConnectSlot
 */
void AutomationFormMethod::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod->ConnectSlot() 函数执行!");
}
