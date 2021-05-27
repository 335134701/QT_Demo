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

/**
 * @def 子线程处理文件压缩任务
 * @brief FileThread::FileCompressionSlot
 * @param exeFilePath
 * @param dirPath
 */
void FileThread::FileCompressionSlot(const QString exeFilePath, const QString dirPath)
{
    QLogHelper::instance()->LogInfo("MyThread->FileCompressionSlot() 函数执行!");
    QStringList args;
    QString flag;
    QString currentDirPath=dirPath.left(dirPath.lastIndexOf("/"));
    if(!exeFilePath.isEmpty()&&!dirPath.isEmpty()){
        QProcess p(0);
        args.append("a");
        args.append(dirPath.mid(dirPath.lastIndexOf("/")+1)+".7z");
        args.append(dirPath);
        p.setWorkingDirectory(currentDirPath);
        p.start(exeFilePath, args);
        if(p.waitForFinished()){
            flag=dirPath+".7z";
        }
    }
    emit EndFileCompressionSignal(flag);
}

