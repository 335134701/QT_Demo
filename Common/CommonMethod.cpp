#include "CommonMethod.h"

CommonMethod::CommonMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("CommonMethod() 构造函数执行!");
}
/**
 * @def 根据条件找到对应的文件
 *      dir 文件所在的路径
 *      conditionOne 条件1
 * @brief CommonMethod::OutputFilePath
 * @param dir
 * @param conditionOne
 * @param conditionTwo
 * @param errCode
 * @return
 */
QStringList CommonMethod::FinFile(const QString dirPath,QStringList filters)
{
    QStringList fileNames;
    QDir dir(dirPath);
    foreach (QString file, dir.entryList(filters,QDir::Files)) {
        fileNames+=dirPath+"/"+file;
    }
    foreach (QString subdir, dir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot)) {
        fileNames+=FinFile(dirPath+"/"+subdir,filters);
    }
    return fileNames;
}
/**
 * @def 根据条件找到对应的文件
 *      dir 文件所在的路径
 *      conditionOne 条件1
 *      conditionTwo 条件2
 * @brief CommonMethod::OutputFilePath
 * @param dir
 * @param conditionOne
 * @param conditionTwo
 * @param errCode
 * @return
 */
/*
QString CommonMethod::OutputFilePath(const QString dir,const QString conditionOne,const QString conditionTwo)
{
    QLogHelper::instance()->LogInfo("CommonMethod->OutputFilePath() 函数执行!");
    QFileInfo file;
    if(dir.isEmpty()||JudgeDirExist(dir)){return NULL;}
    QDir searchDir(dir);
    //列出searchDir(path)目录文件下所有文件和目录信息，存储到file_list容器
    QFileInfoList file_list = searchDir.entryInfoList();
    //进行子文件夹folder_list递归遍历，将内容存入file_list容器
    foreach(QFileInfo fileinfo, file_list)
    {
        if(!conditionTwo.isEmpty()){
            if(fileinfo.fileName().contains(conditionOne)&&fileinfo.fileName().contains(conditionTwo)){
                QLogHelper::instance()->LogInfo("CommonMethod->OutputFilePath() "+fileinfo.fileName()+" 文件存在!" );
                file=fileinfo;
            }
        }else{
            if(fileinfo.fileName().contains(conditionOne)){
                QLogHelper::instance()->LogInfo("UIMethod->SelectExampleSlot() "+fileinfo.fileName()+" 文件存在!" );
                file=fileinfo;
            }
        }
    }
    return file.absoluteFilePath();
}
*/


/**
 * @def 判断目录是否存在
 * @brief CommonMethod::JudgeDirExist
 * @param dir
 * @return
 */
bool CommonMethod::JudgeDirExist(const QString dirPath)
{
    //判断目录是否存在
    if(QFile::exists(dirPath)){return true;
    }else{return false;
    }
}

