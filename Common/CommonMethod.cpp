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
    QLogHelper::instance()->LogInfo("CommonMethod->JudgeIDType() 函数执行!");
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
 * @brief CommonMethod::AnalyzePath
 * @param dirPath
 * @param ID
 * @param IDType
 * @param RelyID
 * @param flag
 * @return
 */
QString CommonMethod::AnalyzePath(const QString dirPath,const QString ID,QString IDType,const QString RelyID,unsigned int flag)
{
    QLogHelper::instance()->LogInfo("CommonMethod->AnalyzePath() 函数执行!");
    QString pathName=dirPath;
    if(!QDir(dirPath).exists()){return pathName;}
    if(pathName.mid(pathName.lastIndexOf("/")+1)=="CK_RedMaple"&&QDir(pathName+"/trunk").exists()){pathName=pathName+"/trunk";}
    if(pathName.mid(pathName.lastIndexOf("/")+1)=="trunk"&&QDir(pathName+"/01REQ").exists()){pathName=pathName+"/01REQ";}
    if(QDir(pathName).exists()){
        switch (flag) {
        case RelyFileflag:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0101Model").exists()){pathName=pathName+"/0101Model";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0101Model"&&QDir(pathName+"/按类型归档").exists()){pathName=pathName+"/按类型归档";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="按类型归档"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/00量产管理表").exists()){pathName=pathName+"/00量产管理表";}
            break;
        case IniFileflag:
        case EEFileflag:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0101Model").exists()){pathName=pathName+"/0101Model";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0101Model"&&QDir(pathName+"/按类型归档").exists()){pathName=pathName+"/按类型归档";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="按类型归档"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/入检资料作成情報").exists()){pathName=pathName+"/入检资料作成情報";}
            break;
        case PFileflag:
        case SWFileflag:
        case JoinFileflag:
        case APPFileflag:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0102Report").exists()){pathName=pathName+"/0102Report";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0102Report"&&QDir(pathName+"/00成果物").exists()){pathName=pathName+"/00成果物";}
            /*
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="00成果物"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(IDType=="EntryAVM"){IDType="Entry";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/"+IDType).exists()){pathName=pathName+"/"+IDType;}
            if(pathName.mid(pathName.lastIndexOf("/")+1)==IDType&&QDir(pathName+"/"+ID).exists()){pathName=pathName+"/"+ID;}
            */
            break;
        case CarInfoFileflag:
        case CarMapFileflag:
        case CarOSDFileflag:
            break;
        case ReadyFileflag:
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="01REQ"&&QDir(pathName+"/0101Model").exists()){pathName=pathName+"/0101Model";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="0101Model"&&QDir(pathName+"/按类型归档").exists()){pathName=pathName+"/按类型归档";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="按类型归档"&&QDir(pathName+"/旭化成工場 火事対応").exists()){pathName=pathName+"/旭化成工場 火事対応";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="旭化成工場 火事対応"&&QDir(pathName+"/入检资料作成情報").exists()){pathName=pathName+"/入检资料作成情報";}
            if(pathName.mid(pathName.lastIndexOf("/")+1)=="入检资料作成情報"&&QDir(pathName+"/入检资料讲解").exists()){pathName=pathName+"/入检资料讲解";}
            if(!RelyID.isEmpty()){
                if(pathName.mid(pathName.lastIndexOf("/")+1)=="入检资料讲解"&&QDir(pathName+"/03单纯流用").exists()){pathName=pathName+"/03单纯流用";}
            }else{
                if(pathName.mid(pathName.lastIndexOf("/")+1)=="入检资料讲解"&&QDir(pathName+"/01模板").exists()){pathName=pathName+"/01模板";}
            }
            break;
        case ConfigFileflag:
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
 * @brief CommonMethod::AnalyzeFilePath
 * @param filePaths
 * @param filePath
 * @param flag
 * @param ID
 */
void CommonMethod::AnalyzeFilePath(const QStringList filePaths, QString *filePath, unsigned int flag,const QString ID)
{
    QLogHelper::instance()->LogInfo("CommonMethod->AnalyzeFilePath() 函数执行!");
    QString path;
    int size=0,tmpsize=0;
    switch (flag) {
    case RelyFileflag:
        if(filePath->size()==0){path="";}
        foreach (QString file, filePaths) {
            tmpsize=file.mid(file.lastIndexOf("_")+1,6).toInt();
            if(tmpsize>size){
                size=tmpsize;
                path=file;
            }
        }
        break;
    case IniFileflag:
    case PFileflag:
    case SWFileflag:
    case CarInfoFileflag:
    case CarMapFileflag:
    case CarOSDFileflag:
        /*
        foreach (QString filePath, filePaths) {
               QLogHelper::instance()->LogDebug(filePath);
        }
        */
        foreach (QString filePath, filePaths) {
            if(!filePath.contains(".txt")&&filePath.contains(ID)){
                QFileInfo file(filePath);
                if(tmpsize==0||file.lastModified().toString("yyMMddhhmm").toInt()>tmpsize){
                    path=filePath;
                    tmpsize=file.lastModified().toString("yyMMddhhmm").toInt();
                }
            }
        }
        if(path.isEmpty()&&filePaths.size()>0){
            foreach (QString filePath, filePaths) {
                if(!filePath.contains(".txt")&&!filePath.contains(ID)){
                    QFileInfo file(filePath);
                    if(tmpsize==0||file.lastModified().toString("yyMMddhhmm").toInt()>tmpsize){
                        path=filePath;
                        tmpsize=file.lastModified().toString("yyMMddhhmm").toInt();
                    }
                }
            }
            if(path.isEmpty()){path=filePaths[filePaths.size()-1];}
        }
        if(filePaths.size()==0){path="";}
        break;
    case JoinFileflag:
    case APPFileflag:
        foreach (QString filePath, filePaths) {
            if(!filePath.contains(".txt")){
                QFileInfo file(filePath);
                if(tmpsize==0||file.lastModified().toString("yyMMddhhmm").toInt()>tmpsize){
                    path=filePath;
                    tmpsize=file.lastModified().toString("yyMMddhhmm").toInt();
                }
            }
        }
        if(path.isEmpty()&&filePaths.size()>0){
            path=filePaths[filePaths.size()-1];
        }
        if(filePaths.size()==0){path="";}
        break;
    case EEFileflag:
    case ReadyFileflag:
        if(filePath->size()==0){path="";}
        foreach (QString file, filePaths) {
            if(!file.contains("~$")){
                path=file;
            }
        }
        break;
    case ConfigFileflag:
        if(filePath->size()==0){path="";}
        foreach (QString file, filePaths) {
            tmpsize=file.mid(file.indexOf(".")+1,2).toInt();
            if(tmpsize>size){
                size=tmpsize;
                path=file;
            }
        }
        break;
    }
    (*filePath)=path;
}
/**
 * @def ini文件写入
 * @brief CommonMethod::INIFileWrite
 * @param filePath
 * @param PartNumber
 * @param DiagnosticCode
 */
void CommonMethod::INIFileWrite(const QString filePath, const QString PartNumber, const QString DiagnosticCode)
{
    QLogHelper::instance()->LogInfo("CommonMethod->INIFileWrite() 函数执行!");
    QFile *file=new QFile(filePath);
    QByteArray tmpByte=PartNumber.toLatin1();
    char *tmp=tmpByte.data();
    QString str,strList;
    QTextStream out(file);
    out.setCodec("Shift-JIS");
    if(file->exists(filePath)&&file->open(QIODevice::ReadOnly))
    {
        str=out.readLine();
        while (!str.isNull()) {
            if(str.contains("LOG_ZONE_IDENT_00"))
            {
                str="LOG_ZONE_IDENT_00=0x"+QString("%1").arg(tmp[0],2,16,QChar('0')).toUpper()+"			;日産部番0, '"+tmp[0]+"'=0x"+QString("%1").arg(tmp[0],2,16,QChar('0')).toUpper();
            }else if (str.contains("LOG_ZONE_IDENT_01")) {
                str="LOG_ZONE_IDENT_01=0x"+QString("%1").arg(tmp[1],2,16,QChar('0')).toUpper()+"			;日産部番0, '"+tmp[1]+"'=0x"+QString("%1").arg(tmp[1],2,16,QChar('0')).toUpper();
            }else if (str.contains("LOG_ZONE_IDENT_02")) {
                str="LOG_ZONE_IDENT_02=0x"+QString("%1").arg(tmp[2],2,16,QChar('0')).toUpper()+"			;日産部番0, '"+tmp[2]+"'=0x"+QString("%1").arg(tmp[2],2,16,QChar('0')).toUpper();
            }else if (str.contains("LOG_ZONE_IDENT_03")) {
                str="LOG_ZONE_IDENT_03=0x"+QString("%1").arg(tmp[3],2,16,QChar('0')).toUpper()+"			;日産部番0, '"+tmp[3]+"'=0x"+QString("%1").arg(tmp[3],2,16,QChar('0')).toUpper();
            }else if (str.contains("LOG_ZONE_IDENT_04")) {
                str="LOG_ZONE_IDENT_04=0x"+QString("%1").arg(tmp[4],2,16,QChar('0')).toUpper()+"			;日産部番0, '"+tmp[4]+"'=0x"+QString("%1").arg(tmp[4],2,16,QChar('0')).toUpper();
            }else if (str.contains("LOG_ZONE_IDENT_05")) {
                str="LOG_ZONE_IDENT_05="+DiagnosticCode+"			;診断識別コード, EntryAVM(0x09), SMB(0x80)";
            }
            strList.append(str+"\n");
            str=out.readLine();
        }
        file->close();
    }
    if(file->exists(filePath)&&file->open(QIODevice::WriteOnly|QIODevice::Text))
    {
        out << strList;
        file->close();
    }
}
/**
 * @def ini文件读取
 * @brief CommonMethod::INIFileRead
 * @param filePath
 * @param PartNumber
 * @param DiagnosticCode
 * @return
 */
bool CommonMethod::INIFileRead(const QString filePath, const QString PartNumber, const QString DiagnosticCode)
{
    QLogHelper::instance()->LogInfo("CommonMethod->INIFileRead() 函数执行!");
    QFile *file=new QFile(filePath);
    QByteArray tmpByte=PartNumber.toLatin1();
    char *tmp=tmpByte.data();
    QString tmpString,str;
    QTextStream out(file);
    out.setCodec("Shift-JIS");
    if(file->exists(filePath)&&file->open(QIODevice::ReadOnly))
    {
        out.setCodec("Shift-JIS");
        str=out.readLine();
        while (!str.isNull()) {
            if(str.contains("LOG_ZONE_IDENT_00"))
            {
                tmpString="LOG_ZONE_IDENT_00=0x"+QString("%1").arg(tmp[0],2,16,QChar('0')).toUpper()+"			;日産部番0, '"+tmp[0]+"'=0x"+QString("%1").arg(tmp[0],2,16,QChar('0')).toUpper();
            }else if (str.contains("LOG_ZONE_IDENT_01")) {
                tmpString="LOG_ZONE_IDENT_01=0x"+QString("%1").arg(tmp[1],2,16,QChar('0')).toUpper()+"			;日産部番0, '"+tmp[1]+"'=0x"+QString("%1").arg(tmp[1],2,16,QChar('0')).toUpper();
            }else if (str.contains("LOG_ZONE_IDENT_02")) {
                tmpString="LOG_ZONE_IDENT_02=0x"+QString("%1").arg(tmp[2],2,16,QChar('0')).toUpper()+"			;日産部番0, '"+tmp[2]+"'=0x"+QString("%1").arg(tmp[2],2,16,QChar('0')).toUpper();
            }else if (str.contains("LOG_ZONE_IDENT_03")) {
                tmpString="LOG_ZONE_IDENT_03=0x"+QString("%1").arg(tmp[3],2,16,QChar('0')).toUpper()+"			;日産部番0, '"+tmp[3]+"'=0x"+QString("%1").arg(tmp[3],2,16,QChar('0')).toUpper();
            }else if (str.contains("LOG_ZONE_IDENT_04")) {
                tmpString="LOG_ZONE_IDENT_04=0x"+QString("%1").arg(tmp[4],2,16,QChar('0')).toUpper()+"			;日産部番0, '"+tmp[4]+"'=0x"+QString("%1").arg(tmp[4],2,16,QChar('0')).toUpper();
            }else if (str.contains("LOG_ZONE_IDENT_05")) {
                tmpString="LOG_ZONE_IDENT_05="+DiagnosticCode+"			;診断識別コード, EntryAVM(0x09), SMB(0x80)";
            }
            if(!tmpString.isEmpty()&&str!=tmpString){return false;}
            str=out.readLine();
            tmpString="";
        }
        file->close();
    }
    return true;
}
/**
 * @def 此函数主要对UIMethod.cpp 中 MessageViewModelEditedSlot() 函数进一步处理
 * @brief CommonMethod::MessageFileTableChangeDeal
 * @param item
 * @param filePath
 */
void CommonMethod::MessageFileTableChangeDeal(QStandardItem *item, QString *filePath)
{
    QLogHelper::instance()->LogInfo("CommonMethod->MessageTableChangeDeal() 函数执行!");
    QFile *file=new QFile();
    if(file->exists(item->text())){
        (*filePath)=item->text();
    }else{
        item->setText(*filePath);
    }
}
/**
 * @def 此函数主要对UIMethod.cpp 中 MessageViewModelEditedSlot() 函数进一步处理
 * @brief CommonMethod::MessageSoftTableChangeDeal
 * @param item
 * @param soft
 * @param index
 * @return
 */
SOFTNUMBERTable CommonMethod::MessageSoftTableChangeDeal(const QStandardItem *item,const SOFTNUMBERTable soft,int index)
{
    QLogHelper::instance()->LogInfo("CommonMethod->MessageSoftTableChangeDeal() 函数执行!");
    SOFTNUMBERTable tmpsoft=soft;
    switch (index) {
    case 0:
        tmpsoft.ModelNumber=item->text();
        break;
    case 1:
        tmpsoft.CarModels=item->text();
        break;
    case 2:
        tmpsoft.PartNumber=item->text();
        break;
    case 3:
        tmpsoft.CANGen=item->text();
        break;
    case 4:
        tmpsoft.Productionstage=item->text();
        break;
    case 5:
        tmpsoft.ApplicationPartNo=item->text();
        break;
    case 6:
        tmpsoft.ApplicationVer=item->text();
        break;
    case 7:
        tmpsoft.CarInfoPartNo=item->text();
        break;
    case 8:
        tmpsoft.CarInfoVer=item->text();
        break;
    case 9:
        tmpsoft.CameraMAPPartNo=item->text();
        break;
    case 10:
        tmpsoft.CameraMAPVer=item->text();
        break;
    case 11:
        tmpsoft.OSDPartNo=item->text();
        break;
    case 12:
        tmpsoft.OSDVer=item->text();
        break;
    case 13:
        tmpsoft.CANfblPartNo=item->text();
        break;
    case 14:
        tmpsoft.CANfblVer=item->text();
        break;
    case 15:
        tmpsoft.BootloaderPartNo=item->text();
        break;
    case 16:
        tmpsoft.BootloaderVer=item->text();
        break;
    case 17:
        tmpsoft.DiagnosticCode=item->text();
        break;
    }
    return tmpsoft;
}


/**
 * @def 获取当前天数往后推四天后的日期
 * @brief CommonMethod::GetDate
 * @return
 */
QStringList CommonMethod::GetDate()
{
    QLogHelper::instance()->LogInfo("CommonMethod->GetDate() 函数执行!");
    QStringList strList;
    QDateTime currenttime = QDateTime::currentDateTime();
    QString currentYear=currenttime.toString("yyyy");
    QString currentWeek=currenttime.toString("ddd");
    QString currentMouth=currenttime.toString("M");
    QString currentDay=currenttime.toString("d");
    int currentMouthNumber=31;
    if(currentMouth.toInt()==2){
        currentMouthNumber=28;
        if((currentYear.toInt()%100==0&&currentMouth.toInt()%400==0)||(currentYear.toInt()%100!=0&&currentMouth.toInt()%4==0)){
            currentMouthNumber=29;
        }
    }
    if(currentMouth.toInt()==4||currentMouth.toInt()==6||currentMouth.toInt()==9||currentMouth.toInt()==11){
        currentMouthNumber=30;
    }
    if(currentWeek=="周一"){
        if(currentDay.toInt()+4<=currentMouthNumber)
        {
            currentDay=QString::number(currentDay.toInt()+4);
        }else{
            currentMouth=QString::number(currentDay.toInt()+4);
            currentDay=QString::number(currentDay.toInt()+4-currentMouthNumber);
        }
    }
    if(currentWeek=="周二"||currentWeek=="周三"||currentWeek=="周四"||currentWeek=="周五"){
        if(currentDay.toInt()+6<currentMouthNumber)
        {
            currentDay=QString::number(currentDay.toInt()+6);
        }else{
            currentMouth=QString::number(currentDay.toInt()+6);
            currentDay=QString::number(currentDay.toInt()+6-currentMouthNumber);
        }
    }
    strList.append(currentMouth);
    strList.append(currentDay);
    return strList;
}
/**
 * @def 错误信息添加
 * @brief CommonMethod::SetErrorTable
 * @param errTableList
 * @param fileName
 * @param sheetName
 * @param row
 * @param col
 * @param errMessage
 */
void CommonMethod::SetErrorTable(QList<ERRORTable> *errTableList, const QString fileName, const QString sheetName, const unsigned int row, const unsigned int col, const QString errMessage)
{
    ERRORTable err;
    //QLogHelper::instance()->LogDebug(QString::number(row)+"  "+QString::number(col));
    err.fileName=fileName.mid(fileName.lastIndexOf("/")+1);
    err.sheetName=sheetName;
    err.row=row;
    err.col=col;
    err.errMessage=errMessage;
    errTableList->append(err);
}

/**
 * @def 获取本机7z程序安装位置
 * @brief CommonMethod::Get7zInstallPath
 * @return
 */
QString CommonMethod::Get7zInstallPath()
{
    QLogHelper::instance()->LogInfo("CommonMethod->Get7zInstallPath() 函数执行!");
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\7-Zip",QSettings::NativeFormat);
    return settings.value("Path").toString()+"7z.exe";
}
/**
 * @def 字符串整体替换
 * @brief CommonMethod::StringALLReplace
 * @param str
 * @param befstr
 * @param aftstr
 * @return
 */
QString CommonMethod::StringALLReplace(const QString str, const QString befstr, const QString aftstr)
{
    QLogHelper::instance()->LogInfo("CommonMethod->StringALLReplace() 函数执行!");
    QStringList tmp;
    QString strtmp;
    tmp=str.split(befstr);
    for(int i=0;i<tmp.size();i++)
    {
         strtmp=strtmp+tmp[i]+aftstr;
    }
    strtmp=strtmp.left(strtmp.lastIndexOf(aftstr));
    return strtmp;
}
