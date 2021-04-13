#include "CommonBean.h"

CommonBean::CommonBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("CommonBean() 构造函数执行!");
    this->Init();
    this->ErrorCodeInit();
}



/**
 * @def 程序启动时初始化相关参数
 * @brief CommonBean::Init
 */
void CommonBean::Init()
{
    QLogHelper::instance()->LogInfo("CommonBean->Init() 函数执行!");
    comMethod=new CommonMethod();
    //初始化设置Error Code，通过解析XML方式获取错误码
    xmlOperate=new XMLOperate();
    //初始化错误码
    RExpression.insert("IDEdit","^EN[3|4]\\d\\d\\dP[A-Z]");
    RExpression.insert("RelyIDEdit","^EN[3|4]\\d\\d\\dP[A-Z]");
}
/**
 * @def 程序运行过程中需要对部分参数进行初始化处理
 * @brief CommonBean::ParameterInit
 */
void CommonBean::ParameterInit()
{
    QLogHelper::instance()->LogInfo("CommonBean->ParameterInit() 函数执行!");
}
/**
 * @def 初始化错误码
 * @brief CommonBean::ErrorCodeInit
 * @return
 */
bool CommonBean::ErrorCodeInit()
{
    bool ret=false;
    if(getXmlOperate()->getErrCodeType().count()>0){
    }
    return ret;
}

QMap<QString, QString> CommonBean::getRExpression() const
{
    return RExpression;
}

QMap<QString, ERRCODETYPE> CommonBean::getErrCode() const
{
    return errCode;
}



QString CommonBean::getIDType() const
{
    return IDType;
}

void CommonBean::setIDType(const QString &value)
{
    IDType = value;
}

QString CommonBean::getRelyFilePath() const
{
    return RelyFilePath;
}

void CommonBean::setRelyFilePath(const QString &value)
{
    RelyFilePath = value;
}

CommonMethod *CommonBean::getComMethod() const
{
    return comMethod;
}

void CommonBean::setComMethod(CommonMethod *value)
{
    comMethod = value;
}

XMLOperate *CommonBean::getXmlOperate() const
{
    return xmlOperate;
}

void CommonBean::setXmlOperate(XMLOperate *value)
{
    xmlOperate = value;
}
