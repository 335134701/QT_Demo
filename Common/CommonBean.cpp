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

/**
 * @def 初始化相关参数
 * @brief CommonBean::Init
 */
void CommonBean::Init()
{
    ID="11111111111";
//   RExpression.insert("");
}


QMap<QString, QString> CommonBean::getRExpression() const
{
    return RExpression;
}

void CommonBean::setRExpression(const QMap<QString, QString> &value)
{
    RExpression = value;
}

QString CommonBean::getID() const
{
    return ID;
}

void CommonBean::setID(const QString &value)
{
    ID = value;
}


