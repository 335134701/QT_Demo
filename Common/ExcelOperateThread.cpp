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
    QString DiagnosticCode;
    if(flag==3){
        (*softList)=exl->ReadSoftExcel(filePath,ID,IDType);
        (*softList)=this->DealSoftTable(*(softList));
        QLogHelper::instance()->LogDebug((*softList)[0].DiagnosticCode);
        //如果还是未获取诊断码，则人为赋值诊断码
        if(softList->size()>0&&(*softList)[0].DiagnosticCode.isEmpty()){
            if(IDType=="EntryAVM"){DiagnosticCode=0x09;}
            if(IDType=="EntryAVM2"){DiagnosticCode=0x09;}
            //if(IDType=="EntryAVM"){DiagnosticCode=0x09;}
            //if(IDType=="EntryAVM"){DiagnosticCode=0x09;}
            //if(IDType=="EntryAVM"){DiagnosticCode=0x09;}
            //if(IDType=="EntryAVM"){DiagnosticCode=0x09;}
            //if(IDType=="EntryAVM"){DiagnosticCode=0x09;}
        }
        for(int i=0;i<softList->size();i++){
            (*softList)[i].DiagnosticCode=DiagnosticCode;
        }
        emit EndExcelOperateThreadSoftSignal(*softList);
    }
    else if(flag==14){
        (*confList)=exl->ReadConfExcel(filePath,ID,IDType);
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
/*
    QString ModelNumber;            //クラリオン機種番号
    QString CarModels;              //車種仕向け
    QString PartNumber;             //日産部番
    QString CANGen;                 //CAN世代
    QString Productionstage;        //生産段階
    QString ApplicationPartNo;      //Application PartsNo
    QString ApplicationVer;         //Application Ver
    QString CarInfoPartNo;          //Car_Info PartsNo
    QString CarInfoVer;             //Car_Info
    QString CameraMAPPartNo;        //CameraMAP PartsNo
    QString CameraMAPVer;           //CameraMAP Ver
    QString OSDPartNo;              //OSD PartsNo
    QString OSDVer;                 //OSD Ver
    QString CANfblPartNo;           //CANfbl PartsNo
    QString CANfblVer;              //CANfbl Ver
    QString BootloaderPartNo;       //Bootloader PartsNo
    QString BootloaderVer;          //Bootloader Ver
    QString DiagnosticCode;         //診断識別コード
 */
QList<SOFTNUMBERTable> ExcelOperateThread::DealSoftTable(QList<SOFTNUMBERTable> list)
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

        //QLogHelper::instance()->LogDebug(list[i].DiagnosticCode.mid(0,list[i].DiagnosticCode.indexOf("/")));
        if(list[i].DiagnosticCode.contains("診断識別コード")){
            DiagnosticCode=list[i].DiagnosticCode.mid(list[i].DiagnosticCode.lastIndexOf("診断識別コード"));
            DiagnosticCode=DiagnosticCode.mid(DiagnosticCode.indexOf("/"));
        }
    }
    for(int i=0;i<list.size();i++){
        list[i].DiagnosticCode=DiagnosticCode;
    }
    return list;
}
