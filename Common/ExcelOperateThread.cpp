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
    if(flag==3){
        //(*softList)=exl->ReadSoftExcel(filePath,ID,IDType);
        //emit EndExcelOperateThreadSoftSignal(softList);
    }
    else if(flag==14){
        //(*confList)=exl->ReadConfExcel(filePath,ID,IDType);
        //emit EndExcelOperateThreadConfSignal(confList);
    }
}
