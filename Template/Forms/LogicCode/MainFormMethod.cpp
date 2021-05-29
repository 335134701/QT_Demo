#include "MainFormMethod.h"

/**
 * @brief MainFormMethod::MainFormMethod
 * @param parent
 */
MainFormMethod::MainFormMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("MainFormMethod 构造函数执行!");
    this->Init();
    this->ConnectSlot();
}

/**
 * @brief MainFormMethod::getMainFormBean
 * @return
 */
MainFormBean *MainFormMethod::getMainFormBean() const
{
    return mainFormBean;
}

/**
 * @brief MainFormMethod::setMainFormBean
 * @param value
 */
void MainFormMethod::setMainFormBean(MainFormBean *value)
{
    mainFormBean = value;
}


/**
 * @def 初始化函数
 * @brief MainFormMethod::Init
 */
void MainFormMethod::Init()
{
    QLogHelper::instance()->LogInfo("MainFormMethod->Init() 函数执行!");
}

/**
 * @def 处理连接信号槽函数
 * @brief MainFormMethod::ConnectSlot
 */
void MainFormMethod::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("MainFormMethod->ConnectSlot() 函数执行!");
}
