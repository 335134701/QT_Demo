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
        //如果错误码不存在，则添加错误码
        errCode->insert(objectName,errCodeType.value(objectName));
    }else if(!errCode->value(objectName).ID.isEmpty()||!condition.isEmpty()){
        //如果存在错误码则移除错误码
        errCode->remove(objectName);
    }
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
QStringList CommonMethod::FindFile(const QString dirPath,QStringList filters)
{
    QStringList fileNames;
    if(!QFile::exists(dirPath)){return fileNames;}
    QDir dir(dirPath);
    foreach (QString file, dir.entryList(filters,QDir::Files)) {
        QLogHelper::instance()->LogDebug(file);
        fileNames+=dirPath+"/"+file;
    }
    foreach (QString subdir, dir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot)) {
        fileNames+=FindFile(dirPath+"/"+subdir,filters);
    }
    return fileNames;
}


