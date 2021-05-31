#include "SIFileOperateMethod.h"

/**
 * @brief SIFileOperateMethod::SIFileOperateMethod
 * @param parent
 */
SIFileOperateMethod::SIFileOperateMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIFileOperateMethod 构造函数执行!");
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
QString SIFileOperateMethod::AnalyzePath(const QString dirPath, const QString ID, QString IDType, unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIFileOperateMethod->AnalyzePath() 函数执行!");
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
QStringList SIFileOperateMethod::SearchFile(const QString dirPath, QStringList filters)
{
    QLogHelper::instance()->LogInfo("SIFileOperateMethod->SearchFile() 函数执行!");
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
 * @return
 */
QString SIFileOperateMethod::AnalyzeFilePath(const QStringList filePaths, const unsigned int flag, const QString ID)
{
    QLogHelper::instance()->LogInfo("SIFileOperateMethod->AnalyzeFilePath() 函数执行!");
    QString path;
    unsigned int tmpsize=0;
    switch (flag) {
    case SIRelyFileflag:
        if(filePaths.size()==0){path="";}
        else{
            foreach (QString filePath, filePaths) {
                QFileInfo file(filePath);
                if(tmpsize==0||file.lastModified().toString("yyMMddhhmm").toInt()>tmpsize){
                    path=filePath;
                    tmpsize=file.lastModified().toString("yyMMddhhmm").toInt();
                }
            }
        }
        break;
    }
    return path;
}
