#include "ExcelOperateThread.h"


/**
 * @brief ExcelOperateThread::ExcelOperateThread
 * @param parent
 */
ExcelOperateThread::ExcelOperateThread(QObject *parent) : QObject(parent)
{
     QLogHelper::instance()->LogInfo("ExcelOperateMethod 构造函数执行!");
}
