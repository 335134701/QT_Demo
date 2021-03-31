#include "CommonBean.h"

CommonBean::CommonBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("CommonBean构造函数触发执行中!");
    this->Init();
}

CommonBean *CommonBean::instance()
{
    static CommonBean comBean;
    return &comBean;
}

QMap<QString, QString> CommonBean::getRExpression() const
{
    return RExpression;
}


QString CommonBean::getID() const
{
    return ID;
}

void CommonBean::setID(const QString &value)
{
    ID = value;
}


/**
 * @def 初始化相关参数
 * @brief CommonBean::Init
 */
void CommonBean::Init()
{
    ID="11111111111";
    RExpression.insert("IDEdit","^EN[3|4]\\d\\d\\dP[A-Z]");
}
