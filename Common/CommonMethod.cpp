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
    if(!QDir(dirPath).exists()){return fileNames;}
    QDir dir(dirPath);
    foreach (QString file, dir.entryList(filters,QDir::Files)) {
        fileNames+=dirPath+"/"+file;
    }
    foreach (QString subdir, dir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot)) {
        fileNames+=FindFile(dirPath+"/"+subdir,filters);
    }
    return fileNames;
}
/**
 * @def 对传入路径进行解析，并组装成指定的路径
 *      flag 说明:
 *      0   无任何表示
 *      1   表示机种番号信息获取
 *      2   表示机种类型信息获取
 *      3   表示量产管理表信息获取
 *      4   表示Ini文件信息获取
 *      5   表示P票信息获取
 *      6   表示SW确认文件获取
 *      7   表示CarInfoMot文件获取
 *      8   表示CarMapMot文件获取
 *      9   表示OSDMot文件获取
 *      10   表示 joinMot 文件获取
 *      11   表示 appMot   文件获取
 *      12   表示 DR会議運用手順_様式7模板文件 获取
 *      13   表示 確認シート 文件获取
 *      14   表示 EntryAVM採用車種 文件获取
 * @brief CommonMethod::AnalyzePath
 * @param dirPath
 * @return
 */
QString CommonMethod::AnalyzePath(const QString dirPath,const QString ID,QString IDType,unsigned int flag)
{
    QLogHelper::instance()->LogInfo("CommonMethod->AnalyzePath() 函数执行!");
    QString pathName=dirPath;
    if(!QDir(dirPath).exists()){return pathName;}
    if(pathName.mid(pathName.lastIndexOf("/")+1)=="CK_RedMaple"&&QDir(pathName+"/trunk").exists()){pathName=pathName+"/trunk";}
    if(pathName.mid(pathName.lastIndexOf("/")+1)=="trunk"&&QDir(pathName+"/01REQ").exists()){pathName=pathName+"/01REQ";}
    if(QDir(pathName).exists()){
        switch (flag) {
        case 3:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0101Model").exists()){pathName=pathName+"/0101Model";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0101Model"&&QDir(pathName+"/按类型归档").exists()){pathName=pathName+"/按类型归档";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="按类型归档"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/00量产管理表").exists()){pathName=pathName+"/00量产管理表";}
            break;
        case 4:
        case 12:
        case 13:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0101Model").exists()){pathName=pathName+"/0101Model";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0101Model"&&QDir(pathName+"/按类型归档").exists()){pathName=pathName+"/按类型归档";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="按类型归档"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/入检资料作成情報").exists()){pathName=pathName+"/入检资料作成情報";}
            break;
        case 5:
        case 6:
        case 10:
        case 11:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0102Report").exists()){pathName=pathName+"/0102Report";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0102Report"&&QDir(pathName+"/00成果物").exists()){pathName=pathName+"/00成果物";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="00成果物"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(IDType=="EntryAVM"){IDType="Entry";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/"+IDType).exists()){pathName=pathName+"/"+IDType;}
            if(pathName.mid(pathName.lastIndexOf("/")+1)==IDType&&QDir(pathName+"/"+ID).exists()){pathName=pathName+"/"+ID;}
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 14:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0101Model").exists()){pathName=pathName+"/0101Model";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0101Model"&&QDir(pathName+"/按类型归档").exists()){pathName=pathName+"/按类型归档";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="按类型归档"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/01コンフィグ詳細").exists()){pathName=pathName+"/01コンフィグ詳細";}
            break;
        }
    }
    return pathName;
}
/**
 * @def 根据获取的文件列表，解析需要的文件路径
 *      flag 说明:
 *      0   无任何表示
 *      1   表示机种番号信息获取
 *      2   表示机种类型信息获取
 *      3   表示量产管理表信息获取
 *      4   表示Ini文件信息获取
 *      5   表示P票信息获取
 *      6   表示SW确认文件获取
 *      7   表示CarInfoMot文件获取
 *      8   表示CarMapMot文件获取
 *      9   表示OSDMot文件获取
 *      10   表示 joinMot 文件获取
 *      11   表示 appMot   文件获取
 *      12   表示 DR会議運用手順_様式7模板文件 获取
 *      13   表示 確認シート 文件获取
 *      14   表示 EntryAVM採用車種 文件获取
 * @brief CommonMethod::AnalyzeFilePath
 * @param filePaths
 * @param filePath
 * @param flag
 * @return
 */
void CommonMethod::AnalyzeFilePath(const QStringList filePaths, QString *filePath, unsigned int flag)
{
    QLogHelper::instance()->LogInfo("CommonMethod->AnalyzeFilePath() 函数执行!");
    QString path;
    int size=0,tmpsize=0;
    switch (flag) {
    case 3:
        foreach (QString file, filePaths) {
            tmpsize=file.mid(file.lastIndexOf("_")+1,6).toInt();
            if(tmpsize>size){
                size=tmpsize;
                path=file;
            }
        }
        break;
    case 4:
    case 5:
    case 6:
        if(filePaths.size()>0){path=filePaths[0];}
        break;
    case 7:
        foreach (QString file, filePaths) {
            QLogHelper::instance()->LogInfo(file);
        }
        break;
    case 8:
        foreach (QString file, filePaths) {
            QLogHelper::instance()->LogInfo(file);
        }
        break;
    case 9:
        foreach (QString file, filePaths) {
            QLogHelper::instance()->LogInfo(file);
        }
        break;
    case 10:
        if(filePaths.size()>0){path=filePaths[0];}
        break;
    case 11:
        if(filePaths.size()>0){path=filePaths[0];}
        break;
    case 12:
        foreach (QString file, filePaths) {
            QLogHelper::instance()->LogInfo(file);
        }
        break;
    case 13:
        foreach (QString file, filePaths) {
            QLogHelper::instance()->LogInfo(file);
        }
        break;
    case 14:
        foreach (QString file, filePaths) {
            QLogHelper::instance()->LogInfo(file);
        }
        break;
    }
    (*filePath)=path;
}


