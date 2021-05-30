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

/**
 * @brief AutomationFormMethod::getTableView
 * @return
 */
QTableView *AutomationFormMethod::getTableView() const
{
    return tableView;
}

/**
 * @brief AutomationFormMethod::setTableView
 * @param value
 */
void AutomationFormMethod::setTableView(QTableView *value)
{
    tableView = value;
}

/**
 * @brief AutomationFormMethod::getAutomationFormBean
 * @return
 */
AutomationFormBean *AutomationFormMethod::getAutomationFormBean() const
{
    return automationFormBean;
}

/**
 * @brief AutomationFormMethod::setAutomationFormBean
 * @param value
 */
void AutomationFormMethod::setAutomationFormBean(AutomationFormBean *value)
{
    automationFormBean = value;
}

/**
 * @brief AutomationFormMethod::getLogFormMethod
 * @return
 */
LogFormMetod *AutomationFormMethod::getLogFormMethod() const
{
    return logFormMethod;
}

/**
 * @brief AutomationFormMethod::setLogFormMethod
 * @param value
 */
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
    excelOperateThread=new AuExcelOperateThread();
    fileOperateThread=new AuFileOperateThread();
}

/**
 * @brief AutomationFormMethod::ConnectSlot
 */
void AutomationFormMethod::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod->ConnectSlot() 函数执行!");
}

/**
 * @brief AutomationFormMethod::InitTableView
 */
void AutomationFormMethod::InitTableView()
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod->InitTableView() 函数执行!");
}

/**
 * @brief AutomationFormMethod::ShowTableView
 * @param message
 * @param flag
 */
void AutomationFormMethod::ShowTableView(const QStringList message, const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod->ShowTableView() 函数执行!");
}

