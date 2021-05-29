#include "SIFormBean.h"

/**
 * @brief SIFormBean::SIFormBean
 * @param parent
 */
SIFormBean::SIFormBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIFormBean 构造函数执行!");
    this->Init();
}
/**
 * @brief SIFormBean::getSIStatus
 * @return
 */
unsigned int SIFormBean::getSIStatus() const
{
    return SIStatus;
}
/**
 * @brief SIFormBean::setSIStatus
 * @param value
 */
void SIFormBean::setSIStatus(unsigned int value)
{
    SIStatus = value;
}


bool SIFormBean::getSVNUpdateStatus() const
{
    return SVNUpdateStatus;
}

void SIFormBean::setSVNUpdateStatus(bool value)
{
    SVNUpdateStatus = value;
}

QString *SIFormBean::getIDType() const
{
    return IDType;
}

void SIFormBean::setIDType(QString *value)
{
    IDType = value;
}

QString *SIFormBean::getRelyID() const
{
    return RelyID;
}

void SIFormBean::setRelyID(QString *value)
{
    RelyID = value;
}

QString *SIFormBean::getRelyIDType() const
{
    return RelyIDType;
}

void SIFormBean::setRelyIDType(QString *value)
{
    RelyIDType = value;
}

QString *SIFormBean::getSVNDirPath() const
{
    return SVNDirPath;
}

void SIFormBean::setSVNDirPath(QString *value)
{
    SVNDirPath = value;
}

QString *SIFormBean::getOutputDirPath() const
{
    return OutputDirPath;
}

void SIFormBean::setOutputDirPath(QString *value)
{
    OutputDirPath = value;
}

QString *SIFormBean::getRelyFilePath() const
{
    return RelyFilePath;
}

void SIFormBean::setRelyFilePath(QString *value)
{
    RelyFilePath = value;
}

QString *SIFormBean::getPFilePath() const
{
    return PFilePath;
}

void SIFormBean::setPFilePath(QString *value)
{
    PFilePath = value;
}

QString *SIFormBean::getSWFilePath() const
{
    return SWFilePath;
}

void SIFormBean::setSWFilePath(QString *value)
{
    SWFilePath = value;
}

QString *SIFormBean::getCarInfoFilePath() const
{
    return CarInfoFilePath;
}

void SIFormBean::setCarInfoFilePath(QString *value)
{
    CarInfoFilePath = value;
}

QString *SIFormBean::getID() const
{
    return ID;
}

void SIFormBean::setID(QString *value)
{
    ID = value;
}

bool SIFormBean::getIDRelyIDflag() const
{
    return IDRelyIDflag;
}

void SIFormBean::setIDRelyIDflag(bool value)
{
    IDRelyIDflag = value;
}

CommonMethod *SIFormBean::getCommonMethod() const
{
    return commonMethod;
}

void SIFormBean::setCommonMethod(CommonMethod *value)
{
    commonMethod = value;
}


/**
 * @def 初始化函数
 * @brief SIFormBean::Init
 */
void SIFormBean::Init()
{
    QLogHelper::instance()->LogInfo("SIFormBean->Init() 函数执行!");
    SIStatus=0;
    ID=new QString();
    IDType=new QString();
    RelyID=new QString();
    RelyIDType=new QString();
    IDRelyIDflag=true;
    SVNDirPath=new QString();
    OutputDirPath=new QString();
    RelyFilePath=new QString();
    PFilePath=new QString();
    SWFilePath=new QString();
    CarInfoFilePath=new QString();
    SVNUpdateStatus=false;

}

/**
 * @def 根据需要初始化部分参数
 * @brief SIFormBean::ResetParameter
 * @param flag
 */
void SIFormBean::ResetParameter(unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIFormBean->ResetParameter() 函数执行!");
    switch (flag) {
    case RET_ID:
        SIStatus=0;
        ID=new QString();
        IDType=new QString();
        IDRelyIDflag=true;
        RelyFilePath=new QString();
        PFilePath=new QString();
        SWFilePath=new QString();
        CarInfoFilePath=new QString();
        break;
    case RET_PATH:

        break;
    default:
        break;
    }
}
