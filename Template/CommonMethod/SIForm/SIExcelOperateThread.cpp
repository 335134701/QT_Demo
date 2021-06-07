#include "SIExcelOperateThread.h"


/**
 * @brief SIExcelOperateThread::SIExcelOperateThread
 * @param parent
 */
SIExcelOperateThread::SIExcelOperateThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIExcelOperateMethod 构造函数执行!");
    this->Init();
}

/**
 * @brief SIExcelOperateThread::getSiExcelOperateMethod
 * @return
 */
SIExcelOperateMethod *SIExcelOperateThread::getSiExcelOperateMethod() const
{
    return siExcelOperateMethod;
}

/**
 * @brief SIExcelOperateThread::setSiExcelOperateMethod
 * @param value
 */
void SIExcelOperateThread::setSiExcelOperateMethod(SIExcelOperateMethod *value)
{
    siExcelOperateMethod = value;
}

/**
 * @brief SIExcelOperateThread::Init
 */
void SIExcelOperateThread::Init()
{
    QLogHelper::instance()->LogInfo("SIExcelOperateThread->Init() 函数执行!");
    siExcelOperateMethod=new SIExcelOperateMethod();
}


/**
 * @def 解析Excel表
 * @brief SIExcelOperateThread::ReadExcelThreadSlot
 * @param filePath
 * @param ID
 * @param IDType
 * @param flag
 */
void SIExcelOperateThread::ReadExcelThreadSlot(const QString filePath, const QString ID, const QString IDType, const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIExcelOperateThread->ReadExcelThreadSlot() 函数执行!");
    QList<SI_SOFTNUMBERTable> *softList;
    QList<SI_DEFINEMESSAGE> *defineList;
    //错误消息集合
    QList<SI_ERRORTable> *errList=new QList<SI_ERRORTable>();
    switch (flag) {
    case SIRelyFileflag:
        softList=new QList<SI_SOFTNUMBERTable>();
        if(QFile::exists(filePath)){
            (*softList)=siExcelOperateMethod->ReadSoftExcel(filePath,ID,IDType,errList);
            (*softList)=siExcelOperateMethod->DealSoftTable((*softList),ProducTionstage);
        }
        emit EndReadSoftExcelSignal(*softList,*errList);
        break;
    case SISHDefineFileflag:
        defineList=new QList<SI_DEFINEMESSAGE>();
        if(QFile::exists(filePath)){
            (*defineList)=siExcelOperateMethod->ReadDefineExcel(filePath,ID,IDType,errList);
        }
        emit EndReadDefineFileExcelSignal(*defineList,*errList);
    default:
        break;
    }
}

/**
 * @brief SIExcelOperateThread::InferRelyIDProcessSlot
 * @param relyFilePath
 * @param defineFilePath
 * @param ID
 * @param IDType
 * @param flag
 */
void SIExcelOperateThread::InferRelyIDProcessSlot(const QString relyFilePath, const QString defineFilePath, const QString ID, const QString IDType, const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIExcelOperateThread->InferRelyIDProcessSlot() 函数执行!");
    QList<SI_SOFTNUMBERTable> *softList;
    QList<SI_DEFINEMESSAGE> *defineList;
    //错误消息集合
    QList<SI_ERRORTable> *errList=new QList<SI_ERRORTable>();
    if(!QFile(relyFilePath).exists()&&!QFile(defineFilePath).exists()){return;}
    softList=new QList<SI_SOFTNUMBERTable>();
    defineList=new QList<SI_DEFINEMESSAGE>();
    switch (flag) {
    case 1:
        (*softList)=siExcelOperateMethod->ReadSoftExcel(relyFilePath,ID,IDType,errList);
        break;
    case 2:
        (*softList)=siExcelOperateMethod->ReadSoftExcel(relyFilePath,ID,IDType,errList);
        (*defineList)=siExcelOperateMethod->ReadDefineExcel(defineFilePath,ID,IDType,errList);
        break;
    case 11:
        
        break;
    default:
        break;
    }
}




