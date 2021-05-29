#include "MainFormBean.h"

/**
 * @brief MainFormBean::MainFormBean
 * @param parent
 */
MainFormBean::MainFormBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("MainFormBean 构造函数执行!");
    this->Init();
}

CommonMethod *MainFormBean::getCommonMethod() const
{
    return commonMethod;
}

void MainFormBean::setCommonMethod(CommonMethod *value)
{
    commonMethod = value;
}

/**
 * @def 初始化函数
 * @brief MainFormBean::Init
 */
void MainFormBean::Init()
{
    QLogHelper::instance()->LogInfo("MainFormBean->Init() 函数执行!");
    commonMethod=new CommonMethod();
}
