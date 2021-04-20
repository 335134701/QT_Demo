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
    errCode=new QMap<QString,ERRCODETYPE>();
    xmlOperate=new XMLOperate();            //初始化设置Error Code，通过解析XML方式获取错误码
    excelOption=new ExcelOperation();
    ID=new QString();
    RelyID=new QString();
    IDType=new QString();
    RelyIDType=new QString();
    SVNDirPath=new QString();
    OutputDirPath=new QString();
    statusflag=false;
    //初始化错误码
    RExpression.insert("IDEdit","^EN(3[3-7]|42)\\d\\dP[A-Z]");
    RExpression.insert("RelyIDEdit","^EN(3[3-7]|42)\\d\\dP[A-Z]");
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
        //this->getErrCode()->insert(IDRelyID,this->getXmlOperate()->getErrCodeType().value(IDRelyID));
    }
    return ret;
}

QString CommonBean::getRelyFilePath() const
{
    return RelyFilePath;
}

void CommonBean::setRelyFilePath(const QString &value)
{
    RelyFilePath = value;
}

XMLOperate *CommonBean::getXmlOperate() const
{
    return xmlOperate;
}

void CommonBean::setXmlOperate(XMLOperate *value)
{
    xmlOperate = value;
}

QMap<QString, QString> CommonBean::getRExpression() const
{
    return RExpression;
}

QString CommonBean::getIniFilePath() const
{
    return IniFilePath;
}

void CommonBean::setIniFilePath(const QString &value)
{
    IniFilePath = value;
}

ExcelOperation *CommonBean::getExcelOption() const
{
    return excelOption;
}

void CommonBean::setExcelOption(ExcelOperation *value)
{
    excelOption = value;
}

QMap<QString, ERRCODETYPE> *CommonBean::getErrCode() const
{
    return errCode;
}

void CommonBean::setErrCode(QMap<QString, ERRCODETYPE> *value)
{
    errCode = value;
}

QString *CommonBean::getID() const
{
    return ID;
}

void CommonBean::setID(QString *value)
{
    ID = value;
}

QString *CommonBean::getRelyID() const
{
    return RelyID;
}

void CommonBean::setRelyID(QString *value)
{
    RelyID = value;
}

QString *CommonBean::getOutputDirPath() const
{
    return OutputDirPath;
}

void CommonBean::setOutputDirPath(QString *value)
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

QString CommonBean::getPFilePath() const
{
    return PFilePath;
}

void CommonBean::setPFilePath(const QString &value)
{
    PFilePath = value;
}

QString CommonBean::getSWFilePath() const
{
    return SWFilePath;
}

void CommonBean::setSWFilePath(const QString &value)
{
    SWFilePath = value;
}

QString CommonBean::getCarInfoFilePath() const
{
    return CarInfoFilePath;
}

void CommonBean::setCarInfoFilePath(const QString &value)
{
    CarInfoFilePath = value;
}

QString CommonBean::getCarMapFilePath() const
{
    return CarMapFilePath;
}

void CommonBean::setCarMapFilePath(const QString &value)
{
    CarMapFilePath = value;
}

QString CommonBean::getCarOSDFilePath() const
{
    return CarOSDFilePath;
}

void CommonBean::setCarOSDFilePath(const QString &value)
{
    CarOSDFilePath = value;
}

QString *CommonBean::getSVNDirPath() const
{
    return SVNDirPath;
}

void CommonBean::setSVNDirPath(QString *value)
{
    SVNDirPath = value;
}

QString *CommonBean::getIDType() const
{
    return IDType;
}

void CommonBean::setIDType(QString *value)
{
    IDType = value;
}

QString *CommonBean::getRelyIDType() const
{
    return RelyIDType;
}

void CommonBean::setRelyIDType(QString *value)
{
    RelyIDType = value;
}

unsigned int CommonBean::getStatusflag() const
{
    return statusflag;
}

void CommonBean::setStatusflag(unsigned int value)
{
    statusflag = value;
}
