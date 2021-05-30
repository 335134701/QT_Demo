#include "SIExcelOperateThread.h"


/**
 * @brief SIExcelOperateThread::SIExcelOperateThread
 * @param parent
 */
SIExcelOperateThread::SIExcelOperateThread(QObject *parent) : QObject(parent)
{
     QLogHelper::instance()->LogInfo("SIExcelOperateMethod 构造函数执行!");
}
