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
 * @brief SIFileOperateThread::getSiFileOperateMethod
 * @return
 */
SIFileOperateMethod *SIFileOperateThread::getSiFileOperateMethod() const
{
    return siFileOperateMethod;
}

/**
 * @brief SIFileOperateThread::setSiFileOperateMethod
 * @param value
 */
void SIFileOperateThread::setSiFileOperateMethod(SIFileOperateMethod *value)
{
    siFileOperateMethod = value;
}

/**
 * @brief SIFileOperateThread::Init
 */
void SIFileOperateThread::Init()
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->Init() 函数执行!");
    siFileOperateMethod=new SIFileOperateMethod();
    siCommonMethod=new SICommonMethod();
    siFileOperateMethod->setSiCommonMethod(siCommonMethod);
    commonMethod=new CommonMethod();
    siFileOperateMethod->setCommonMethod(commonMethod);
}

/**
 * @def SVN更新任务处理
 * @brief SIFileOperateThread::UpdateSVNSlot
 * @param exeFilePath
 * @param dirPath
 * @param flag
 */
void SIFileOperateThread::UpdateSVNSlot(const QString exeFilePath, const QString dirPath,const unsigned flag)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->RunOrderSignal() 函数执行!");
    QFile *file;
    QProcess *p=new QProcess();
    QStringList args;
    QString tmp=dirPath;
    bool result=false;
    if(file->exists(exeFilePath)&&!tmp.isEmpty()){
        if(file->exists(tmp+"/trunk")){tmp=tmp+"/trunk";}
        if(file->exists(tmp+"/01REQ")){tmp=tmp+"/01REQ";}
        p->setProcessChannelMode(QProcess::MergedChannels);
        //先执行清理
        args.append("cleanup");
        args.append(dirPath);
        p->start(exeFilePath, args);
        result=p->waitForFinished();
        //清理执行完成后执行更新任务
        if(result){
            args.clear();
            args.append("update");
            args.append(dirPath);
            p->start(exeFilePath, args);
            //等待1分钟，1分钟未更新完成，则默认更新失败
            result=p->waitForFinished(60000);
        }
    }
    emit EndRunOrderSignal(result,flag);
}

/**
 * @def 解压文件任务处理
 * @brief SIFileOperateThread::UNZipCodeFileSlot
 * @param exeFilePath
 * @param filePath
 * @param desDirPath
 * @param flag
 */
void SIFileOperateThread::UNZipCodeFileSlot(const QString exeFilePath, const QString filePath,const QString desDirPath, const unsigned flag)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->UNZipCodeFileSlot() 函数执行!");
    QFile *file;
    QProcess *p=new QProcess();
    QStringList args;
    QString tmp=desDirPath+"/"+ProjectName;
    bool result=false;
    if(file->exists(exeFilePath)&&file->exists(filePath)){
        p->setProcessChannelMode(QProcess::MergedChannels);
        args.append("x");
        args.append(filePath);
        args.append("-y");
        args.append("-o"+tmp);
        p->start(exeFilePath, args);
        result=p->waitForFinished(-1);
    }
    if(result){
        result=siFileOperateMethod->UNZipCodeDearDirSignal(tmp);
    }
    emit EndRunOrderSignal(result,flag);
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
    tmpDirPath=siFileOperateMethod->AnalyzeFilePath(filePaths,flag,ID,IDType);
    QLogHelper::instance()->LogDebug(tmpDirPath);
    emit EndFileSearcSignal(tmpDirPath,flag,isGoON);
}

/**
 * @brief SIFileOperateThread::CheckFileSlot
 * @param dirPath
 * @param filters
 * @param ID
 * @param IDType
 * @param flag
 * @param softList
 */
void SIFileOperateThread::CheckFileSlot(const QString dirPath,const QString ID,const QString IDType,const unsigned int flag,const QList<SI_SOFTNUMBERTable> softList)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->CheckFileSignalSlot() 函数执行!");
    bool tmpflag=false;
    QStringList tmpDirPathList;
    //错误消息集合
    QList<SI_ERRORTable> *errList=new QList<SI_ERRORTable>();
    switch (flag) {
    case SIBADirflag:
        //文件路径根据flag进行细化
        tmpDirPathList=siCommonMethod->GetBeforeAfterDirPath(siFileOperateMethod->AnalyzePath(dirPath,ID,IDType,flag),softList);
        if(tmpDirPathList.size()>=2){
            if(!QDir(tmpDirPathList[0]).exists()){
                errList->append(SICommonMethod::SetERRMessage(tmpDirPathList[0],tmpDirPathList[0].mid(tmpDirPathList[0].lastIndexOf("/")+1)+" 文件夹版本号信息错误!"));
                emit EndCheckFileSignal(flag,tmpflag,*errList);
                return;
            }
            if(!QDir(tmpDirPathList[1]).exists()){
                errList->append(SICommonMethod::SetERRMessage(tmpDirPathList[1],tmpDirPathList[1].mid(tmpDirPathList[1].lastIndexOf("/")+1)+" 文件夹版本号信息错误!"));
                emit EndCheckFileSignal(flag,tmpflag,*errList);
                return;
            }
            (*errList)=siFileOperateMethod->CheckBAFileExist(tmpDirPathList[1],IDType);
            if(errList->size()==0){tmpflag=true;}
        }else{
            errList->append(SICommonMethod::SetERRMessage(dirPath,"Before&After 路径解析失败!"));
        }
        emit EndCheckFileSignal(flag,tmpflag,*errList);
        break;
    default:
        break;
    }
}

/**
 * @def 项目源码复制任务
 * @brief SIFileOperateThread::CopyFileSlot
 * @param srcFilePath
 * @param desFilePath
 * @param flag
 */
void SIFileOperateThread::CopyFileSlot(const QString srcFilePath,const QString desFilePath,const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->CopyCodeFileSlot() 函数执行!");
    bool result=false;
    result=commonMethod->CopyFile(srcFilePath,desFilePath);
    if(result){emit EndCopyFileSignal(desFilePath,flag,result);
    }else{emit EndCopyFileSignal(srcFilePath,flag,result);}
}




