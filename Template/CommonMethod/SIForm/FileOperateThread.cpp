#include "FileOperateThread.h"


/**
 * @brief FileOperateThread::FileOperateThread
 * @param parent
 */
FileOperateThread::FileOperateThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("FileOperateThread 构造函数执行!");
}
