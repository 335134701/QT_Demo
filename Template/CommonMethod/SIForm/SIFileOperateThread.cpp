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
        result=siFileOperateMethod->UNZipCodeDearDir(tmp);
    }
    emit EndRunOrderSignal(result,flag);
}

/**
 * @brief SIFileOperateThread::ZipCodeFileSlot
 * @param exeFilePath
 * @param dirPath
 * @param txt
 * @param APPVer
 * @param flag
 */
void SIFileOperateThread::ZipCodeFileSlot(const QString exeFilePath, const QString dirPath, const QString IDType, const QString txt,QString APPVer, const unsigned flag)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->ZipCodeFileSlot() 函数执行!");
    QFile *file;
    QProcess *p=new QProcess();
    QStringList args;
    QString Folder_SH7766_Name;
    QString Folder_SVN_Name;
    bool result=false;
    if(APPVer.left(1)=="0"){APPVer="A"+APPVer.mid(1);}
    if(APPVer.left(1)=="1"){APPVer="B"+APPVer.mid(1);}
    if(IDType!="NextPH3"){
        Folder_SH7766_Name=IDType+"_"+txt+"_v"+APPVer.left(4)+"_"+QDateTime::currentDateTime().toString("yyyyMMdd")+"_ROM_MASS_CAN.7z";
        Folder_SVN_Name="Ver"+APPVer.left(2)+"."+APPVer.left(4).mid(2)+"_"+txt+".7z";
    }else{
        Folder_SH7766_Name="N_PH3_"+txt+"_v"+APPVer.left(4)+"_"+QDateTime::currentDateTime().toString("yyyyMMdd")+"_ROM_MASS_CAN.7z";
        Folder_SVN_Name="N_PH3_Ver"+APPVer.left(2)+"."+APPVer.left(4).mid(2)+".7z";
    }
    if(file->exists(exeFilePath)&&QDir(dirPath).exists()){
        args.append("a");
        args.append(dirPath.left(dirPath.lastIndexOf("/"))+"/"+Folder_SH7766_Name);
        args.append(dirPath);
        p->start(exeFilePath, args);
        result=p->waitForFinished(-1);
        if(result){
            args.clear();
            args.append("a");
            args.append(dirPath.left(dirPath.lastIndexOf("/"))+"/"+Folder_SVN_Name);
            args.append(dirPath);
            p->start(exeFilePath, args);
            result=p->waitForFinished(-1);
        }
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
    QLogHelper::instance()->LogDebug("-------------------搜索标记: "+QString::number(flag)+"    搜索路径:"+tmpDirPath);
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
 * @param softList
 */
void SIFileOperateThread::CheckBAFileSlot(const QString dirPath,const QString ID,const QString IDType,const QList<SI_SOFTNUMBERTable> softList,const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->CheckFileSignalSlot() 函数执行!");
    bool tmpflag=false;
    QStringList tmpDirPathList;
    //错误消息集合
    QList<SI_ERRORTable> *errList=new QList<SI_ERRORTable>();
    //文件路径根据flag进行细化
    tmpDirPathList=siCommonMethod->GetBeforeAfterDirPath(siFileOperateMethod->AnalyzePath(dirPath,ID,IDType,flag),softList);
    if(tmpDirPathList.size()>=2){
        if(!QDir(tmpDirPathList[0]).exists()){
            errList->append(SICommonMethod::SetERRMessage(tmpDirPathList[0],tmpDirPathList[0].mid(tmpDirPathList[0].lastIndexOf("/")+1)+" 文件夹版本号信息错误!"));
            emit EndCheckBAFileSignal(tmpflag,*errList);
            return;
        }
        if(!QDir(tmpDirPathList[1]).exists()){
            errList->append(SICommonMethod::SetERRMessage(tmpDirPathList[1],tmpDirPathList[1].mid(tmpDirPathList[1].lastIndexOf("/")+1)+" 文件夹版本号信息错误!"));
            emit EndCheckBAFileSignal(tmpflag,*errList);
            return;
        }
        (*errList)=siFileOperateMethod->CheckBAFileExist(tmpDirPathList[1],IDType);
        if(errList->size()==0){tmpflag=true;}
    }else{
        errList->append(SICommonMethod::SetERRMessage(dirPath,"Before&After 路径解析失败!"));
    }
    emit EndCheckBAFileSignal(tmpflag,*errList);
}

/**
 * @brief SIFileOperateThread::CheckCLFileSlot
 * @param dirPath
 * @param ID
 * @param softList
 */
void SIFileOperateThread::CheckCLFileSlot(const QString dirPath, const QString ID, const QList<SI_SOFTNUMBERTable> softList)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->CopyCodeFileSlot() 函数执行!");
    QDir folder(dirPath);
    QString carMessage=softList.value(softList.size()-1).CarModels+"_"+ID;
    //错误消息集合
    QList<SI_ERRORTable> *errList=new QList<SI_ERRORTable>();
    if(!folder.exists()){
        errList->append(SICommonMethod::SetERRMessage(dirPath,"文件夹不存在,无法进行校验!"));
        emit EndCheckCLFileSignal(false,*errList);
        return;
    }
    if(QFile(dirPath+"/common/car_param/"+carMessage+"/CL_Ver_Parts_ex.h").exists()){

    }else {

    }
    emit EndCheckCLFileSignal(true,*errList);
}

/**
 * @def 项目源码复制任务
 * @brief SIFileOperateThread::CopyCodeFileSlot
 * @param srcFilePath
 * @param desFilePath
 */
void SIFileOperateThread::CopyCodeFileSlot(const QString srcFilePath,const QString desFilePath)
{
    QLogHelper::instance()->LogInfo("SIFileOperateThread->CopyCodeFileSlot() 函数执行!");
    bool result=false;
    result=commonMethod->CopyFile(srcFilePath,desFilePath);
    if(result){emit EndCopyCodeFileSignal(desFilePath,result);
    }else{emit EndCopyCodeFileSignal(srcFilePath,result);}
}




