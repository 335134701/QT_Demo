#include "FileThread.h"

FileThread::FileThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("FileThread() 构造函数执行!");
}
/**
 * @def 子线程处理文件搜索任务
 * @brief MyThread::FindFileThreadSlot
 * @param dirPath
 * @param FindFile
 * @param st
 * @param filters
 * @param flag
 */
void FileThread::FindFileThreadSlot(const QString dirPath,CommonMethod *commonMethod,const QStringList filters,unsigned int flag, bool goOn)
{
    QLogHelper::instance()->LogInfo("MyThread->FindFileThreadSlot() 函数执行!");
    QStringList qlist;
    //文件搜索
    qlist=commonMethod->FindFile(dirPath,filters);
    //线程处理结束
    emit EndFindFileThreadSignal(qlist,flag,goOn);
}

