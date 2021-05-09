#include "ExcelOperateThread.h"

ExcelOperateThread::ExcelOperateThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("ExcelOperateThread() 构造函数执行!");
}
/**
 * @brief ExcelOperateThread::ExcelOperateThreadSlot
 * @param filePath
 */
void ExcelOperateThread::ExcelOperateThreadSlot(ExcelOperation *exl, const QString filePath, const QString ID, const QString IDType, unsigned int flag)
{
    QLogHelper::instance()->LogInfo("ExcelOperateThread->ExcelOperateThreadSlot() 函数执行!");
    QList<SOFTNUMBERTable> *softList=new QList<SOFTNUMBERTable>();
    QList<CONFIGTable> *confList=new QList<CONFIGTable>();
    QFile *file=new QFile();
    if(flag==RelyFileflag){
        if(file->exists(filePath)){
            (*softList)=exl->ReadSoftExcel(filePath,ID,IDType);
            (*softList)=this->DealSoftTable(*(softList),IDType);
        }
        emit EndExcelOperateThreadSoftSignal(*softList);
    }
    else if(flag==ConfigFileflag){
        if(file->exists(filePath)){
            (*confList)=exl->ReadConfExcel(filePath,ID);
        }
        emit EndExcelOperateThreadConfSignal((*confList));
    }
}

/**
 * @def 获取的ソフトウエア部品番号管理表(量産)_AKM対応用表部分属性需要进一步处理
 *      本函数功能是对量产管理表获取的属性进一步处理
 * @brief ExcelOperateThread::DealSoftTable
 * @param list
 * @return
 */
QList<SOFTNUMBERTable> ExcelOperateThread::DealSoftTable(QList<SOFTNUMBERTable> list,QString IDType)
{
    QLogHelper::instance()->LogInfo("ExcelOperateThread->DealSoftTable() 函数执行!");
    SOFTNUMBERTable soft;
    QString DiagnosticCode;
    if(list.size()<=0){return list;}
    for(int i=0;i<list.size();i++){
        if(!list[i].ApplicationPartNo.isEmpty()){soft.ApplicationPartNo=list[i].ApplicationPartNo;}
        else {list[i].ApplicationPartNo=soft.ApplicationPartNo;}

        if(!list[i].ApplicationVer.isEmpty()){soft.ApplicationVer=list[i].ApplicationVer;}
        else {list[i].ApplicationVer=soft.ApplicationVer;}

        if(!list[i].CarInfoPartNo.isEmpty()){soft.CarInfoPartNo=list[i].CarInfoPartNo;}
        else {list[i].CarInfoPartNo=soft.CarInfoPartNo;}

        if(!list[i].CarInfoVer.isEmpty()){soft.CarInfoVer=list[i].CarInfoVer;}
        else {list[i].CarInfoVer=soft.CarInfoVer;}

        if(!list[i].CameraMAPPartNo.isEmpty()){soft.CameraMAPPartNo=list[i].CameraMAPPartNo;}
        else {list[i].CameraMAPPartNo=soft.CameraMAPPartNo;}

        if(!list[i].CameraMAPVer.isEmpty()){soft.CameraMAPVer=list[i].CameraMAPVer;}
        else {list[i].CameraMAPVer=soft.CameraMAPVer;}

        if(!list[i].OSDPartNo.isEmpty()){soft.OSDPartNo=list[i].OSDPartNo;}
        else {list[i].OSDPartNo=soft.OSDPartNo;}

        if(!list[i].OSDVer.isEmpty()){soft.OSDVer=list[i].OSDVer;}
        else {list[i].OSDVer=soft.OSDVer;}

        if(!list[i].CANfblPartNo.isEmpty()){soft.CANfblPartNo=list[i].CANfblPartNo;}
        else {list[i].CANfblPartNo=soft.CANfblPartNo;}

        if(!list[i].CANfblVer.isEmpty()){soft.CANfblVer=list[i].CANfblVer;}
        else {list[i].CANfblVer=soft.CANfblVer;}

        if(!list[i].BootloaderPartNo.isEmpty()){soft.BootloaderPartNo=list[i].BootloaderPartNo;}
        else {list[i].BootloaderPartNo=soft.BootloaderPartNo;}

        if(!list[i].BootloaderVer.isEmpty()){soft.BootloaderVer=list[i].BootloaderVer;}
        else {list[i].BootloaderVer=soft.BootloaderVer;}

        if(list[i].DiagnosticCode.contains("診断識別コード")){
            DiagnosticCode=list[i].DiagnosticCode.left(list[i].DiagnosticCode.indexOf("/"));
            DiagnosticCode=DiagnosticCode.right(DiagnosticCode.indexOf("ード"));
        }
    }
    //未获取到诊断识别码
    if(DiagnosticCode.isEmpty()){
        if(IDType=="EntryAVM"){DiagnosticCode=0x09;}
        if(IDType=="EntryAVM2"){DiagnosticCode=0x0A;}
        if(IDType=="EntryIPA"){DiagnosticCode=0x0C;}
        if(IDType=="FAP"){DiagnosticCode=0x09;}
        if(IDType=="NextPH3"){DiagnosticCode=0x0D;}
    }
    //填充所有诊断识别码
    for(int i=0;i<list.size();i++){
        list[i].DiagnosticCode=DiagnosticCode;
    }
    return list;
}

void ExcelOperateThread::EEExcelWriteSlot(ExcelOperation *exl, const QString filePath, const QString ID,const QString IDType, QList<SOFTNUMBERTable> *softNumberTable)
{
    QLogHelper::instance()->LogInfo("ExcelOperateThread->EEExcelWriteSlot() 函数执行!");
    bool flag=true;
    QFile *file=new QFile();
    if(file->exists(filePath))
    {
        flag=exl->EEFileWrite(filePath,ID,IDType,softNumberTable);
    }else{
        flag=false;
    }
    emit EndEEExcelWriteSignal(flag);
}

void ExcelOperateThread::ReadyExcelWriteSlot(ExcelOperation *exl, const QString filePath, QList<SOFTNUMBERTable> *softNumberTable, QList<CONFIGTable> *configTable,QStringList DefineConfigList)
{
    QLogHelper::instance()->LogInfo("ExcelOperateThread->ReadyExcelWriteSlot() 函数执行!");
    bool flag=true;
    QFile *file=new QFile();
    if(file->exists(filePath))
    {
        flag=exl->ReadyFileWrite(filePath,softNumberTable,configTable,DefineConfigList);
    }else{
        flag=false;
    }
    emit EndReadyExcelWriteSignal(flag);
}
