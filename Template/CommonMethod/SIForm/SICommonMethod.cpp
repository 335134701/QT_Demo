#include "SICommonMethod.h"

/**
 * @brief SICommonMethod::SICommonMethod
 * @param parent
 */
SICommonMethod::SICommonMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SICommonMethod 构造函数执行!");
}

/**
 * @def 判断机种类型
 *      根据ID字符串，第三四位字符判断
 * @brief SICommonMethod::JudgeIDType
 * @param ID
 * @return
 */
QString SICommonMethod::JudgeIDType(const QString ID)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->JudgeIDType() 函数执行!");
    QString ret;
    if(!ID.contains("EN")){return NULL;}
    //字符转数字
    switch (ID.mid(2,2).toInt()) {
    case 33 :
    case 40 :
    case 42 :
        ret="EntryAVM";
        break;
    case 34:
        ret="EntryAVM2";
        break;
    case 35:
        ret="EntryIPA";
        break;
    case 36:
        ret="FAP";
        break;
    case 37:
        ret="NextPH3";
        break;
    default:
        break;
    }
    return ret;
}


/**
 * @def 根据量产管理表信息，解析出before&After路径
 * @brief SICommonMethod::GetBeforeAfterDirPath
 * @param softList
 * @return
 */
QStringList SICommonMethod::GetBeforeAfterDirPath(const QString dirPath,const QList<SI_SOFTNUMBERTable> softList)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->GetBeforeAfterDirPath() 函数执行!");
    QStringList dirPathList;
    QString beforeVer,afterVer;
    if(QDir(dirPath).exists()&&softList.size()>0)
    {
        if(softList.size()==1){
            if(softList[0].Productionstage==ProducTionstage){
                afterVer=softList[0].ApplicationVer;
            }else{
                beforeVer=softList[0].ApplicationVer;
            }
        }else{
            beforeVer=softList[softList.size()-2].ApplicationVer;
            afterVer=softList[softList.size()-1].ApplicationVer;
        }
        dirPathList.append(dirPath+"/Before("+beforeVer.left(2)+"."+beforeVer.left(4).mid(2)+"."+beforeVer.right(2)+")");
        dirPathList.append(dirPath+"/After("+afterVer.left(2)+"."+afterVer.left(4).mid(2)+"."+afterVer.right(2)+")");
    }
    return dirPathList;

}

/**
 * @def 比较join.mot与ALL.mot文件是否生成成功
 * @brief SICommonMethod::CompressionCompareMotFile
 * @param dirPath
 * @return
 */
bool SICommonMethod::CompressionCompareMotFile(const QString dirPath)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->CompressionCompareMotFile() 函数执行!");
    QString tmpfile=dirPath;
    QString currentDay=QDateTime::currentDateTime().toString("yyyyMMdd");
    QString currentHour=QDateTime::currentDateTime().toString("h");
    QString currentMin=QDateTime::currentDateTime().toString("m");
    QFileInfo *file=new QFileInfo();
    tmpfile=dirPath+"/ALL.mot";
    file->setFile(tmpfile);
    if(!file->exists()||file->lastModified().toString("yyyyMMdd")!=currentDay){return false;}
    if(((currentHour.toInt()-file->lastModified().toString("h").toInt())*60+(currentMin.toInt()-file->lastModified().toString("m").toInt()))>30){return false;}
    tmpfile=dirPath+"/App.mot";
    file->setFile(tmpfile);
    if(!file->exists()||file->lastModified().toString("yyyyMMdd")!=currentDay){return false;}
    if(((currentHour.toInt()-file->lastModified().toString("h").toInt())*60+(currentMin.toInt()-file->lastModified().toString("m").toInt()))>30){return false;}
    return true;

}


/**
 * @def 修改Bat文件
 * @brief SICommonMethod::CompressionChangeBatFile
 * @param dirPath
 * @param txt
 * @return
 */
bool SICommonMethod::CompressionChangeBatFile(const QString dirPath,const QString txt)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->CompressionChangeBatFile() 函数执行!");
    QString tmpfile=dirPath;
    QFile *file=new QFile();
    QTextStream out;
    tmpfile=dirPath+"/"+SUMBAT;
    file->setFileName(tmpfile);
    if(file->exists()){file->remove();}
    file->open( QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(file);
    out << "SumTool\\CalcSum_Ver005.exe "+txt+" 004A0000 004E0000 004E0000 004F0000 FF";
    file->close();

    tmpfile=dirPath+"/"+JOINBAT;
    file->setFileName(tmpfile);
    if(file->exists(tmpfile)){file->remove();}
    file->open( QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(file);
    out << "JoinTool\\joinMot_Ver001.exe ALL.mot "+txt+" join.mot";
    file->close();
    return true;
}

/**
 * @def 执行cmd命令
 * @brief SICommonMethod::CompressionRunCmd
 * @param filePath
 * @return
 */
bool SICommonMethod::CompressionRunCmd(const QString filePath)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->CompressionRunCmd() 函数执行!");
    bool result=false;
    if(!QFile::exists(filePath)){return result;}
    QProcess *p=new QProcess();
    p->setWorkingDirectory(filePath.left(filePath.lastIndexOf("/")));
    p->start(filePath);
    result=p->waitForFinished();
    return result;
}

/**
 * @brief SICommonMethod::CompressionRemoveDir
 * @param dirPath
 * @return
 */
bool SICommonMethod::CompressionRemoveDir(const QString dirPath,const QString txt)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->CompressionRemoveDir() 函数执行!");
    QDir dir(dirPath);
    if(!dir.exists()){return false;}
    QStringList fileList = dir.entryList(QDir::Dirs);
    foreach (QString str, fileList) {
        if(str!=txt&&str!="Debug"&&str!="Release"&&str!="."&&str!=".."){
            dir.setPath(dirPath+"/"+str);
            if(dir.removeRecursively()){return false;}
        }
    }
    return true;
}

/**
 * @brief SICommonMethod::CompressionRemoveFile
 * @param dirPath
 * @return
 */
bool SICommonMethod::CompressionRemoveFile(const QString dirPath)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->CompressionRemoveFile() 函数执行!");
    QDir dir(dirPath);
    if(!dir.exists()){return false;}
    QStringList fileList = dir.entryList(QDir::Files);
    foreach (QString str, fileList) {
        if(str!="ALL.mot"&&str!="APP.abs"&&str!="APP.hlk"&&str!="APP.lib"&&str!="APP.map"&&str!="APP.mot"&&str!="APP.sni"&&str!=dirPath.mid(dirPath.lastIndexOf("/")+1)+".hdp"){
            if(!dir.remove(dirPath+"/"+str)){return false;}
        }
    }
    return true;
}

/**
 * @brief SICommonMethod::CompressionCopyMot
 * @param dirPath
 * @param txt
 * @return
 */
bool SICommonMethod::CompressionCopyMot(const QString dirPath, const QString txt, const QString IDType, QString APPVer, const QString InfoVer)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->CompressionCopyMot() 函数执行!");
    CommonMethod commonMethod;
    QString APPMotName;
    QString JoinMotName;
    if(!QDir(dirPath).exists()){return false;}
    if(APPVer.left(1)=="0"){APPVer="A"+APPVer.mid(1);}
    if(APPVer.left(1)=="1"){APPVer="B"+APPVer.mid(1);}
    if(IDType=="EntryAVM"){
        JoinMotName="join_Entry_"+txt+"_v"+APPVer.left(4)+"_CP"+InfoVer.left(4).mid(2)+"_"+QDateTime::currentDateTime().toString("yyyyMMdd")+".mot";
        APPMotName="APP_Entry_"+txt.mid(txt.lastIndexOf("_")+1)+"_"+txt.left(txt.lastIndexOf("_"))+"_v"+APPVer.left(4)+"_"+QDateTime::currentDateTime().toString("yyyyMMdd")+".mot";
    }else if(IDType=="EntryAVM2"){
        JoinMotName="join_Entry2_"+txt+"_v"+APPVer.left(4)+"_CP"+InfoVer.left(4).mid(2)+"_"+QDateTime::currentDateTime().toString("yyyyMMdd")+".mot";
        APPMotName="APP_Entry2_"+txt.mid(txt.lastIndexOf("_")+1)+"_"+txt.left(txt.lastIndexOf("_"))+"_v"+APPVer.left(4)+"_"+QDateTime::currentDateTime().toString("yyyyMMdd")+".mot";
    }
    else if(IDType=="EntryIPA"){
        JoinMotName="join_EntryIPA_"+txt+"_v"+APPVer.left(4)+"_CP"+InfoVer.left(4).mid(2)+"_"+QDateTime::currentDateTime().toString("yyyyMMdd")+".mot";
        APPMotName="APP_EntryIPA_"+txt.mid(txt.lastIndexOf("_")+1)+"_"+txt.left(txt.lastIndexOf("_"))+"_v"+APPVer.left(4)+"_"+QDateTime::currentDateTime().toString("yyyyMMdd")+".mot";
    }else if(IDType=="NextPH3"){
        JoinMotName="join_N_PH3_"+txt.left(txt.lastIndexOf("_"))+"_v"+APPVer.left(4)+"_CP"+InfoVer.left(4)+"_"+QDateTime::currentDateTime().toString("yyyyMMdd")+".mot";
        APPMotName="APP_N_PH3_"+txt.left(txt.lastIndexOf("_"))+"_v"+APPVer.left(4)+"_"+QDateTime::currentDateTime().toString("yyyyMMdd")+".mot";
    }
    //复制文件
    if(!commonMethod.CopyFile(dirPath+"/ALL/ALL/"+txt+"/APP.mot",dirPath.left(dirPath.lastIndexOf("/"))+"/"+APPMotName)){return false;}
    if(!commonMethod.CopyFile(dirPath+"/tools/join.mot",dirPath.left(dirPath.lastIndexOf("/"))+"/"+JoinMotName)){return false;}
    return true;
}

/**
 * @def 错误信息配置
 * @brief SICommonMethod::SetERRMessage
 * @param fileName
 * @param errMessage
 * @return
 */
SI_ERRORTable SICommonMethod::SetERRMessage(const QString fileName, const QString errMessage)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->SetERRMessage() 函数执行!");
    SI_ERRORTable err;
    err.fileName=fileName;
    err.errMessage=errMessage;
    err.col=0;
    err.row=0;
    return err;
}

/**
 * @def 错误信息配置
 * @brief SICommonMethod::SetERRMessage
 * @param fileName
 * @param sheetName
 * @param errMessage
 * @return
 */
SI_ERRORTable SICommonMethod::SetERRMessage(const QString fileName, const QString sheetName, const QString errMessage)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->SetERRMessage() 函数执行!");
    SI_ERRORTable err;
    err.fileName=fileName;
    err.sheetName=sheetName;
    err.errMessage=errMessage;
    err.col=0;
    err.row=0;
    return err;
}

/**
 * @def 错误信息配置
 * @brief SICommonMethod::SetERRMessage
 * @param fileName
 * @param sheetName
 * @param row
 * @param col
 * @param errMessage
 * @return
 */
SI_ERRORTable SICommonMethod::SetERRMessage(const QString fileName, const QString sheetName, const unsigned int row, const unsigned int col, const QString errMessage)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->SetERRMessage() 函数执行!");
    SI_ERRORTable err;
    err.fileName=fileName;
    err.sheetName=sheetName;
    err.row=row;
    err.col=col;
    err.errMessage=errMessage;
    return err;
}
