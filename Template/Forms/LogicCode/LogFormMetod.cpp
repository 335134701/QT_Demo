#include "LogFormMetod.h"

/**
 * @brief LogFormMetod::LogFormMetod
 * @param parent
 */
LogFormMetod::LogFormMetod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("LogFormMetod 构造函数执行!");
    this->Init();
    this->ConnectSlot();
}

/**
 * @brief LogFormMetod::getLogFormBean
 * @return
 */
LogFormBean *LogFormMetod::getLogFormBean() const
{
    return logFormBean;
}

/**
 * @brief LogFormMetod::setLogFormBean
 * @param value
 */
void LogFormMetod::setLogFormBean(LogFormBean *value)
{
    logFormBean = value;
}

/**
 * @def 初始化函数
 * @brief LogFormMetod::Init
 */
void LogFormMetod::Init()
{
    QLogHelper::instance()->LogInfo("LogFormMetod->Init() 函数执行!");
}

/**
 * @def 处理连接信号槽函数
 * @brief LogFormMetod::ConnectSlot
 */
void LogFormMetod::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("LogFormMetod->ConnectSlot() 函数执行!");
}

/**
 * @brief LogFormMetod::PrintMessage
 * @param message
 * @param level
 */
void LogFormMetod::PrintMessage(const QStringList message,const unsigned int level)
{
    QLogHelper::instance()->LogInfo("LogFormMetod->PrintMessage() 函数执行!");
    QStringList tmplist;
    switch (level) {
    case LOG_INFO:
        foreach (QString str, message) {
               logFormBean->getLogView()->append(str);
        }
        break;
    case LOG_DEBUG:

        break;
    case LOG_WARN:

        break;
    case LOG_ERROR:

        break;
    default:
        break;
    }
}

/**
 * @brief LogFormMetod::ShowLogMessageSlot
 * @param message
 * @param level
 */
void LogFormMetod::ShowLogMessageSlot(const QStringList message,const unsigned int level)
{
    QLogHelper::instance()->LogInfo("LogFormMetod->ShowLogMessageSlot() 函数执行!");
    if(message.size()>0)
    {
        this->PrintMessage(message,level);
    }
}

