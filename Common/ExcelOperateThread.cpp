#include "ExcelOperateThread.h"

ExcelOperateThread::ExcelOperateThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("ExcelOperateThread() 构造函数执行!");
}
/**
 * @brief ExcelOperateThread::ExcelOperateThreadSlot
 * @param filePath
 */
void ExcelOperateThread::ExcelOperateThreadSlot(const QString filePath, const QString ID, const QString IDType, unsigned int flag)
{
    QLogHelper::instance()->LogInfo("ExcelOperateThread->ExcelOperateThreadSlot() 函数执行!");
    QList<SOFTNUMBERTable> softList;
    QList<CONFIGTable> confList;
    if(QFile::exists(filePath)){

    }
    if(flag==3){
        emit EndExcelOperateThreadSoftSignal(softList);
    }
    else if(flag==14){
        emit EndExcelOperateThreadConfSignal(confList);
    }
}
