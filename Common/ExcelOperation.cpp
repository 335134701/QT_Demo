#include "ExcelOperation.h"

ExcelOperation::ExcelOperation(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("ExcelOperation() 构造函数执行!");
}

bool ExcelOperation::Init(const QString filePath)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->Init() 函数执行!");
    if(!QFile::exists(filePath)){return false;}
    if(filePath.endsWith("xls",Qt::CaseInsensitive))//判断是否是.xls文件，不区分大小写
    {
        book = xlCreateBook(); // xlCreateBook() for xls
    }
    else if(filePath.endsWith("xlsx",Qt::CaseInsensitive))//判断是否是.xlsx文件，不区分大小写
    {
        book = xlCreateXMLBook(); // xlCreateXMLBook() for xlsx
    }
    book->setKey("Tool", "windows-2b21230302c2ee006fb56d68a1s2sbic");
    return true;
}
/**
 * @brief ExcelOperation::ReadSoftExcel
 * @param filePath
 * @param ID
 * @param IDType
 * @return
 */
QList<SOFTNUMBERTable> ExcelOperation::ReadSoftExcel(const QString filePath, const QString ID, const QString IDType)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadExcel() 函数执行!");
    bool flag=false;
    Sheet *sheetread;
    QByteArray byte;
    QList<SOFTNUMBERTable> *softlist=new QList<SOFTNUMBERTable>();
    QString CarModels,PartNumber,CANGen;
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        if(IDType!="EntryAVM2"){
            sheetread = book->getSheet(0);
        }else{
            sheetread = book->getSheet(1);
        }
        if(sheetread==NULL){return *softlist;}
        int rowNum = sheetread->lastRow();
        for (int i = 1; i < rowNum; ++i)
        {
            if(ID==QString::fromLocal8Bit(sheetread->readStr(i,1))||(flag&&QString::fromLocal8Bit(sheetread->readStr(i,1)).isEmpty())){
                flag=true;
            }else{flag=false;}
            if(flag)
            {
                if(CarModels.isEmpty()){CarModels=QString(sheetread->readStr(i,2));}
                if(PartNumber.isEmpty()){PartNumber=QString(sheetread->readStr(i,3));}
                if(CANGen.isEmpty()){CANGen=QString(sheetread->readStr(i,4));}
                if((sheetread->cellFormat(i,5)->patternForegroundColor()==13||sheetread->cellFormat(i,5)->patternForegroundColor()==51))
                {
                    SOFTNUMBERTable soft;
                    soft.ModelNumber=ID;
                    soft.CarModels=CarModels;
                    soft.PartNumber=PartNumber;
                    soft.CANGen=CANGen;
                    byte.append(sheetread->readStr(i,5));
                    soft.Productionstage=codec->toUnicode(byte);
                    byte.clear();
                    soft.ApplicationPartNo=QString(sheetread->readStr(i,11));
                    soft.ApplicationVer=QString(sheetread->readStr(i,12));
                    soft.CarInfoPartNo=QString(sheetread->readStr(i,13));
                    soft.CarInfoVer=QString(sheetread->readStr(i,14));
                    soft.CameraMAPPartNo=QString(sheetread->readStr(i,15));
                    soft.CameraMAPVer=QString(sheetread->readStr(i,16));
                    soft.OSDPartNo=QString(sheetread->readStr(i,17));
                    soft.OSDVer=QString(sheetread->readStr(i,18));
                    soft.CANfblPartNo=QString(sheetread->readStr(i,19));
                    soft.CANfblVer=QString(sheetread->readStr(i,20));
                    soft.BootloaderPartNo=QString(sheetread->readStr(i,21));
                    soft.BootloaderVer=QString(sheetread->readStr(i,22));
                    byte.append(sheetread->readStr(i,23));
                    soft.DiagnosticCode = codec->toUnicode(byte);
                    byte.clear();
                    softlist->append(soft);
                }
            }
        }
        book->release();
    }
    return *softlist;
}
/**
 * @brief ExcelOperation::ReadConfExcel
 * @param filePath
 * @param ID
 * @param IDType
 * @return
 */
QList<CONFIGTable> ExcelOperation::ReadConfExcel(const QString filePath, const QString ID)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadConfExcel() 函数执行!");
    bool flag=false;
    Sheet *sheetread;
    QByteArray byte;
    QList<CONFIGTable> *conflist=new QList<CONFIGTable>();
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        sheetread = book->getSheet(1);
        if(sheetread==NULL){return *conflist;}
        int rowNum = sheetread->lastRow();
        for (int i = 1; i < rowNum; ++i)
        {
            if(QString::fromLocal8Bit(sheetread->readStr(i,2)).contains(ID)){
                flag=true;
            }else{flag=false;}
            if(flag){
                CONFIGTable conf;
                byte.append(sheetread->readStr(i,3));
                conf.Vehicletype=codec->toUnicode(byte);            //車種 Vehicle
                byte.clear();
                byte.append(sheetread->readStr(i,6));
                conf.CANGen=codec->toUnicode(byte);                    //ITS
                byte.clear();
                byte.append(sheetread->readStr(i,9));
                conf.ITS=codec->toUnicode(byte);                    //ITS
                byte.clear();
                byte.append(sheetread->readStr(i,11));
                conf.PickMethod=codec->toUnicode(byte);              //接続先・方式
                byte.clear();
                byte.append(sheetread->readStr(i,13));
                conf.Destination=codec->toUnicode(byte);             //仕向け
                byte.clear();
                byte.append(sheetread->readStr(i,15));
                conf.Wheelspeedpulse=codec->toUnicode(byte);         //車輪速パルス
                byte.clear();
                byte.append(sheetread->readStr(i,17));
                conf.CANspecifications=codec->toUnicode(byte);       //CAN仕様
                byte.clear();
                byte.append(sheetread->readStr(i,19));
                conf.Camerasystem=codec->toUnicode(byte);            //カメラシステム
                byte.clear();
                byte.append(sheetread->readStr(i,21));
                conf.WAS=codec->toUnicode(byte);                     //4WAS有無
                byte.clear();
                byte.append(sheetread->readStr(i,23));
                conf.Steeringgearratio=codec->toUnicode(byte);       //ステアリングギア比
                byte.clear();
                byte.append(sheetread->readStr(i,25));
                conf.VCANsonar=codec->toUnicode(byte);              //V-CANソナー有無
                byte.clear();
                byte.append(sheetread->readStr(i,27));
                conf.Sonarinterrupt=codec->toUnicode(byte);          //ソナー割り込み表示機能有無
                byte.clear();
                byte.append(sheetread->readStr(i,29));
                conf.Expectedadroute=codec->toUnicode(byte);         //予想進路線の有無
                byte.clear();
                byte.append(sheetread->readStr(i,31));
                conf.Steerspecifications=codec->toUnicode(byte);     //左右ステアリング仕様
                byte.clear();
                byte.append(sheetread->readStr(i,33));
                conf.Mission=codec->toUnicode(byte);                 //ミッション
                byte.clear();
                byte.append(sheetread->readStr(i,43));
                conf.BCIFunction=codec->toUnicode(byte);           //BCI機能有無
                byte.clear();
                byte.append(sheetread->readStr(i,45));
                conf.RRREBfunction=codec->toUnicode(byte);           //RR　REB機能有無
                byte.clear();
                byte.append(sheetread->readStr(i,51));
                conf.BCWfunction=codec->toUnicode(byte);             //BCW機能有無
                byte.clear();
                byte.append(sheetread->readStr(i,53));
                conf.Buzzer=codec->toUnicode(byte);                  //ブザー
                byte.clear();
                byte.append(sheetread->readStr(i,57));
                conf.MeterSW=codec->toUnicode(byte);                //メーターSW
                byte.clear();
                byte.append(sheetread->readStr(i,63));
                conf.OFFROADMODE=codec->toUnicode(byte);             //OFF ROAD MODE
                byte.clear();
                byte.append(sheetread->readStr(i,67));
                conf.Movingway=codec->toUnicode(byte);               //駆動方式
                byte.clear();
                byte.append(sheetread->readStr(i,71));
                conf.DAS=codec->toUnicode(byte);                    //DAS機能有無
                byte.clear();
                byte.append(sheetread->readStr(i,73));
                conf.PSRfunction=codec->toUnicode(byte);             //PSR機能有無
                byte.clear();
                byte.append(sheetread->readStr(i,75));
                conf.Rearnormalview=codec->toUnicode(byte);          //リアノーマルビュー有無
                byte.clear();
                byte.append(sheetread->readStr(i,83));
                conf.Enginespecifications=codec->toUnicode(byte);    //エンジン仕様
                byte.clear();
                byte.append(sheetread->readStr(i,85));
                conf.Tiresize=codec->toUnicode(byte);                //タイヤサイズ
                byte.clear();
                byte.append(sheetread->readStr(i,89));
                conf.Configpartnumber=codec->toUnicode(byte);        //コンフィグ部番
                byte.clear();
                conflist->append(conf);
            }
        }
        book->release();
    }
    return *conflist;
}
/**
 * @def EE-A002-1000 DR会議運用手順_様式7_20190320_Entry2AVM_EN3445PC_20210419.xlsx 文件写入
 * @brief ExcelOperation::EEFileWrite
 * @param filePath
 * @param ID
 * @param IDType
 * @param RelyID
 * @param softNumberTable
 * @param errTableList
 */
void ExcelOperation::EEFileWrite(const QString filePath, const QString ID,const QString IDType,const QString RelyID, QList<SOFTNUMBERTable> *softNumberTable,QList<ERRORTable> *errTableList)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->EEFileWrite() 函数执行!");
    Sheet *sheetread;
    QStringList strList=CommonMethod::GetDate();
    QDateTime curDateTime=QDateTime::currentDateTime();
    int Years=curDateTime.toString("yyyy").toInt();
    int Mounth=curDateTime.toString("M").toInt();
    int Day=curDateTime.toString("d").toInt();
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        sheetread = book->getSheet(0);
        if(sheetread==NULL){
            CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),0,0,codec->toUnicode(QByteArray(sheetread->name())+"表不存在!"));
            return;
        }
        sheetread->writeNum(3,4,Years);
        sheetread->writeNum(11,5,Years);
        sheetread->writeNum(12,5,Years);
        sheetread->writeNum(3,6,Mounth);
        sheetread->writeNum(11,7,Mounth);
        sheetread->writeNum(12,7,QString(strList.value(0)).toInt());
        sheetread->writeNum(3,9,Day);
        sheetread->writeNum(11,9,Day);
        sheetread->writeNum(12,9,QString(strList.value(1)).toInt());
        sheetread->writeStr(7,6,ID.left(7).mid(2).toLocal8Bit().data());
        sheetread->writeStr(7,9,ID.right(1).toLocal8Bit().data());

        //此处需要确认是否为母体，如果为母体，则需要写入母体，而不是当前ID
        if(!RelyID.isEmpty()){}
        sheetread->writeStr(15,21,ID.left(7).mid(2).toLocal8Bit().data());

        sheetread->writeStr(15,24,ID.right(1).toLocal8Bit().data());
        sheetread->writeStr(9,4,("[　"+IDType+"　　　　　　　　　　　　　　]").toLocal8Bit().data());
        if(softNumberTable->size()>0){
            sheetread->writeStr(8,17,softNumberTable->value(0).CarModels.toLocal8Bit().data());
            sheetread->writeStr(10,23,(softNumberTable->value(softNumberTable->size()-1).ApplicationVer.left(2)+"."+softNumberTable->value(softNumberTable->size()-1).ApplicationVer.left(4).mid(2)+"."+softNumberTable->value(softNumberTable->size()-1).ApplicationVer.right(2)).toLatin1().data());
            sheetread->writeStr(40,29,("Soft Version組み合わせ　：　APP=Ver."+softNumberTable->value(softNumberTable->size()-1).ApplicationVer.left(2)+"."+softNumberTable->value(softNumberTable->size()-1).ApplicationVer.left(4).mid(2)+"."+softNumberTable->value(softNumberTable->size()-1).ApplicationVer.right(2)+ \
                                       " / CarInfo=Ver."+softNumberTable->value(softNumberTable->size()-1).CarInfoVer.left(2)+"."+softNumberTable->value(softNumberTable->size()-1).CarInfoVer.left(4).mid(2)+ \
                                       "/CameraMAP=Ver."+softNumberTable->value(softNumberTable->size()-1).CameraMAPVer.left(2)+"."+softNumberTable->value(softNumberTable->size()-1).CameraMAPVer.left(4).mid(2)+ \
                                       "/OSD=Ver."+softNumberTable->value(softNumberTable->size()-1).OSDVer.left(2)+"."+softNumberTable->value(softNumberTable->size()-1).OSDVer.left(4).mid(2)+ \
                                       "/Boot=Ver."+softNumberTable->value(softNumberTable->size()-1).BootloaderVer.left(2)+"."+softNumberTable->value(softNumberTable->size()-1).BootloaderVer.left(4).mid(2)+ \
                                       "/CANfbl=Ver."+softNumberTable->value(softNumberTable->size()-1).CANfblVer.left(2)+"."+softNumberTable->value(softNumberTable->size()-1).CANfblVer.left(4).mid(2)).toLocal8Bit().data());
            sheetread->writeStr(41,29,("診断識別コード　：　"+softNumberTable->value(softNumberTable->size()-1).DiagnosticCode).toLocal8Bit().data());
            if(IDType=="NextPh3")
            {
                sheetread->writeStr(42,29,QString("Version管理表　：　http://turtle.ct.clarion/svn/NextPh3/Documents/06_software02_支援プロセス/05_変更構成管理/02_設計変更管理").toLocal8Bit().data());
                sheetread->writeStr(43,29,QString("Config情報　：　\\claris-si\\P_CAMERA_NISSAN_SH7766_PF\09_次世代PH3$01_project\11_車両日程_車両展開\01_工場コンフィグ").toLocal8Bit().data());
            }
        }
        book->save(filePath.toLocal8Bit());
        book->release();
    }else{
        CommonMethod::SetErrorTable(errTableList,filePath,"",0,0,filePath+"文件不存在!");
        return;
    }
    return;
}
/**
 * @def P02F-PRC 5R00A 確認シート.xlsx 文件写入
 * @brief ExcelOperation::ReadyFileWrite
 * @param filePath
 * @param softNumberTable
 * @param configTable
 * @param DefineConfigList
 * @param RelyID
 * @return
 */
void ExcelOperation::ReadyFileWrite(const QString filePath, QList<SOFTNUMBERTable> *softNumberTable, QList<CONFIGTable> *configTable,QStringList DefineConfigList,const QString IDType,const QString RelyID,QList<ERRORTable> *errTableList)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadyFileWrite() 函数执行!");
    ReadyFileFirstSheet(filePath,softNumberTable,configTable,errTableList);
    ReadyFileSecondSheet(filePath,configTable,errTableList);
    if(RelyID.isEmpty()){
        ReadyFileThirdSheet(filePath,softNumberTable,DefineConfigList,IDType,false,errTableList);
    }else{
        ReadyFileThirdSheet(filePath,softNumberTable,DefineConfigList,IDType,true,errTableList);
    }
    return;
}


/**
 * @def 確認シート.xlsx 表第一个sheet修改
 * @brief ExcelOperation::ReadyFileFirstSheet
 * @param filePath
 * @param softNumberTable
 * @param configTable
 * @param errTable
 */
void ExcelOperation::ReadyFileFirstSheet(const QString filePath, QList<SOFTNUMBERTable> *softNumberTable, QList<CONFIGTable> *configTable,QList<ERRORTable> *errTableList)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadyFileFirstSheet() 函数执行!");
    Sheet *sheetread;
    int firstRow=0,firstCol=0,i=0;
    QString ConfigpartnumberList;
    Format* format;
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        sheetread=book->getSheet(0);
        //最少两种状态，如果有更多状态，可以进一步处理
        if(sheetread->lastRow()<11){
            CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),0,0,codec->toUnicode(QByteArray(sheetread->name())+"表格式错误!"));
            return;
        }
        firstRow=sheetread->firstRow()+6;
        firstCol=sheetread->firstCol()+1;
        for(i=0;i<configTable->size();i++){
            ConfigpartnumberList.append(configTable->value(i).Configpartnumber+"\n");
        }
        for(i=0;i<softNumberTable->size();i++){
            SOFTNUMBERTable soft=softNumberTable->value(i);
            sheetread->writeStr(firstRow+i*2,firstCol,soft.CarModels.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+1,soft.PartNumber.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+2,soft.Productionstage.toLocal8Bit().data());
            if(configTable->size()<5)
            {
                format=book->addFormat(sheetread->cellFormat(firstCol+i*2,firstCol+3));
                format->font()->setSize(11);
                format->setBorder(BORDERSTYLE_THIN);
                sheetread->writeStr(firstRow+i*2,firstCol+3,ConfigpartnumberList.toLocal8Bit().data(),format);
            }else{
                sheetread->writeStr(firstRow+i*2,firstCol+3,ConfigpartnumberList.toLocal8Bit().data());
            }
            format=book->addFormat(sheetread->cellFormat(firstCol+i*2,firstCol+4));
            format->setBorder(BORDERSTYLE_THIN);
            sheetread->writeStr(firstRow+i*2,firstCol+4,soft.DiagnosticCode.toLocal8Bit().data(),format);
            sheetread->writeStr(firstRow+i*2,firstCol+5,soft.ApplicationPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+6,soft.ApplicationVer.toLocal8Bit().data());
            if(soft.Productionstage=="AKM対応"){
                sheetread->writeStr(firstRow+i*2,firstCol+7,QString("AKM対応").toLocal8Bit().data());
            }else{
                if(i>0){
                    SetPatternForegroundColor(sheetread,firstRow+i*2,firstCol+7,COLOR_RED);
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i*2,firstCol+7,"需要说明内容");
                    sheetread->writeStr(firstRow+i*2,firstCol+7,QString("").toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i*2,firstCol+10,QString("").toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i*2,firstCol+13,QString("").toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i*2,firstCol+16,QString("").toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i*2,firstCol+19,QString("").toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i*2,firstCol+21,QString("").toLocal8Bit().data());
                }else{
                    sheetread->writeStr(firstRow+i*2,firstCol+7,QString("母体").toLocal8Bit().data());
                }
            }
            sheetread->writeStr(firstRow+i*2,firstCol+8,soft.CarInfoPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+9,soft.CarInfoVer.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+11,soft.CameraMAPPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+12,soft.CameraMAPVer.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+14,soft.OSDPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+15,soft.OSDVer.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+17,soft.CANfblPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+18,soft.CANfblVer.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+20,soft.BootloaderPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i*2,firstCol+21,soft.BootloaderVer.toLocal8Bit().data());
            if(i>0)
            {
                if((soft.ApplicationPartNo!=softNumberTable->value(i-1).ApplicationPartNo)||((soft.ApplicationVer.left(1)=="A"&&softNumberTable->value(i-1).ApplicationVer.left(1)!="0")||(soft.ApplicationVer.left(1)=="B"&&softNumberTable->value(i-1).ApplicationVer.left(1)!="1")))
                {
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i*2,firstCol+7,"Application不一致错误");
                    SetPatternForegroundColor(sheetread,firstRow+i*2,firstCol+7,COLOR_RED);
                }
                if((soft.CarInfoPartNo!=softNumberTable->value(i-1).CarInfoPartNo)||(soft.CarInfoVer!=softNumberTable->value(i-1).CarInfoVer))
                {
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i*2,firstCol+10,"CarInfo不一致错误");
                    SetPatternForegroundColor(sheetread,firstRow+i*2,firstCol+10,COLOR_RED);
                }
                if((soft.CameraMAPPartNo!=softNumberTable->value(i-1).CameraMAPPartNo)||(soft.CameraMAPVer!=softNumberTable->value(i-1).CameraMAPVer))
                {
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i*2,firstCol+13,"CameraMAP不一致错误");
                    SetPatternForegroundColor(sheetread,firstRow+i*2,firstCol+13,COLOR_RED);
                }
                if((soft.OSDPartNo!=softNumberTable->value(i-1).OSDPartNo)||(soft.OSDVer!=softNumberTable->value(i-1).OSDVer))
                {
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i*2,firstCol+16,"OSD不一致错误");
                    SetPatternForegroundColor(sheetread,firstRow+i*2,firstCol+16,COLOR_RED);
                }
                if((soft.CANfblPartNo!=softNumberTable->value(i-1).CANfblPartNo)||(soft.CANfblVer!=softNumberTable->value(i-1).CANfblVer))
                {
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i*2,firstCol+19,"CANfbl不一致错误");
                    SetPatternForegroundColor(sheetread,firstRow+i*2,firstCol+19,COLOR_RED);
                }
                if((soft.BootloaderPartNo!=softNumberTable->value(i-1).BootloaderPartNo)||(soft.BootloaderVer!=softNumberTable->value(i-1).BootloaderVer))
                {
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i*2,firstCol+21,"Bootloader不一致错误");
                    SetPatternForegroundColor(sheetread,firstRow+i*2,firstCol+21,COLOR_RED);
                }
            }

        }
        book->save(filePath.toLocal8Bit());
        book->release();
    }else{
        CommonMethod::SetErrorTable(errTableList,filePath,"",0,0,filePath+"文件不存在!");
        return;
    }
    return;
}
/**
 * @def 確認シート.xlsx 表第二个sheet修改
 * @brief ExcelOperation::ReadyFileSecondSheet
 * @param filePath
 * @param configTable
 * @param errTable
 * @return
 */
void ExcelOperation::ReadyFileSecondSheet(const QString filePath, QList<CONFIGTable> *configTable,QList<ERRORTable> *errTableList)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadyFileSecondSheet() 函数执行!");
    int firstRow=0,i=0,lastRow=0;
    QString ConfigpartnumberList;
    Sheet *sheetread;
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        sheetread=book->getSheet(1);
        firstRow=sheetread->firstRow()+11;
        lastRow=sheetread->lastRow();
        for(i=0;i<configTable->size();i++){
            ConfigpartnumberList.append(configTable->value(i).Configpartnumber+"/");
        }
        //QLogHelper::instance()->LogDebug(QString::number(firstRow)+"  "+QString::number(firstCol));
        ConfigpartnumberList=ConfigpartnumberList.left(ConfigpartnumberList.lastIndexOf("/"));
        sheetread->writeStr(sheetread->firstRow(),sheetread->firstCol()+11,ConfigpartnumberList.toLocal8Bit().data());
        sheetread->writeStr(sheetread->firstRow()+1,sheetread->firstCol()+11,QString("").toLocal8Bit().data());
        //模板文件有问题，直接返回
        if(sheetread->lastRow()<=12){
            CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),0,0,codec->toUnicode(QByteArray(sheetread->name())+"表格式错误!"));
            return;
        }
        //如果解析出来无数据
        if(configTable->size()==0){
            sheetread->removeRow(12,sheetread->lastRow());
            return;
        }
        //如果模板列多余需要的列，则删除多余的模板列
        if(configTable->size()+12<lastRow)
        {
            sheetread->removeRow(configTable->size()+12,sheetread->lastRow());
        }
        //如果模板列少于需要的列，则需要复制单元格式
        if(configTable->size()+12>sheetread->lastRow())
        {
            Format * format;
            for(i=0;i<(configTable->size()+12)-sheetread->lastRow();i++)
            {
                for(int j=0;j<sheetread->lastCol();j++)
                {
                    format=book->addFormat(sheetread->cellFormat(lastRow-1,j));
                    sheetread->writeStr(lastRow+i,j,sheetread->readStr(lastRow-1,j),format);
                }
            }
        }
        for(i=0;i<configTable->size();i++){
            CONFIGTable conf=configTable->value(i);
            sheetread->writeStr(firstRow+i,2,(conf.Vehicletype.left(conf.Vehicletype.indexOf("("))).toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,3,conf.ITS.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,5,conf.PickMethod.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,7,conf.Destination.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,9,conf.Wheelspeedpulse.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,11,conf.CANspecifications.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,13,conf.Camerasystem.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,15,conf.WAS.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,17,conf.Steeringgearratio.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,19,conf.VCANsonar.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,21,conf.Sonarinterrupt.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,23,conf.Expectedadroute.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,25,conf.Steerspecifications.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,27,conf.Mission.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,37,conf.BCIFunction.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,39,conf.RRREBfunction.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,45,conf.BCWfunction.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,47,conf.Buzzer.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,51,conf.MeterSW.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,57,conf.OFFROADMODE.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,59,conf.Movingway.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,61,conf.DAS.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,63,conf.PSRfunction.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,65,conf.Rearnormalview.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,69,conf.Enginespecifications.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,71,conf.Tiresize.toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,73,(conf.CANGen+"世代CAN対応").toLocal8Bit().data());
            sheetread->writeStr(firstRow+i,74,conf.Configpartnumber.toLocal8Bit().data());
        }
        book->save(filePath.toLocal8Bit());
        book->release();
    }else{
        CommonMethod::SetErrorTable(errTableList,filePath,"",0,0,filePath+"文件不存在!");
        return;
    }
    return;
}

/**
 * @def 確認シート.xlsx 表第三个sheet修改
 * @brief ExcelOperation::ReadyFileThirdSheet
 * @param filePath
 * @param softNumberTable
 * @param DefineConfigList
 * @param flag
 * @param errTable
 * @return
 */
void ExcelOperation::ReadyFileThirdSheet(const QString filePath,QList<SOFTNUMBERTable> *softNumberTable,QStringList DefineConfigList,const QString IDType,bool flag,QList<ERRORTable> *errTableList)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadyFileThirdSheet() 函数执行!");
    Sheet *sheetread;
    int i=0,firstRow=4;
    QString Vehicletype;
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        sheetread=book->getSheet(2);
        if(sheetread->lastRow()<4){
            CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),0,0,codec->toUnicode(QByteArray(sheetread->name())+"表格式错误!"));
            return;
        }
        if(softNumberTable->value(0).CarModels.contains("-")){
            Vehicletype=softNumberTable->value(0).CarModels.left(softNumberTable->value(0).CarModels.indexOf("-"));
        }else if(softNumberTable->value(0).CarModels.contains("_"))
        {
            Vehicletype=softNumberTable->value(0).CarModels.left(softNumberTable->value(0).CarModels.indexOf("_"));
        }
        sheetread->writeStr(1,2,Vehicletype.toLocal8Bit().data());
        sheetread->writeStr(1,15,Vehicletype.toLocal8Bit().data());
        sheetread->writeStr(1,16,Vehicletype.toLocal8Bit().data());
        sheetread->writeStr(1,24,Vehicletype.toLocal8Bit().data());
        sheetread->writeStr(1,32,Vehicletype.toLocal8Bit().data());
        sheetread->writeStr(1,40,Vehicletype.toLocal8Bit().data());
        sheetread->writeStr(3,1,Vehicletype.toLocal8Bit().data());
        for(i=0;i<softNumberTable->size();i++)
        {
            SOFTNUMBERTable soft=softNumberTable->value(i);
            sheetread->writeStr(firstRow+i,1,soft.Productionstage.toLocal8Bit().data());
            if(soft.ApplicationVer.left(1)=="0"){
                sheetread->writeStr(firstRow+i,2,(soft.ApplicationVer.left(2).mid(1)+"."+soft.ApplicationVer.left(4).mid(2)).toLocal8Bit().data());
            }else{
                sheetread->writeStr(firstRow+i,2,(soft.ApplicationVer.left(2)+"."+soft.ApplicationVer.left(4).mid(2)).toLocal8Bit().data());
            }
            if(i==0)
            {
                if(soft.CarModels.contains("_"))
                {
                    Vehicletype=soft.CarModels.replace(soft.CarModels.indexOf("_"),"-");
                }
                sheetread->writeStr(firstRow+i,3,(Vehicletype+"\n"+soft.Productionstage+"\n"+"("+soft.CANGen+"世代CAN)").toLocal8Bit().data());
                sheetread->writeStr(firstRow+i,14,soft.CarInfoVer.toLocal8Bit().data());
                sheetread->writeStr(firstRow+i,15,soft.CarInfoPartNo.toLocal8Bit().data());
                sheetread->writeStr(firstRow+i,21,soft.CameraMAPVer.toLocal8Bit().data());
                sheetread->writeStr(firstRow+i,22,soft.CameraMAPPartNo.toLocal8Bit().data());
                sheetread->writeStr(firstRow+i,28,soft.OSDVer.toLocal8Bit().data());
                sheetread->writeStr(firstRow+i,29,soft.OSDPartNo.toLocal8Bit().data());
                sheetread->writeStr(firstRow+i,35,soft.CANfblVer.toLocal8Bit().data());
                sheetread->writeStr(firstRow+i,36,soft.CANfblPartNo.toLocal8Bit().data());
                sheetread->writeStr(firstRow+i,42,soft.BootloaderVer.toLocal8Bit().data());
                sheetread->writeStr(firstRow+i,43,soft.BootloaderPartNo.toLocal8Bit().data());
            }else{
                sheetread->writeStr(firstRow+i,3,QString("変更無し").toLocal8Bit().data());
                if(soft.CarInfoPartNo==softNumberTable->value(i-1).CarInfoPartNo&&soft.CarInfoVer==softNumberTable->value(i-1).CarInfoVer){
                    sheetread->writeStr(firstRow+i,14,QString("-").toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i,15,QString("-").toLocal8Bit().data());
                }else{
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i,16,"CarInfo不一致错误");
                    sheetread->writeStr(firstRow+i,14,soft.CarInfoVer.toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i,15,soft.CarInfoPartNo.toLocal8Bit().data());
                    SetPatternForegroundColor(sheetread,firstRow+i,16,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,17,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,18,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,19,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,20,COLOR_RED);
                }
                if(soft.CameraMAPPartNo==softNumberTable->value(i-1).CameraMAPPartNo&&soft.CameraMAPVer==softNumberTable->value(i-1).CameraMAPVer){
                    sheetread->writeStr(firstRow+i,21,QString("-").toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i,22,QString("-").toLocal8Bit().data());
                }else{
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i,23,"CameraMAP不一致错误");
                    sheetread->writeStr(firstRow+i,21,soft.CameraMAPVer.toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i,22,soft.CameraMAPPartNo.toLocal8Bit().data());
                    SetPatternForegroundColor(sheetread,firstRow+i,23,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,24,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,25,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,26,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,27,COLOR_RED);
                }
                if(soft.OSDPartNo==softNumberTable->value(i-1).OSDPartNo&&soft.OSDVer==softNumberTable->value(i-1).OSDVer){
                    sheetread->writeStr(firstRow+i,28,QString("-").toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i,29,QString("-").toLocal8Bit().data());
                }else{
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i,30,"OSD不一致错误");
                    sheetread->writeStr(firstRow+i,28,soft.OSDVer.toLatin1().data());
                    sheetread->writeStr(firstRow+i,29,soft.OSDPartNo.toLocal8Bit().data());
                    SetPatternForegroundColor(sheetread,firstRow+i,30,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,31,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,32,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,33,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,34,COLOR_RED);
                }
                if(soft.CANfblPartNo==softNumberTable->value(i-1).CANfblPartNo&&soft.CANfblVer==softNumberTable->value(i-1).CANfblVer){
                    sheetread->writeStr(firstRow+i,35,QString("-").toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i,36,QString("-").toLocal8Bit().data());
                }else{
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i,37,"CANfbl不一致错误");
                    sheetread->writeStr(firstRow+i,35,soft.CANfblVer.toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i,36,soft.CANfblPartNo.toLocal8Bit().data());
                    SetPatternForegroundColor(sheetread,firstRow+i,37,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,38,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,39,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,40,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,41,COLOR_RED);
                }
                if(soft.BootloaderPartNo==softNumberTable->value(i-1).BootloaderPartNo&&soft.BootloaderVer==softNumberTable->value(i-1).BootloaderVer){
                    sheetread->writeStr(firstRow+i,42,QString("-").toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i,43,QString("-").toLocal8Bit().data());
                }else{
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i,44,"Bootloader不一致错误");
                    sheetread->writeStr(firstRow+i,42,soft.BootloaderVer.toLocal8Bit().data());
                    sheetread->writeStr(firstRow+i,43,soft.BootloaderPartNo.toLocal8Bit().data());
                    SetPatternForegroundColor(sheetread,firstRow+i,44,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,45,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,46,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,47,COLOR_RED);
                    SetPatternForegroundColor(sheetread,firstRow+i,48,COLOR_RED);
                }
            }
            if(!flag){
                if(i==0){
                    sheetread->writeStr(firstRow+i,4,soft.ApplicationPartNo.toLocal8Bit().data());
                }else{
                    if(soft.ApplicationPartNo==softNumberTable->value(i-1).ApplicationPartNo){
                        sheetread->writeStr(firstRow+i,4,QString("変更無し").toLocal8Bit().data());
                    }else{
                        sheetread->writeStr(firstRow+i,4,soft.ApplicationPartNo.toLocal8Bit().data());
                    }
                    if(soft.Productionstage=="AKM対応")
                    {
                        if((soft.ApplicationVer.left(1)=="A"&&softNumberTable->value(i-1).ApplicationVer.left(1)!="0")||(soft.ApplicationVer.left(1)=="B"&&softNumberTable->value(i-1).ApplicationVer.left(1)!="1")){
                            CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i,6,"Application不一致错误");
                            SetPatternForegroundColor(sheetread,firstRow+i,6,COLOR_RED);
                            SetPatternForegroundColor(sheetread,firstRow+i,7,COLOR_RED);
                            SetPatternForegroundColor(sheetread,firstRow+i,9,COLOR_RED);
                        }
                        sheetread->writeStr(firstRow+i,6,soft.Productionstage.toLocal8Bit().data());
                        /*
                        Vehicletype=QString("旭化成工場火災の影響でvideo encoderが、\n")+ \
                                QString("AK8817VQからML86640TBZ0AXに変更されるため、\n")+ \
                                QString("対応するソフトウエアを変更\n")+ \
                                QString(" ・エンコーダ用I2Cスレーブアドレス設定変更\n")+ \
                                QString(" ・エンコーダのレジスタ設定変更\n")+ \
                                QString(" ・エンコーダー用ポート設定変更");
                        sheetread->writeStr(firstRow+i,7,Vehicletype.toLocal8Bit().data());
                        */
                        if(IDType=="EntryAVM"||IDType=="EntryAVM2"){
                            Vehicletype=QString("Changed paths:\n")+ \
                                    QString("   Application/view/src/vi_app_i2c.c\n")+ \
                                    QString("   Application/view/src/vi_main.c\n")+ \
                                    QString("   common/include/CL_Ver_ex.h\n")+ \
                                    QString("   common/include/gpiow_wrapper_ex.h\n")+ \
                                    QString("   common/include/i2cw_wrapper_ex.h\n")+ \
                                    QString("   common/include/vi_common_ex.h\n")+ \
                                    QString("   LMiddle/i2c_wrapper/src/i2cw_wrapper.c");
                            sheetread->writeStr(firstRow+i,9,Vehicletype.toLocal8Bit().data());
                        }else if(IDType=="NextPh3"||IDType=="EntryIPA"){
                            Vehicletype=QString("Changed paths:\n")+ \
                                    QString("   Application/view/src/vi_app_i2c.c\n")+ \
                                    QString("   Application/view/src/vi_main.c\n")+ \
                                    QString("   common/include/CL_Ver_ex.h\n")+ \
                                    QString("   common/include/i2cw_wrapper_ex.h\n")+ \
                                    QString("   common/include/vi_common_ex.h\n")+ \
                                    QString("   LMiddle/i2c_wrapper/src/i2cw_wrapper.c");
                            sheetread->writeStr(firstRow+i,9,Vehicletype.toLocal8Bit().data());
                        }
                    }else{
                        CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i,6,"既不是母体机种又不是AKM対応,需要添加说明");
                        SetPatternForegroundColor(sheetread,firstRow+i,6,COLOR_RED);
                        SetPatternForegroundColor(sheetread,firstRow+i,7,COLOR_RED);
                        SetPatternForegroundColor(sheetread,firstRow+i,9,COLOR_RED);
                        SetPatternForegroundColor(sheetread,firstRow+i,12,COLOR_RED);
                        SetPatternForegroundColor(sheetread,firstRow+i,13,COLOR_RED);
                    }
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i*2,10,"需要手动连接图标");
                    SetPatternForegroundColor(sheetread,firstRow+i,10,COLOR_RED);
                    CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i*2,11,"需要手动连接图标");
                    SetPatternForegroundColor(sheetread,firstRow+i,11,COLOR_RED);
                }
                sheetread->writeStr(firstRow+i,5,DefineConfigList.value(i).toLocal8Bit().data());
            }else{
                //需要写入共同机种
                CommonMethod::SetErrorTable(errTableList,filePath,codec->toUnicode(QByteArray(sheetread->name())),firstRow+i*2,4,"覆盖图标需要手动更改依赖机种");
            }
        }
        book->save(filePath.toLocal8Bit());
        book->release();
    }else{
        CommonMethod::SetErrorTable(errTableList,filePath,"",0,0,filePath+"文件不存在!");
        return;
    }
    return;
}

/**
 * @def 设置单元格背景颜色
 * @brief ExcelOperation::SetPatternForegroundColor
 * @param sheetread
 * @param row
 * @param col
 * @param color
 */
void ExcelOperation::SetPatternForegroundColor(Sheet *sheetread,const int row, const int col, const Color color)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->SetPatternForegroundColor() 函数执行!");
    //QLogHelper::instance()->LogDebug("行: "+QString::number(row)+"  列:  "+QString::number(col));
    Format *format;
    if(sheetread==NULL){return;}
    format=book->addFormat(sheetread->cellFormat(row,col));
    format->setFillPattern(FILLPATTERN_SOLID);
    format->setPatternForegroundColor(color);
    sheetread->setCellFormat(row,col,format);
}

