#include "AutomationFormBean.h"

/**
 * @brief AutomationFormBean::AutomationFormBean
 * @param parent
 */
AutomationFormBean::AutomationFormBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("LogFormBean 构造函数执行!");
    this->Init();
}

/**
 * @brief AutomationFormBean::getDefineConfigList
 * @return
 */
QStringList AutomationFormBean::getDefineConfigList() const
{
    return DefineConfigList;
}

/**
 * @brief AutomationFormBean::setDefineConfigList
 * @param value
 */
void AutomationFormBean::setDefineConfigList(const QStringList &value)
{
    DefineConfigList = value;
}

/**
 * @brief AutomationFormBean::getID
 * @return
 */
QString *AutomationFormBean::getID() const
{
    return ID;
}

/**
 * @brief AutomationFormBean::setID
 * @param value
 */
void AutomationFormBean::setID(QString *value)
{
    ID = value;
}

/**
 * @brief AutomationFormBean::getRelyID
 * @return
 */
QString *AutomationFormBean::getRelyID() const
{
    return RelyID;
}

/**
 * @brief AutomationFormBean::setRelyID
 * @param value
 */
void AutomationFormBean::setRelyID(QString *value)
{
    RelyID = value;
}

/**
 * @brief AutomationFormBean::getIDRelyIDflag
 * @return
 */
bool AutomationFormBean::getIDRelyIDflag() const
{
    return IDRelyIDflag;
}

/**
 * @brief AutomationFormBean::setIDRelyIDflag
 * @param value
 */
void AutomationFormBean::setIDRelyIDflag(bool value)
{
    IDRelyIDflag = value;
}

/**
 * @brief AutomationFormBean::getSVNDirPath
 * @return
 */
QString *AutomationFormBean::getSVNDirPath() const
{
    return SVNDirPath;
}

/**
 * @brief AutomationFormBean::setSVNDirPath
 * @param value
 */
void AutomationFormBean::setSVNDirPath(QString *value)
{
    SVNDirPath = value;
}

/**
 * @brief AutomationFormBean::getOutputDirPath
 * @return
 */
QString *AutomationFormBean::getOutputDirPath() const
{
    return OutputDirPath;
}

/**
 * @brief AutomationFormBean::setOutputDirPath
 * @param value
 */
void AutomationFormBean::setOutputDirPath(QString *value)
{
    OutputDirPath = value;
}

/**
 * @brief AutomationFormBean::getIDType
 * @return
 */
QString *AutomationFormBean::getIDType() const
{
    return IDType;
}

/**
 * @brief AutomationFormBean::setIDType
 * @param value
 */
void AutomationFormBean::setIDType(QString *value)
{
    IDType = value;
}

/**
 * @brief AutomationFormBean::getRelyIDType
 * @return
 */
QString *AutomationFormBean::getRelyIDType() const
{
    return RelyIDType;
}

/**
 * @brief AutomationFormBean::setRelyIDType
 * @param value
 */
void AutomationFormBean::setRelyIDType(QString *value)
{
    RelyIDType = value;
}

/**
 * @brief AutomationFormBean::getRelyFilePath
 * @return
 */
QString *AutomationFormBean::getRelyFilePath() const
{
    return RelyFilePath;
}

/**
 * @brief AutomationFormBean::setRelyFilePath
 * @param value
 */
void AutomationFormBean::setRelyFilePath(QString *value)
{
    RelyFilePath = value;
}

/**
 * @brief AutomationFormBean::getIniFilePath
 * @return
 */
QString *AutomationFormBean::getIniFilePath() const
{
    return IniFilePath;
}

/**
 * @brief AutomationFormBean::setIniFilePath
 * @param value
 */
void AutomationFormBean::setIniFilePath(QString *value)
{
    IniFilePath = value;
}

/**
 * @brief AutomationFormBean::getPFilePath
 * @return
 */
QString *AutomationFormBean::getPFilePath() const
{
    return PFilePath;
}

/**
 * @brief AutomationFormBean::setPFilePath
 * @param value
 */
void AutomationFormBean::setPFilePath(QString *value)
{
    PFilePath = value;
}

/**
 * @brief AutomationFormBean::getSWFilePath
 * @return
 */
QString *AutomationFormBean::getSWFilePath() const
{
    return SWFilePath;
}

/**
 * @brief AutomationFormBean::setSWFilePath
 * @param value
 */
void AutomationFormBean::setSWFilePath(QString *value)
{
    SWFilePath = value;
}

/**
 * @brief AutomationFormBean::getCarInfoFilePath
 * @return
 */
QString *AutomationFormBean::getCarInfoFilePath() const
{
    return CarInfoFilePath;
}

/**
 * @brief AutomationFormBean::setCarInfoFilePath
 * @param value
 */
void AutomationFormBean::setCarInfoFilePath(QString *value)
{
    CarInfoFilePath = value;
}

/**
 * @brief AutomationFormBean::getCarMapFilePath
 * @return
 */
QString *AutomationFormBean::getCarMapFilePath() const
{
    return CarMapFilePath;
}

/**
 * @brief AutomationFormBean::setCarMapFilePath
 * @param value
 */
void AutomationFormBean::setCarMapFilePath(QString *value)
{
    CarMapFilePath = value;
}

/**
 * @brief AutomationFormBean::getCarOSDFilePath
 * @return
 */
QString *AutomationFormBean::getCarOSDFilePath() const
{
    return CarOSDFilePath;
}

/**
 * @brief AutomationFormBean::setCarOSDFilePath
 * @param value
 */
void AutomationFormBean::setCarOSDFilePath(QString *value)
{
    CarOSDFilePath = value;
}

/**
 * @brief AutomationFormBean::getJoinFileMot
 * @return
 */
QString *AutomationFormBean::getJoinFileMot() const
{
    return JoinFileMot;
}

/**
 * @brief AutomationFormBean::setJoinFileMot
 * @param value
 */
void AutomationFormBean::setJoinFileMot(QString *value)
{
    JoinFileMot = value;
}

/**
 * @brief AutomationFormBean::getAPPFileMot
 * @return
 */
QString *AutomationFormBean::getAPPFileMot() const
{
    return APPFileMot;
}

/**
 * @brief AutomationFormBean::setAPPFileMot
 * @param value
 */
void AutomationFormBean::setAPPFileMot(QString *value)
{
    APPFileMot = value;
}

/**
 * @brief AutomationFormBean::getEEFilePath
 * @return
 */
QString *AutomationFormBean::getEEFilePath() const
{
    return EEFilePath;
}

/**
 * @brief AutomationFormBean::setEEFilePath
 * @param value
 */
void AutomationFormBean::setEEFilePath(QString *value)
{
    EEFilePath = value;
}

/**
 * @brief AutomationFormBean::getReadyFilePath
 * @return
 */
QString *AutomationFormBean::getReadyFilePath() const
{
    return ReadyFilePath;
}

/**
 * @brief AutomationFormBean::setReadyFilePath
 * @param value
 */
void AutomationFormBean::setReadyFilePath(QString *value)
{
    ReadyFilePath = value;
}

/**
 * @brief AutomationFormBean::getConfigFilePath
 * @return
 */
QString *AutomationFormBean::getConfigFilePath() const
{
    return ConfigFilePath;
}

/**
 * @brief AutomationFormBean::setConfigFilePath
 * @param value
 */
void AutomationFormBean::setConfigFilePath(QString *value)
{
    ConfigFilePath = value;
}

/**
 * @brief AutomationFormBean::getZIPFilePath
 * @return
 */
QString *AutomationFormBean::getZIPFilePath() const
{
    return ZIPFilePath;
}

/**
 * @brief AutomationFormBean::setZIPFilePath
 * @param value
 */
void AutomationFormBean::setZIPFilePath(QString *value)
{
    ZIPFilePath = value;
}

/**
 * @brief AutomationFormBean::getCommonMethod
 * @return
 */
CommonMethod *AutomationFormBean::getCommonMethod() const
{
    return commonMethod;
}

/**
 * @brief AutomationFormBean::setCommonMethod
 * @param value
 */
void AutomationFormBean::setCommonMethod(CommonMethod *value)
{
    commonMethod = value;
}

unsigned int AutomationFormBean::getAumationStatus() const
{
    return AumationStatus;
}

void AutomationFormBean::setAumationStatus(unsigned int value)
{
    AumationStatus = value;
}

/**
 * @brief AutomationFormBean::Init
 */
void AutomationFormBean::Init()
{
    QLogHelper::instance()->LogInfo("AutomationFormBean->Init() 函数执行!");    //机种番号
    AumationStatus=AU_READY;
    ID=new QString();
    RelyID=new QString();
    IDRelyIDflag=false;
    SVNDirPath=new QString();
    OutputDirPath=new QString();
    IDType=new QString();
    RelyIDType=new QString();
    RelyFilePath=new QString();
    IniFilePath=new QString();
    PFilePath=new QString();
    SWFilePath=new QString();
    CarInfoFilePath=new QString();
    CarMapFilePath=new QString();
    CarOSDFilePath=new QString();
    JoinFileMot=new QString();
    APPFileMot=new QString();
    EEFilePath=new QString();
    ReadyFilePath=new QString();
    ConfigFilePath=new QString();
    ZIPFilePath=new QString();
}

/**
 * @brief AutomationFormBean::ResetParameter
 * @param flag
 */
void AutomationFormBean::ResetParameter(unsigned int flag)
{
    QLogHelper::instance()->LogInfo("AutomationFormBean->ResetParameter() 函数执行!");
    switch (flag) {
    case RET_ID:
        AumationStatus=AU_READY;
        ID=new QString();
        IDRelyIDflag=false;
        IDType=new QString();
        RelyFilePath=new QString();
        IniFilePath=new QString();
        PFilePath=new QString();
        SWFilePath=new QString();
        CarInfoFilePath=new QString();
        CarMapFilePath=new QString();
        CarOSDFilePath=new QString();
        JoinFileMot=new QString();
        APPFileMot=new QString();
        EEFilePath=new QString();
        ReadyFilePath=new QString();
        ConfigFilePath=new QString();
        ZIPFilePath=new QString();
        break;
    case RET_SVNFilePath:
        SVNDirPath=new QString();
        break;
    case RET_OutPutFilePath:
        OutputDirPath=new QString();
        break;
    default:
        break;
    }
}
