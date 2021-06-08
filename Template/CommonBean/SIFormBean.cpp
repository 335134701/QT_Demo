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

/**
 * @brief SIFormBean::getSVNUpdateStatus
 * @return
 */
bool SIFormBean::getSVNUpdateStatus() const
{
    return SVNUpdateStatus;
}

/**
 * @brief SIFormBean::setSVNUpdateStatus
 * @param value
 */
void SIFormBean::setSVNUpdateStatus(bool value)
{
    SVNUpdateStatus = value;
}

/**
 * @brief SIFormBean::getIDType
 * @return
 */
QString *SIFormBean::getIDType() const
{
    return IDType;
}

/**
 * @brief SIFormBean::setIDType
 * @param value
 */
void SIFormBean::setIDType(QString *value)
{
    IDType = value;
}

/**
 * @brief SIFormBean::getRelyID
 * @return
 */
QString *SIFormBean::getRelyID() const
{
    return RelyID;
}

/**
 * @brief SIFormBean::setRelyID
 * @param value
 */
void SIFormBean::setRelyID(QString *value)
{
    RelyID = value;
}

/**
 * @brief SIFormBean::getRelyIDType
 * @return
 */
QString *SIFormBean::getRelyIDType() const
{
    return RelyIDType;
}

/**
 * @brief SIFormBean::setRelyIDType
 * @param value
 */
void SIFormBean::setRelyIDType(QString *value)
{
    RelyIDType = value;
}

/**
 * @brief SIFormBean::getSVNDirPath
 * @return
 */
QString *SIFormBean::getSVNDirPath() const
{
    return SVNDirPath;
}

/**
 * @brief SIFormBean::setSVNDirPath
 * @param value
 */
void SIFormBean::setSVNDirPath(QString *value)
{
    SVNDirPath = value;
}

/**
 * @brief SIFormBean::getOutputDirPath
 * @return
 */
QString *SIFormBean::getOutputDirPath() const
{
    return OutputDirPath;
}

/**
 * @brief SIFormBean::setOutputDirPath
 * @param value
 */
void SIFormBean::setOutputDirPath(QString *value)
{
    OutputDirPath = value;
}

/**
 * @brief SIFormBean::getRelyFilePath
 * @return
 */
QString *SIFormBean::getRelyFilePath() const
{
    return RelyFilePath;
}

/**
 * @brief SIFormBean::setRelyFilePath
 * @param value
 */
void SIFormBean::setRelyFilePath(QString *value)
{
    RelyFilePath = value;
}

/**
 * @brief SIFormBean::getPFilePath
 * @return
 */
QString *SIFormBean::getPFilePath() const
{
    return PFilePath;
}

/**
 * @brief SIFormBean::setPFilePath
 * @param value
 */
void SIFormBean::setPFilePath(QString *value)
{
    PFilePath = value;
}

/**
 * @brief SIFormBean::getSWFilePath
 * @return
 */
QString *SIFormBean::getSWFilePath() const
{
    return SWFilePath;
}

/**
 * @brief SIFormBean::setSWFilePath
 * @param value
 */
void SIFormBean::setSWFilePath(QString *value)
{
    SWFilePath = value;
}

/**
 * @brief SIFormBean::getCarInfoFilePath
 * @return
 */
QString *SIFormBean::getCarInfoFilePath() const
{
    return CarInfoFilePath;
}

/**
 * @brief SIFormBean::setCarInfoFilePath
 * @param value
 */
void SIFormBean::setCarInfoFilePath(QString *value)
{
    CarInfoFilePath = value;
}

/**
 * @brief SIFormBean::getID
 * @return
 */
QString *SIFormBean::getID() const
{
    return ID;
}

/**
 * @brief SIFormBean::setID
 * @param value
 */
void SIFormBean::setID(QString *value)
{
    ID = value;
}

/**
 * @brief SIFormBean::getIDRelyIDflag
 * @return
 */
bool SIFormBean::getIDRelyIDflag() const
{
    return IDRelyIDflag;
}

/**
 * @brief SIFormBean::setIDRelyIDflag
 * @param value
 */
void SIFormBean::setIDRelyIDflag(bool value)
{
    IDRelyIDflag = value;
}

/**
 * @brief SIFormBean::getCommonMethod
 * @return
 */
CommonMethod *SIFormBean::getCommonMethod() const
{
    return commonMethod;
}

/**
 * @brief SIFormBean::setCommonMethod
 * @param value
 */
void SIFormBean::setCommonMethod(CommonMethod *value)
{
    commonMethod = value;
}

/**
 * @brief SIFormBean::getCodeFilePath
 * @return
 */
QString *SIFormBean::getCodeFilePath() const
{
    return CodeFilePath;
}

/**
 * @brief SIFormBean::setCodeFilePath
 * @param value
 */
void SIFormBean::setCodeFilePath(QString *value)
{
    CodeFilePath = value;
}

/**
 * @brief SIFormBean::getBuildFilePath
 * @return
 */
QString *SIFormBean::getBuildFilePath() const
{
    return BuildFilePath;
}

/**
 * @brief SIFormBean::setBuildFilePath
 * @param value
 */
void SIFormBean::setBuildFilePath(QString *value)
{
    BuildFilePath = value;
}

/**
 * @brief SIFormBean::getSHDefineFilePath
 * @return
 */
QString *SIFormBean::getSHDefineFilePath() const
{
    return SHDefineFilePath;
}

/**
 * @brief SIFormBean::getErrList
 * @return
 */
QList<SI_ERRORTable> *SIFormBean::getErrList() const
{
    return ErrList;
}

/**
 * @brief SIFormBean::setErrList
 * @param value
 */
void SIFormBean::setErrList(QList<SI_ERRORTable> *value)
{
    ErrList = value;
}

/**
 * @brief SIFormBean::getSoftList
 * @return
 */
QList<SI_SOFTNUMBERTable> *SIFormBean::getSoftList() const
{
    return SoftList;
}

/**
 * @brief SIFormBean::setSoftList
 * @param value
 */
void SIFormBean::setSoftList(QList<SI_SOFTNUMBERTable> *value)
{
    SoftList = value;
}

/**
 * @brief SIFormBean::getBAflag
 * @return
 */
bool SIFormBean::getBAflag() const
{
    return BAflag;
}

/**
 * @brief SIFormBean::setBAflag
 * @param value
 */
void SIFormBean::setBAflag(bool value)
{
    BAflag = value;
}

/**
 * @brief SIFormBean::getUnzipflag
 * @return
 */
bool SIFormBean::getUnzipflag() const
{
    return Unzipflag;
}

/**
 * @brief SIFormBean::setUnzipflag
 * @param value
 */
void SIFormBean::setUnzipflag(bool value)
{
    Unzipflag = value;
}

/**
 * @brief SIFormBean::getDefineList
 * @return
 */
QList<SI_DEFINEMESSAGE> *SIFormBean::getDefineList() const
{
    return DefineList;
}

/**
 * @brief SIFormBean::setDefineList
 * @param value
 */
void SIFormBean::setDefineList(QList<SI_DEFINEMESSAGE> *value)
{
    DefineList = value;
}

/**
 * @brief SIFormBean::getIsSearchRelyIDflag
 * @return
 */
bool SIFormBean::getIsSearchRelyIDflag() const
{
    return isSearchRelyIDflag;
}

/**
 * @brief SIFormBean::setIsSearchRelyIDflag
 * @param value
 */
void SIFormBean::setIsSearchRelyIDflag(bool value)
{
    isSearchRelyIDflag = value;
}

bool SIFormBean::getCopyCodeflag() const
{
    return CopyCodeflag;
}

void SIFormBean::setCopyCodeflag(bool value)
{
    CopyCodeflag = value;
}
/**
 * @brief SIFormBean::getZIPflag
 * @return
 */
bool SIFormBean::getZIPflag() const
{
    return ZIPflag;
}
/**
 * @brief SIFormBean::setZIPflag
 * @param value
 */
void SIFormBean::setZIPflag(bool value)
{
    ZIPflag = value;
}
/**
 * @brief SIFormBean::getRelyIDSoftList
 * @return
 */
QList<SI_SOFTNUMBERTable> *SIFormBean::getRelyIDSoftList() const
{
    return RelyIDSoftList;
}

/**
 * @brief SIFormBean::setRelyIDSoftList
 * @param value
 */
void SIFormBean::setRelyIDSoftList(QList<SI_SOFTNUMBERTable> *value)
{
    RelyIDSoftList = value;
}

bool SIFormBean::getNEWCLflag() const
{
    return NEWCLflag;
}

void SIFormBean::setNEWCLflag(bool value)
{
    NEWCLflag = value;
}

QStandardItemModel *SIFormBean::getMessageViewModel() const
{
    return messageViewModel;
}

void SIFormBean::setMessageViewModel(QStandardItemModel *value)
{
    messageViewModel = value;
}


/**
 * @def 初始化函数
 * @brief SIFormBean::Init
 */
void SIFormBean::Init()
{
    QLogHelper::instance()->LogInfo("SIFormBean->Init() 函数执行!");
    SIStatus=SI_READY;
    ID=new QString();
    IDType=new QString();
    RelyID=new QString();
    RelyIDType=new QString();
    IDRelyIDflag=true;
    CodeFilePath=new QString();
    SVNDirPath=new QString();
    OutputDirPath=new QString();
    RelyFilePath=new QString();
    PFilePath=new QString();
    SWFilePath=new QString();
    CarInfoFilePath=new QString();
    BuildFilePath=new QString();
    SHDefineFilePath=new QString();
    SVNUpdateStatus=false;
    BAflag=false;
    Unzipflag=false;
    isSearchRelyIDflag=false;
    CopyCodeflag=false;
    ZIPflag=false;
    NEWCLflag=false;
    SoftList=new QList<SI_SOFTNUMBERTable>();
    RelyIDSoftList=new QList<SI_SOFTNUMBERTable>();
    ErrList=new QList<SI_ERRORTable>();
    DefineList=new QList<SI_DEFINEMESSAGE>();
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
        SIStatus=SI_READY;
        ID=new QString();
        IDType=new QString();
        IDRelyIDflag=true;
        CodeFilePath=new QString();
        RelyFilePath=new QString();
        PFilePath=new QString();
        SWFilePath=new QString();
        CarInfoFilePath=new QString();
        BuildFilePath=new QString();
        SHDefineFilePath=new QString();
        BAflag=false;
        Unzipflag=false;
        isSearchRelyIDflag=false;
        CopyCodeflag=false;
        ZIPflag=false;
        NEWCLflag=false;
        SoftList=new QList<SI_SOFTNUMBERTable>();
        RelyIDSoftList=new QList<SI_SOFTNUMBERTable>();
        ErrList=new QList<SI_ERRORTable>();
        DefineList=new QList<SI_DEFINEMESSAGE>();
        break;
    case RET_RELYID:
        RelyID=new QString();
        RelyIDType=new QString();
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

