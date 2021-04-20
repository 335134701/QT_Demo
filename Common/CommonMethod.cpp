#include "CommonMethod.h"

CommonMethod::CommonMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("CommonMethod() 构造函数执行!");
}



/**
 * @def 判断机种类型
 *      根据ID字符串，第三四位字符判断
 * @brief UIMethod::JudgeIDType
 * @param ID
 * @return 返回机种类型对象
 */
QString CommonMethod::JudgeIDType(const QString ID)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDType() 函数执行!");
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
        ret="";
        break;
    }
    return ret;
}
/**
 * @def 对传入路径进行解析，并组装成指定的路径
 * @brief CommonMethod::AnalyzePath
 * @param dirPath
 * @return
 */
QString CommonMethod::AnalyzePath(const QString dirPath)
{
    QString pathName;
    if(!QFile::exists(dirPath)){return pathName;}
    pathName=dirPath.mid(dirPath.lastIndexOf("/")+1);
    return pathName;
}
/**
 * @def   处理关于错误码的赋值，删除，以及不确定赋值或者删除操作
 * @brief CommonMethod::ErrorCodeDeal
 * @param errCode
 * @param errCodeType
 * @param objectName
 * @param condition
 * @param flag
 */
void CommonMethod::ErrorCodeDeal(QMap<QString, ERRCODETYPE> *errCode, QMap<QString,ERRCODETYPE> errCodeType,const QString objectName, const QString condition, bool flag)
{
    QLogHelper::instance()->LogInfo("CommonMethod->ErrorCodeDeal() 函数执行!");
    if(errCode==NULL){return;}
    if((flag&&errCode->value(objectName).ID.isEmpty())&&condition.isEmpty()){
        QLogHelper::instance()->LogDebug("CommonMethod->ErrorCodeDeal() 添加错误码!");
        //如果错误码不存在，则添加错误码
        errCode->insert(objectName,errCodeType.value(objectName));
    }else if(!errCode->value(objectName).ID.isEmpty()||!condition.isEmpty()){
        QLogHelper::instance()->LogDebug("CommonMethod->ErrorCodeDeal() 移除错误码!");
        //如果存在错误码则移除错误码
        errCode->remove(objectName);
    }
}
/**
 * @def 根据条件找到对应的文件
 *      dir 文件所在的路径
 *      filters 过滤器
 * @brief CommonMethod::FindFile
 * @param dirPath
 * @param filters
 * @return
 */
QStringList CommonMethod::FindFile(const QString dirPath,QStringList filters)
{
    QStringList fileNames;
    if(!QFile::exists(dirPath)){return fileNames;}
    QDir dir(dirPath);
    foreach (QString file, dir.entryList(filters,QDir::Files)) {
        fileNames+=dirPath+"/"+file;
    }
    //if(fileNames.size()>100){return fileNames;}
    foreach (QString subdir, dir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot)) {
        fileNames+=FindFile(dirPath+"/"+subdir,filters);
    }
    return fileNames;
}
/**
 * @def 此函数主要功能是获取到依赖文件列表后取出最新的依赖文件
 * @brief CommonMethod::AnalyzeRelyFilePath
 * @param filePath
 * @return
 */
QString CommonMethod::AnalyzeRelyFilePath(const QStringList filePath)
{
    QLogHelper::instance()->LogInfo("CommonMethod->AnalyzeRelyFilePath() 函数执行!");
    int size=0,tmpsize=0;
    QString relyFilePath;
    if(filePath.size()>0){
        foreach (QString file, filePath) {
            tmpsize=file.mid(file.lastIndexOf("_")+1,6).toInt();
            if(tmpsize>size){
                size=tmpsize;
                relyFilePath=file;
            }
        }
    }
    return relyFilePath;
}

