#include "AuExcelOperateThread.h"


/**
 * @brief AuExcelOperateThread::AuExcelOperateThread
 * @param parent
 */
AuExcelOperateThread::AuExcelOperateThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SICommonMethod 构造函数执行!");
}
