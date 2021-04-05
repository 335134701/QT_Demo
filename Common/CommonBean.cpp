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

UIMethod *CommonBean::getUiMethod() const
{
    return uiMethod;
}

void CommonBean::setUiMethod(UIMethod *value)
{
    uiMethod = value;
}

QMap<QString, ERRCODETYPE> CommonBean::getErrCode() const
{
    return errCode;
}


/**
 * @def 程序启动时初始化相关参数
 * @brief CommonBean::Init
 */
void CommonBean::Init()
{
    ID="";
    RelyID="";
    comMethod=new CommonMethod();
    uiMethod=new UIMethod();
    xmlOperate=new XMLOperate();
    RExpression.insert("IDEdit","^EN[3|4]\\d\\d\\dP[A-Z]");
}
/**
 * @def 程序运行过程中需要对部分参数进行初始化处理
 * @brief CommonBean::ParameterInit
 */
void CommonBean::ParameterInit()
{

}
