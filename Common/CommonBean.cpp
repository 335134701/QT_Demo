#include "CommonBean.h"

CommonBean::CommonBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("CommonBean->CommonBean() 构造函数执行!");
    this->Init();
}



/**
 * @def 程序启动时初始化相关参数
 * @brief CommonBean::Init
 */
void CommonBean::Init()
{
    QLogHelper::instance()->LogInfo("CommonBean->Init() 函数执行!");
    comMethod=new CommonMethod();
    xmlOperate=new XMLOperate();
    //初始化设置Error Code，通过解析XML方式获取错误码
    getXmlOperate()->ReadXML();
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

QString CommonBean::getID() const
{
    return ID;
}

void CommonBean::setID(const QString &value)
{
    ID = value;
}

QString CommonBean::getIDType() const
{
    return IDType;
}

void CommonBean::setIDType(const QString &value)
{
    IDType = value;
}

QString CommonBean::getRelyID() const
{
    return RelyID;
}

void CommonBean::setRelyID(const QString &value)
{
    RelyID = value;
}

QString CommonBean::getRelyFilePath() const
{
    return RelyFilePath;
}

void CommonBean::setRelyFilePath(const QString &value)
{
    RelyFilePath = value;
}

QString CommonBean::getResultDirPath() const
{
    return ResultDirPath;
}

void CommonBean::setResultDirPath(const QString &value)
{
    ResultDirPath = value;
}

QString CommonBean::getMotDirPath() const
{
    return MotDirPath;
}

void CommonBean::setMotDirPath(const QString &value)
{
    MotDirPath = value;
}

QString CommonBean::getOutputDirPath() const
{
    return OutputDirPath;
}

void CommonBean::setOutputDirPath(const QString &value)
{
    OutputDirPath = value;
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
