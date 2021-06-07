#include "AuFileOperateThread.h"


/**
 * @brief AuFileOperateThread::AuFileOperateThread
 * @param parent
 */
AuFileOperateThread::AuFileOperateThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("AuFileOperateThread 构造函数执行!");
    this->Init();
}

/**
 * @brief AuFileOperateThread::Init
 */
void AuFileOperateThread::Init()
{
    QLogHelper::instance()->LogInfo("AuFileOperateThread->Init() 函数执行!");
    auFileOperateMethod=new AuFileOperateMethod();
}

/**
 * @brief AuFileOperateThread::FileSearchSlot
 * @param dirPath
 * @param filters
 * @param ID
 * @param IDType
 * @param flag
 * @param isGoON
 */
void AuFileOperateThread::FileSearchSlot(const QString dirPath, const QStringList filters, const QString ID, QString IDType, unsigned int flag, bool isGoON)
{
    QLogHelper::instance()->LogInfo("AuFileOperateThread->FileSearchSlot() 函数执行!");
    QStringList filePaths;
    QString tmpDirPath=dirPath;
    //文件路径根据flag进行细化
    tmpDirPath=auFileOperateMethod->AnalyzePath(tmpDirPath,ID,IDType,flag);
    //文件搜索执行
    filePaths=auFileOperateMethod->SearchFile(tmpDirPath,filters);
    //对搜索到的文件进行处理,得到指定文件
    tmpDirPath=auFileOperateMethod->AnalyzeFilePath(filePaths,flag,ID);
    QLogHelper::instance()->LogDebug(tmpDirPath);
    emit EndFileSearcSignal(tmpDirPath,flag,isGoON);
}
