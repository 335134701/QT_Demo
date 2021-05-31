#include "SIFileOperateThread.h"


/**
 * @brief SIFileOperateThread::SIFileOperateThread
 * @param parent
 */
SIFileOperateThread::SIFileOperateThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread 构造函数执行!");
    this->Init();
}

/**
 * @brief SIFileOperateThread::Init
 */
void SIFileOperateThread::Init()
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->Init() 函数执行!");
    siFileOperateMethod=new SIFileOperateMethod();
}

/**
 * @def 处理SVN更新任务
 * @brief SIFileOperateThread::UpdateSVNSlot
 * @param exeFilePath
 * @param dirPath
 */
void SIFileOperateThread::UpdateSVNSlot(const QString exeFilePath, const QString dirPath)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->UpdateSVNSlot() 函数执行!");
    QFile *file;
    QProcess *p=new QProcess();
    QStringList args;
    QString tmp=dirPath;
    bool flag=false;
    if(!exeFilePath.isEmpty()&&!tmp.isEmpty()){
        if(file->exists(tmp+"/trunk")){tmp=tmp+"/trunk";}
        if(file->exists(tmp+"/01REQ")){tmp=tmp+"/01REQ";}
        p->setProcessChannelMode(QProcess::MergedChannels);
        //先执行清理
        args.append("cleanup");
        args.append(dirPath);
        p->start(exeFilePath, args);
        flag=p->waitForFinished();
        QLogHelper::instance()->LogDebug("----SVN清理任务执行完成 ： "+QString::number(flag));
        //清理执行完成后执行更新任务
        if(flag){
            args.clear();
            args.append("update");
            args.append(dirPath);
            p->start(exeFilePath, args);
            flag=p->waitForFinished();
        }
        QLogHelper::instance()->LogDebug("----SVNUpdate任务执行完成 ： "+QString::number(flag));
    }
    emit EndUpdateSVNSignal(flag);
}

/**
 * @brief SIFileOperateThread::FileSearchSlot
 * @param dirPath
 * @param filters
 * @param ID
 * @param IDType
 * @param flag
 * @param isGoON
 */
void SIFileOperateThread::FileSearchSlot(const QString dirPath,const QStringList filters,const QString ID,QString IDType,unsigned int flag, bool isGoON)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->FileSearchSlot() 函数执行!");
    QStringList filePaths;
    QString tmpDirPath=dirPath;
    //文件路径根据flag进行细化
    tmpDirPath=siFileOperateMethod->AnalyzePath(tmpDirPath,ID,IDType,flag);
    //文件搜索执行
    filePaths=siFileOperateMethod->SearchFile(tmpDirPath,filters);
    //对搜索到的文件进行处理,得到指定文件
    tmpDirPath=siFileOperateMethod->AnalyzeFilePath(filePaths,flag,ID);
    QLogHelper::instance()->LogDebug(tmpDirPath);
    emit EndFileSearcSignal(tmpDirPath,flag,isGoON);
}
