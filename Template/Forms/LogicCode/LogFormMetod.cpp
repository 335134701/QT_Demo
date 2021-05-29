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

/**
 * @brief LogFormMetod::PrintMessage
 * @param message
 * @param level
 */
void LogFormMetod::PrintMessage(QStringList message,const unsigned int level)
{
    QLogHelper::instance()->LogInfo("LogFormMetod->PrintMessage() 函数执行!");
    unsigned int i=0;
    message.insert(0,"===================================================");
    message.insert(message.size(),"===================================================");
    foreach (QString str, message) {
        if(i==0||i==(message.size()-1))
        {
            logFormBean->getLogView()->setTextColor(0x00FF00);
            logFormBean->getLogView()->append(DATAStytle+str);
        }else{
            logFormBean->getLogView()->setTextColor(0x00FF00);
            logFormBean->getLogView()->append(DATAStytle);
            switch (level) {
            case LOG_INFO:
                logFormBean->getLogView()->setTextColor(0x00FFFF);
                break;
            case LOG_DEBUG:
                logFormBean->getLogView()->setTextColor(0x0000FF);
                break;
            case LOG_WARN:
                logFormBean->getLogView()->setTextColor(0xFFFF00);
                break;
            case LOG_ERROR:
                logFormBean->getLogView()->setTextColor(0xFF0000);
                break;
            }
            logFormBean->getLogView()->insertPlainText(str);
        }
        i++;
    }
    logFormBean->getLogView()->setTextColor(0xFFFFFF);
}
