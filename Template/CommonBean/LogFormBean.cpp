#include "LogFormBean.h"

/**
 * @brief LogFormBean::LogFormBean
 * @param parent
 */
LogFormBean::LogFormBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("LogFormBean 构造函数执行!");
    this->Init();
}

/**
 * @brief LogFormBean::getLogView
 * @return
 */
QTextEdit *LogFormBean::getLogView() const
{
    return logView;
}

/**
 * @brief LogFormBean::setLogView
 * @param value
 */
void LogFormBean::setLogView(QTextEdit *value)
{
    logView = value;
}

/**
 * @brief LogFormBean::getTextList
 * @return
 */
QStringList *LogFormBean::getTextList() const
{
    return textList;
}

/**
 * @brief LogFormBean::setTextList
 * @param value
 */
void LogFormBean::setTextList(QStringList *value)
{
    textList = value;
}

/**
 * @def 初始化函数
 * @brief LogFormBean::Init
 */
void LogFormBean::Init()
{
    QLogHelper::instance()->LogInfo("LogFormBean->Init() 函数执行!");
    textList=new QStringList();
}
