#include "SIFileOperateThread.h"


/**
 * @brief SIFileOperateThread::SIFileOperateThread
 * @param parent
 */
SIFileOperateThread::SIFileOperateThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread 构造函数执行!");
}
