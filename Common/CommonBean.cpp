#include "CommonBean.h"

CommonBean::CommonBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("CommonBean构造函数触发执行中!");
    this->Init();
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

QMap<QString, QChar> CommonBean::getErrCode() const
{
    return errCode;
}

QString CommonBean::getRelyID() const
{
    return RelyID;
}

void CommonBean::setRelyID(const QString &value)
{
    RelyID = value;
}


/**
 * @def 程序启动时初始化相关参数
 * @brief CommonBean::Init
 */
void CommonBean::Init()
{
    ID="";
    RelyID="";
    RExpression.insert("IDEdit","^EN[3|4]\\d\\d\\dP[A-Z]");
}
/**
 * @def 程序运行过程中需要对部分参数进行初始化处理
 * @brief CommonBean::ParameterInit
 */
void CommonBean::ParameterInit()
{

}
