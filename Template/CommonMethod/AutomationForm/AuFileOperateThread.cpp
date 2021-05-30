#include "AuFileOperateThread.h"


/**
 * @brief AuFileOperateThread::AuFileOperateThread
 * @param parent
 */
AuFileOperateThread::AuFileOperateThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("AuFileOperateThread 构造函数执行!");
}
