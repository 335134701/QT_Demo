#include "CommonMethod.h"

CommonMethod::CommonMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("CommonMethod() 构造函数执行!");
}
/**
 * @def 根据条件找到对应的文件
 *      dir 文件所在的路径
 *      conditionOne 条件1
 *      conditionTwo 条件2
 *      errCode 错误码
 * @brief CommonMethod::OutputFilePath
 * @param dir
 * @param conditionOne
 * @param conditionTwo
 * @param errCode
 * @return
 */
QString CommonMethod::OutputFilePath(const QString dir,const QString conditionOne,const QString conditionTwo)
{
    QLogHelper::instance()->LogInfo("CommonMethod->OutputFilePath() 函数执行!");
    QFileInfo file;
    if(dir.isEmpty()){return NULL;}
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

