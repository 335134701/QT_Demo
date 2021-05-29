#include "CommonBean.h"

CommonBean::CommonBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("CommonBean() 构造函数执行!");
    this->Init();
}

QMap<QString, QString> CommonBean::getRExpression() const
{
    return RExpression;
}

void CommonBean::setRExpression(const QMap<QString, QString> &value)
{
    RExpression = value;
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

bool CommonBean::getIDRelyIDflag() const
{
    return IDRelyIDflag;
}

void CommonBean::setIDRelyIDflag(bool value)
{
    IDRelyIDflag = value;
}

QString *CommonBean::getZIPFilePath() const
{
    return ZIPFilePath;
}

void CommonBean::setZIPFilePath(QString *value)
{
    ZIPFilePath = value;
}

/**
 * @def 程序启动时初始化相关参数
 * @brief CommonBean::Init
 */
void CommonBean::Init()
{
    QLogHelper::instance()->LogInfo("CommonBean->Init() 函数执行!");
    comMethod=new CommonMethod();
    softNumberTable=new QList<SOFTNUMBERTable>();
    configTable=new QList<CONFIGTable>();
    excelOption=new ExcelOperation();
    ID=new QString();
    RelyID=new QString();
    IDType=new QString();
    RelyIDType=new QString();
    RelyFilePath=new QString();
    IniFilePath=new QString();
    PFilePath=new QString();
    SWFilePath=new QString();
    //此变量与其他变量在生成新的机种时不需要初始化
    OutputDirPath=new QString();
    CarInfoFilePath=new QString();
    CarMapFilePath=new QString();
    CarOSDFilePath=new QString();
    JoinMot=new QString();
    APPMot=new QString();
    EEFilePath=new QString();
    ReadyFilePath=new QString();
    ConfigFilePath=new QString();
    ZIPFilePath=new QString();
    statusflag=0;
    IDRelyIDflag=true;
    //初始化错误码
    RExpression.insert("IDEdit","^EN(3[3-7]|42)\\d\\dP[A-Z]");
    RExpression.insert("RelyIDEdit","^EN(3[3-7]|42)\\d\\dP[A-Z]");
}

/**
 * @def 程序执行过程中初始化相关参数
 * @brief CommonBean::ResetParameter
 */
void CommonBean::ResetParameter()
{
    QLogHelper::instance()->LogInfo("CommonBean->ResetParameter() 函数执行!");
    softNumberTable=new QList<SOFTNUMBERTable>();
    configTable=new QList<CONFIGTable>();
    excelOption=new ExcelOperation();
    ID=new QString();
    IDType=new QString();
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
    statusflag=0;
    IDRelyIDflag=true;
}
