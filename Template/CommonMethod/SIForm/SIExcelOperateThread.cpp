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
 * @brief SIExcelOperateThread::Init
 */
void SIExcelOperateThread::Init()
{
    QLogHelper::instance()->LogInfo("SIExcelOperateThread->Init() 函数执行!");
}
