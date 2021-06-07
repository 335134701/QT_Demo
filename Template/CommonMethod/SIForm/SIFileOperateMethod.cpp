#include "SIFileOperateMethod.h"

/**
 * @brief SIFileOperateMethod::SIFileOperateMethod
 * @param parent
 */
SIFileOperateMethod::SIFileOperateMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIFileOperateMethod 构造函数执行!");
    this->Init();
}

/**
 * @brief SIFileOperateMethod::getSiCommonMethod
 * @return
 */
SICommonMethod *SIFileOperateMethod::getSiCommonMethod() const
{
    return siCommonMethod;
}

/**
 * @brief SIFileOperateMethod::setSiCommonMethod
 * @param value
 */
void SIFileOperateMethod::setSiCommonMethod(SICommonMethod *value)
{
    siCommonMethod = value;
}


/**
 * @brief SIFileOperateMethod::Init
 */
void SIFileOperateMethod::Init()
{
    QLogHelper::instance()->LogInfo("SIFileOperateMethod->Init() 函数执行!");
}

/**
 * @brief SIFileOperateMethod::getCommonMethod
 * @return
 */
CommonMethod *SIFileOperateMethod::getCommonMethod() const
{
    return commonMethod;
}

/**
 * @brief SIFileOperateMethod::setCommonMethod
 * @param value
 */
void SIFileOperateMethod::setCommonMethod(CommonMethod *value)
{
    commonMethod = value;
}

/**
 * @def 对路径进行处理
 * @brief SIFileOperateMethod::AnalyzePath
 * @param dirPath
 * @param ID
 * @param IDType
 * @param flag
 * @return
 */
QString SIFileOperateMethod::AnalyzePath(const QString dirPath,const QString ID,const QString IDType, unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIFileOperateMethod->AnalyzePath() 函数执行!");
    QString tmpIDType=IDType;
    QString pathName=dirPath;
    if(!QDir(dirPath).exists()){return pathName;}
    if(pathName.mid(pathName.lastIndexOf("/")+1)=="CK_RedMaple"&&QDir(pathName+"/trunk").exists()){pathName=pathName+"/trunk";}
    if(pathName.mid(pathName.lastIndexOf("/")+1)=="trunk"&&QDir(pathName+"/01REQ").exists()){pathName=pathName+"/01REQ";}
    if(QDir(pathName).exists()){
        switch (flag) {
        case SIRelyFileflag:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0101Model").exists()){pathName=pathName+"/0101Model";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0101Model"&&QDir(pathName+"/按类型归档").exists()){pathName=pathName+"/按类型归档";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="按类型归档"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/00量产管理表").exists()){pathName=pathName+"/00量产管理表";}
            break;
        case SIPFileflag:
        case SIBuildFileflag:
        case SISWFileflag:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0102Report").exists()){pathName=pathName+"/0102Report";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0102Report"&&QDir(pathName+"/00成果物").exists()){pathName=pathName+"/00成果物";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="00成果物"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(tmpIDType=="EntryAVM"){tmpIDType="Entry";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/"+tmpIDType).exists()){pathName=pathName+"/"+tmpIDType;}
        case SICodeFileflag:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0101Model").exists()){pathName=pathName+"/0101Model";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0101Model"&&QDir(pathName+"/按类型归档").exists()){pathName=pathName+"/按类型归档";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="按类型归档"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            break;
        case SICarInfoFileflag:
            break;
        case SISHDefineFileflag:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0102Report").exists()){pathName=pathName+"/0102Report";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0102Report"&&QDir(pathName+"/00成果物").exists()){pathName=pathName+"/00成果物";}
            break;
        case SIBADirflag:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0102Report").exists()){pathName=pathName+"/0102Report";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0102Report"&&QDir(pathName+"/00成果物").exists()){pathName=pathName+"/00成果物";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="00成果物"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(tmpIDType=="EntryAVM"){tmpIDType="Entry";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/"+tmpIDType).exists()){pathName=pathName+"/"+tmpIDType;}
            if(pathName.mid(pathName.lastIndexOf("/")+1)==tmpIDType&&QDir(pathName+"/"+ID).exists()){pathName=pathName+"/"+ID;}
            if(pathName.mid(pathName.lastIndexOf("/")+1)==ID&&QDir(pathName+"/"+ID).exists()){pathName=pathName+"/"+ID;}
            if(pathName.mid(pathName.lastIndexOf("/")+1)==ID&&QDir(pathName+"/SourceDiff").exists()){pathName=pathName+"/SourceDiff";}
        }
    }
    return pathName;
}

/**
 * @def 文件查找处理
 * @brief SIFileOperateMethod::SearchFile
 * @param dirPath
 * @param filters
 * @return
 */
QStringList SIFileOperateMethod::SearchFile(const QString dirPath, const QStringList filters)
{
    //QLogHelper::instance()->LogInfo("SIFileOperateMethod->SearchFile() 函数执行!");
    QStringList filePaths;
    if(!QDir(dirPath).exists()){return filePaths;}
    QDir dir(dirPath);
    foreach (QString file, dir.entryList(filters,QDir::Files)) {
        filePaths+=dirPath+"/"+file;
    }
    foreach (QString subdir, dir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot)) {
        filePaths+=SearchFile(dirPath+"/"+subdir,filters);
    }
    return filePaths;
}

/**
 * @def 对搜索文件结果进行处理
 * @brief SIFileOperateMethod::AnalyzeFilePath
 * @param filePaths
 * @param flag
 * @param ID
 * @return
 */
QString SIFileOperateMethod::AnalyzeFilePath(const QStringList filePaths, const unsigned int flag, const QString ID, const QString IDType)
{
    QLogHelper::instance()->LogInfo("SIFileOperateMethod->AnalyzeFilePath() 函数执行!");
    QString tmpIDType=IDType;
    QString path;
    int tmpsize=0;
    if(filePaths.size()==0){path="";}else{
        switch (flag) {
        case SIRelyFileflag:
        case SIPFileflag:
        case SISWFileflag:
        case SIBuildFileflag:
        case SICarInfoFileflag:
            foreach (QString filePath, filePaths) {
                QFileInfo file(filePath);
                if(tmpsize==0||file.lastModified().toString("yyMMddhhmm").toInt()>tmpsize){
                    path=filePath;
                    tmpsize=file.lastModified().toString("yyMMddhhmm").toInt();
                }
            }
            break;
        case SICodeFileflag:
            foreach (QString filePath, filePaths) {
                if(filePath.contains(ID)){
                    QFileInfo file(filePath);
                    if(tmpsize==0||file.lastModified().toString("yyMMddhhmm").toInt()>tmpsize){
                        path=filePath;
                        tmpsize=file.lastModified().toString("yyMMddhhmm").toInt();
                    }
                }
            }
            break;
        case SISHDefineFileflag:
            if(tmpIDType.contains("Entry")){tmpIDType="";}
            if(tmpIDType.contains("NextPH3")){tmpIDType="N_PH3";}
            foreach (QString filePath, filePaths) {
                if((!tmpIDType.isEmpty()&&filePath.contains(tmpIDType))||(tmpIDType.isEmpty()&&!filePath.contains("(")&&!filePath.contains(")"))){
                    QFileInfo file(filePath);
                    if(tmpsize==0||file.lastModified().toString("yyMMddhhmm").toInt()>tmpsize){
                        path=filePath;
                        tmpsize=file.lastModified().toString("yyMMddhhmm").toInt();
                    }
                }
            }
            break;
        }
    }
    return path;
}

/**
 * @def 对解压后的文件进行二次处理，保证源码目录结构一致，便于后续对项目源码打包
 * @brief SIFileOperateMethod::UNZipCodeDearDir
 * @param dirPath
 * @return
 */
bool SIFileOperateMethod::UNZipCodeDearDir(const QString dirPath)
{
    QLogHelper::instance()->LogInfo("SIFileOperateMethod->UNZipCodeDearDir() 函数执行!");
    QString tmpDirPath=dirPath;
    bool flag=false;
    QDir folder(tmpDirPath);
    if(!folder.exists()){return flag;}
    if(folder.exists(tmpDirPath+"/ALL")&&folder.exists(tmpDirPath+"/BOOT")&&folder.exists(tmpDirPath+"/tools")){
        flag=true;
    }else{
        folder.setFilter(QDir::Dirs | QDir::Files);
        folder.setSorting(QDir::DirsFirst);//文件夹排在前面
        QFileInfoList list = folder.entryInfoList();
        foreach (QFileInfo file, list) {
            tmpDirPath=tmpDirPath+"/"+file.baseName();
            if(folder.exists(tmpDirPath+"/ALL")&&folder.exists(tmpDirPath+"/BOOT")&&folder.exists(tmpDirPath+"/tools")){
                flag=commonMethod->CopyDir(tmpDirPath,tmpDirPath.left(tmpDirPath.lastIndexOf("/")));
                folder.setPath(tmpDirPath);
                flag=folder.removeRecursively();
                break;
            }
        }
    }
    return flag;
}

/**
 * @def 校验Before After文件夹下文件是否存在
 * @brief SIFileOperateMethod::CheckBAFileExist
 * @param dirPath
 * @param IDType
 * @return
 */
QList<SI_ERRORTable> SIFileOperateMethod::CheckBAFileExist(const QString dirPath, const QString IDType)
{
    QLogHelper::instance()->LogInfo("SIFileOperateMethod->CheckBAFileExist() 函数执行!");
    QStringList fileList;
    fileList.append("/Application/view/src/vi_app_i2c.c");
    fileList.append("/Application/view/src/vi_main.c");
    //fileList.append("/common/include/CL_Ver_ex.h");
    //fileList.append("/common/include/gpiow_wrapper_ex.h");
    fileList.append("/common/include/i2cw_wrapper_ex.h");
    fileList.append("/common/include/vi_common_ex.h");
    fileList.append("/LMiddle/i2c_wrapper/src/i2cw_wrapper.c");
    QString tmpDirPath=dirPath,Ver=dirPath.mid(dirPath.lastIndexOf("/")+1);
    //错误消息集合
    QList<SI_ERRORTable> *errList=new QList<SI_ERRORTable>();
    QFile *file=new QFile();
    foreach (QString filestr, fileList) {
        if(!file->exists(tmpDirPath+filestr)){
            errList->append(SICommonMethod::SetERRMessage(tmpDirPath+filestr,"文件不存在!"));
        }
    }
    tmpDirPath=dirPath+"/common/include/gpiow_wrapper_ex.h";
    if((IDType=="EntryAVM1"||IDType=="EntryAVM2")&&file->exists(tmpDirPath)){
        errList->append(SICommonMethod::SetERRMessage(tmpDirPath,"文件不存在!"));
    }
    tmpDirPath=dirPath+"/common/include/CL_Ver_ex.h";
    if(file->exists(tmpDirPath)){
        Ver=Ver.mid(Ver.indexOf("(")+1);
        Ver=Ver.left(Ver.indexOf(")"));
        file->setFileName(tmpDirPath);
        if(!file->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            errList->append(SICommonMethod::SetERRMessage(tmpDirPath,"文件无法读取,无法判断版本号信息!"));
        }
        while(!file->atEnd())
        {
            QByteArray line = file->readLine();
            QString str(line),tmp;
            if(str.contains("#define		E_SOFTVER")){
                tmp=str.mid(str.indexOf("0x")+2);
                tmp=tmp.left(tmp.indexOf("U"));
                if(tmp!=(Ver.left(2)+Ver.left(5).mid(3))){
                    errList->append(SICommonMethod::SetERRMessage(tmpDirPath,"E_SOFTVER 版本信息错误!"));
                }
            }
            if(str.contains("#define		E_MINORVER")){
                tmp=str.mid(str.indexOf("0x")+2);
                tmp=tmp.left(tmp.indexOf("U"));
                if(tmp!=Ver.right(2)){
                    errList->append(SICommonMethod::SetERRMessage(tmpDirPath,"E_MINORVER 版本信息错误!"));
                }
                break;
            }
        }
    }else{
        errList->append(SICommonMethod::SetERRMessage(tmpDirPath,"文件不存在!"));
    }
    return *errList;
}




