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
    softNumberTable=new QList<SOFTNUMBERTable>();
    configTable=new QList<CONFIGTable>();
    xmlOperate=new XMLOperate();            //初始化设置Error Code，通过解析XML方式获取错误码
    excelOption=new ExcelOperation();
    ID=new QString();
    RelyID=new QString();
    IDType=new QString();
    RelyIDType=new QString();
    SVNDirPath=new QString();
    OutputDirPath=new QString();
    RelyFilePath=new QString();
    IniFilePath=new QString();
    PFilePath=new QString();
    SWFilePath=new QString();
    CarInfoFilePath=new QString();
    CarMapFilePath=new QString();
    CarOSDFilePath=new QString();
    JoinMot=new QString();
    APPMot=new QString();
    EEFilePath=new QString();
    ReadyFilePath=new QString();
    ConfigFilePath=new QString();
    messageViewModel=new QStandardItemModel();
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
    errCode->clear();
    *RelyFilePath="";
    *IniFilePath="";
    *PFilePath="";
    *SWFilePath="";
    *CarInfoFilePath="";
    *CarMapFilePath="";
    *CarOSDFilePath="";
    *JoinMot="";
    *APPMot="";
    *EEFilePath="";
    *ReadyFilePath="";
    *ConfigFilePath="";
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

QMap<QString, QString> CommonBean::getRExpression() const
{
    return RExpression;
}

void CommonBean::setRExpression(const QMap<QString, QString> &value)
{
    RExpression = value;
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

QString *CommonBean::getSVNDirPath() const
{
    return SVNDirPath;
}

void CommonBean::setSVNDirPath(QString *value)
{
    SVNDirPath = value;
}

QString *CommonBean::getOutputDirPath() const
{
    return OutputDirPath;
}

void CommonBean::setOutputDirPath(QString *value)
{
    OutputDirPath = value;
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

QString *CommonBean::getRelyFilePath() const
{
    return RelyFilePath;
}

void CommonBean::setRelyFilePath(QString *value)
{
    RelyFilePath = value;
}

QString *CommonBean::getIniFilePath() const
{
    return IniFilePath;
}

void CommonBean::setIniFilePath(QString *value)
{
    IniFilePath = value;
}

QString *CommonBean::getPFilePath() const
{
    return PFilePath;
}

void CommonBean::setPFilePath(QString *value)
{
    PFilePath = value;
}

QString *CommonBean::getSWFilePath() const
{
    return SWFilePath;
}

void CommonBean::setSWFilePath(QString *value)
{
    SWFilePath = value;
}

QString *CommonBean::getCarInfoFilePath() const
{
    return CarInfoFilePath;
}

void CommonBean::setCarInfoFilePath(QString *value)
{
    CarInfoFilePath = value;
}

QString *CommonBean::getCarMapFilePath() const
{
    return CarMapFilePath;
}

void CommonBean::setCarMapFilePath(QString *value)
{
    CarMapFilePath = value;
}

QString *CommonBean::getCarOSDFilePath() const
{
    return CarOSDFilePath;
}

void CommonBean::setCarOSDFilePath(QString *value)
{
    CarOSDFilePath = value;
}

QString *CommonBean::getJoinMot() const
{
    return JoinMot;
}

void CommonBean::setJoinMot(QString *value)
{
    JoinMot = value;
}

QString *CommonBean::getAPPMot() const
{
    return APPMot;
}

void CommonBean::setAPPMot(QString *value)
{
    APPMot = value;
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

unsigned int CommonBean::getStatusflag() const
{
    return statusflag;
}

void CommonBean::setStatusflag(unsigned int value)
{
    statusflag = value;
}

ExcelOperation *CommonBean::getExcelOption() const
{
    return excelOption;
}

void CommonBean::setExcelOption(ExcelOperation *value)
{
    excelOption = value;
}

QString *CommonBean::getEEFilePath() const
{
    return EEFilePath;
}

void CommonBean::setEEFilePath(QString *value)
{
    EEFilePath = value;
}

QString *CommonBean::getReadyFilePath() const
{
    return ReadyFilePath;
}

void CommonBean::setReadyFilePath(QString *value)
{
    ReadyFilePath = value;
}

QString *CommonBean::getConfigFilePath() const
{
    return ConfigFilePath;
}

void CommonBean::setConfigFilePath(QString *value)
{
    ConfigFilePath = value;
}

QList<SOFTNUMBERTable> *CommonBean::getSoftNumberTable() const
{
    return softNumberTable;
}

void CommonBean::setSoftNumberTable(QList<SOFTNUMBERTable> *value)
{
    softNumberTable = value;
}

QList<CONFIGTable> *CommonBean::getConfigTable() const
{
    return configTable;
}

void CommonBean::setConfigTable(QList<CONFIGTable> *value)
{
    configTable = value;
}

QStandardItemModel *CommonBean::getMessageViewModel() const
{
    return messageViewModel;
}

void CommonBean::setMessageViewModel(QStandardItemModel *value)
{
    messageViewModel = value;
}

bool CommonBean::getTableViewEditflag() const
{
    return tableViewEditflag;
}

void CommonBean::setTableViewEditflag(bool value)
{
    tableViewEditflag = value;
}

QStringList CommonBean::getDefineConfigList() const
{
    return DefineConfigList;
}

void CommonBean::setDefineConfigList(const QStringList &value)
{
    DefineConfigList = value;
}

