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
 * @param errTable
 * @return
 */
bool ExcelOperation::EEFileWrite(const QString filePath, const QString ID,const QString IDType,const QString RelyID, QList<SOFTNUMBERTable> *softNumberTable,QList<ErrorTable> *errTable)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->EEFileWrite() 函数执行!");
    Sheet *sheetread;
    QDateTime curDateTime=QDateTime::currentDateTime();
    int Years=curDateTime.toString("yyyy").toInt();
    int Mounth=curDateTime.toString("M").toInt();
    int Day=curDateTime.toString("d").toInt();
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        sheetread = book->getSheet(0);
        sheetread->writeNum(3,4,Years);
        sheetread->writeNum(11,5,Years);
        sheetread->writeNum(12,5,Years);
        sheetread->writeNum(3,6,Mounth);
        sheetread->writeNum(11,7,Mounth);
        sheetread->writeNum(12,7,Mounth);
        sheetread->writeNum(3,9,Day);
        sheetread->writeNum(11,9,Day);
        sheetread->writeNum(12,9,Day);
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
                sheetread->writeStr(43,29,  QString("Config情報　：　\\claris-si\\P_CAMERA_NISSAN_SH7766_PF\09_次世代PH3$01_project\11_車両日程_車両展開\01_工場コンフィグ").toLocal8Bit().data());
            }
        }
    }
    book->save(filePath.toLocal8Bit());
    book->release();
    return true;
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
bool ExcelOperation::ReadyFileWrite(const QString filePath, QList<SOFTNUMBERTable> *softNumberTable, QList<CONFIGTable> *configTable,QStringList DefineConfigList,const QString RelyID,QList<ErrorTable> *errTable)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadyFileWrite() 函数执行!");
    //ReadyFileFirstSheet(filePath,softNumberTable,configTable,RelyID,errTable);
    //ReadyFileSecondSheet(filePath,configTable,errTable);
    ReadyFileThirdSheet(filePath,softNumberTable,DefineConfigList,false,errTable);
    return true;
}
/**
 * @def 確認シート.xlsx 表第一个sheet修改
 * @brief ExcelOperation::ReadyFileFirstSheet
 * @param filePath
 * @param softNumberTable
 * @param configTable
 * @param RelyID
 * @param errTable
 * @return
 */
bool ExcelOperation::ReadyFileFirstSheet(const QString filePath, QList<SOFTNUMBERTable> *softNumberTable, QList<CONFIGTable> *configTable,const QString RelyID,QList<ErrorTable> *errTable)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadyFileFirstSheet() 函数执行!");
    Sheet *sheetread;
    int firstCol=7;
    QString ConfigpartnumberList;
    ERRORTable *err=new ErrorTable();
    int i=0;
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        sheetread=book->getSheet(0);
        //最少两种状态，如果有更多状态，可以进一步处理
        if(sheetread->lastRow()<11||softNumberTable->size()<0){
            err=new ErrorTable();
            err->fileName=filePath.mid(filePath.lastIndexOf("/")+1);
            err->errMessage="模板格式错误!";
            errTable->append(*err);
            return false;
        }
        for(i=0;i<configTable->size();i++){
            ConfigpartnumberList.append(configTable->value(i).Configpartnumber+"\n");
        }
        for(i=0;i<softNumberTable->size();i++){
            SOFTNUMBERTable soft=softNumberTable->value(i);
            sheetread->writeStr(firstCol+i*2,1,soft.CarModels.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,2,soft.PartNumber.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,3,soft.Productionstage.toLocal8Bit().data());
            if(!ConfigpartnumberList.isEmpty()){
                if(configTable->size()<5)
                {
                    Format* companyFormat=sheetread->cellFormat(firstCol+i*2,4);
                    companyFormat->font()->setSize(11);
                    sheetread->writeStr(firstCol+i*2,4,ConfigpartnumberList.toLocal8Bit().data(),companyFormat);
                }else{
                    sheetread->writeStr(firstCol+i*2,4,ConfigpartnumberList.toLocal8Bit().data());
                }
            }else{
                sheetread->cellFormat(firstCol+i*2,4)->setPatternForegroundColor(COLOR_RED);
            }
            sheetread->writeStr(firstCol+i*2,5,soft.DiagnosticCode.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,6,soft.ApplicationPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,7,soft.ApplicationVer.toLocal8Bit().data());
            if(soft.Productionstage=="AKM対応"){
                sheetread->writeStr(firstCol+i*2,8,QString("AKM対応").toLocal8Bit().data());
            }else{
                if(!RelyID.isEmpty()){
                    sheetread->writeStr(firstCol+i*2,8,(RelyID+"と共通").toLocal8Bit().data());
                }else{
                    sheetread->writeStr(firstCol+i*2,8,QString("母体").toLocal8Bit().data());
                }
            }
            sheetread->writeStr(firstCol+i*2,9,soft.CarInfoPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,10,soft.CarInfoVer.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,12,soft.CameraMAPPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,13,soft.CameraMAPVer.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,15,soft.OSDPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,16,soft.OSDVer.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,18,soft.CANfblPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,19,soft.CANfblVer.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,21,soft.BootloaderPartNo.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i*2,22,soft.BootloaderVer.toLocal8Bit().data());
            if(i>0)
            {
                if(soft.ApplicationPartNo!=softNumberTable->value(i-1).ApplicationPartNo)
                {
                    sheetread->cellFormat(firstCol+i*2,8)->setPatternForegroundColor(COLOR_RED);
                }
                if((soft.CarInfoPartNo!=softNumberTable->value(i-1).CarInfoPartNo)||(soft.CarInfoVer!=softNumberTable->value(i-1).CarInfoVer))
                {
                    sheetread->cellFormat(firstCol+i*2,11)->setPatternForegroundColor(COLOR_RED);
                }
                if((soft.CameraMAPPartNo!=softNumberTable->value(i-1).CameraMAPPartNo)||(soft.CameraMAPVer!=softNumberTable->value(i-1).CameraMAPVer))
                {
                    sheetread->cellFormat(firstCol+i*2,14)->setPatternForegroundColor(COLOR_RED);
                }
                if((soft.OSDPartNo!=softNumberTable->value(i-1).OSDPartNo)||(soft.OSDVer!=softNumberTable->value(i-1).OSDVer))
                {
                    sheetread->cellFormat(firstCol+i*2,17)->setPatternForegroundColor(COLOR_RED);
                }
                if((soft.CANfblPartNo!=softNumberTable->value(i-1).CANfblPartNo)||(soft.CANfblVer!=softNumberTable->value(i-1).CANfblVer))
                {
                    sheetread->cellFormat(firstCol+i*2,20)->setPatternForegroundColor(COLOR_RED);
                }
                if((soft.BootloaderPartNo!=softNumberTable->value(i-1).BootloaderPartNo)||(soft.BootloaderVer!=softNumberTable->value(i-1).BootloaderVer))
                {
                    sheetread->cellFormat(firstCol+i*2,23)->setPatternForegroundColor(COLOR_RED);
                }
            }
        }
        book->save(filePath.toLocal8Bit());
        book->release();
    }else{
        err=new ErrorTable();
        err->fileName=filePath.mid(filePath.lastIndexOf("/")+1);
        err->errMessage="模板格式错误!";
        errTable->append(*err);
        return false;
    }
    return true;
}
/**
 * @def 確認シート.xlsx 表第二个sheet修改
 * @brief ExcelOperation::ReadyFileSecondSheet
 * @param filePath
 * @param configTable
 * @param errTable
 * @return
 */
bool ExcelOperation::ReadyFileSecondSheet(const QString filePath, QList<CONFIGTable> *configTable,QList<ErrorTable> *errTable)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadyFileSecondSheet() 函数执行!");
    int firstCol=12,i=0,row=0;
    QString ConfigpartnumberList;
    Sheet *sheetread;
    ERRORTable *err=new ErrorTable();
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        sheetread=book->getSheet(1);
        for(i=0;i<configTable->size();i++){
            ConfigpartnumberList.append(configTable->value(i).Configpartnumber+"/");
        }
        ConfigpartnumberList=ConfigpartnumberList.left(ConfigpartnumberList.lastIndexOf("/"));
        sheetread->writeStr(1,11,ConfigpartnumberList.toLocal8Bit().data());
        row=sheetread->lastRow();
        //模板文件有问题，直接返回
        if(row<=12){
            err=new ErrorTable();
            err->fileName=filePath.mid(filePath.lastIndexOf("/")+1);
            err->errMessage="模板格式错误!";
            errTable->append(*err);
            return false;
        }
        //如果解析出来无数据
        if(configTable->size()==0){
            sheetread->removeRow(12,sheetread->lastRow());
            return true;
        }
        //如果模板列多余需要的列，则删除多余的模板列
        if(configTable->size()+12<row)
        {
            sheetread->removeRow(configTable->size()+12,sheetread->lastRow());
        }
        //如果模板列少于需要的列，则需要复制单元格式
        if(configTable->size()+12>sheetread->lastRow())
        {
            Format * format;
            for(i=0;i<(configTable->size()+12)-sheetread->lastRow();i++)
            {
                 //QLogHelper::instance()->LogDebug("复制单元格："+QString::number(i));
                 for(int j=0;j<sheetread->lastCol();j++)
                 {
                     format=sheetread->cellFormat(row-1,j);
                     sheetread->writeStr(row+i,j,sheetread->readStr(row-1,j),format);
                 }
            }
        }
        for(i=0;i<configTable->size();i++){
            CONFIGTable conf=configTable->value(i);
            sheetread->writeStr(firstCol+i,2,(conf.Vehicletype.left(conf.Vehicletype.indexOf("("))).toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,3,conf.ITS.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,5,conf.PickMethod.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,7,conf.Destination.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,9,conf.Wheelspeedpulse.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,11,conf.CANspecifications.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,13,conf.Camerasystem.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,15,conf.WAS.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,17,conf.Steeringgearratio.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,19,conf.VCANsonar.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,21,conf.Sonarinterrupt.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,23,conf.Expectedadroute.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,25,conf.Steerspecifications.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,27,conf.Mission.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,37,conf.BCIFunction.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,39,conf.RRREBfunction.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,45,conf.BCWfunction.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,47,conf.Buzzer.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,51,conf.MeterSW.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,57,conf.OFFROADMODE.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,59,conf.Movingway.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,61,conf.DAS.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,63,conf.PSRfunction.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,65,conf.Rearnormalview.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,69,conf.Enginespecifications.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,71,conf.Tiresize.toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,73,(conf.CANGen+"世代CAN対応").toLocal8Bit().data());
            sheetread->writeStr(firstCol+i,74,conf.Configpartnumber.toLocal8Bit().data());
        }
        book->save(filePath.toLocal8Bit());
        book->release();
    }else{
        err=new ErrorTable();
        err->fileName=filePath.mid(filePath.lastIndexOf("/")+1);
        err->errMessage="模板格式错误!";
        errTable->append(*err);
        return false;
    }
    return true;
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
/*
typedef struct SoftNumberTable{
    QString ModelNumber;            //クラリオン機種番号
    QString CarModels;              //車種仕向け
    QString PartNumber;             //日産部番
    QString CANGen;                 //CAN世代
    QString Productionstage;        //生産段階
    QString ApplicationPartNo;      //Application PartsNo
    QString ApplicationVer;         //Application Ver
    QString CarInfoPartNo;          //Car_Info PartsNo
    QString CarInfoVer;             //Car_Info Ver
    QString CameraMAPPartNo;        //CameraMAP PartsNo
    QString CameraMAPVer;           //CameraMAP Ver
    QString OSDPartNo;              //OSD PartsNo
    QString OSDVer;                 //OSD Ver
    QString CANfblPartNo;           //CANfbl PartsNo
    QString CANfblVer;              //CANfbl Ver
    QString BootloaderPartNo;       //Bootloader PartsNo
    QString BootloaderVer;          //Bootloader Ver
    QString DiagnosticCode;         //診断識別コード
}SOFTNUMBERTable;
*/
bool ExcelOperation::ReadyFileThirdSheet(const QString filePath,QList<SOFTNUMBERTable> *softNumberTable,QStringList DefineConfigList,bool flag,QList<ErrorTable> *errTable)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadyFileThirdSheet() 函数执行!");
    Sheet *sheetread;
    ERRORTable *err=new ErrorTable();
    int i=0;
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        sheetread=book->getSheet(2);
        if(softNumberTable->size()<0){return false;}
        QString Vehicletype=softNumberTable->value(0).CarModels.left(softNumberTable->value(0).CarModels.indexOf("_"));
        sheetread->writeStr(1,2,Vehicletype.toLatin1().data());
        sheetread->writeStr(1,15,Vehicletype.toLatin1().data());
        sheetread->writeStr(1,16,Vehicletype.toLatin1().data());
        sheetread->writeStr(1,24,Vehicletype.toLatin1().data());
        sheetread->writeStr(1,32,Vehicletype.toLatin1().data());
        sheetread->writeStr(1,40,Vehicletype.toLatin1().data());
        sheetread->writeStr(3,1,Vehicletype.toLatin1().data());
 /*       for(i=0;i<softNumberTable->size();i++)
        {
            if(!flag){


            }
        }
        */
        book->save(filePath.toLocal8Bit());
        book->release();
    }else{
        err=new ErrorTable();
        err->fileName=filePath.mid(filePath.lastIndexOf("/")+1);
        err->errMessage="模板格式错误!";
        errTable->append(*err);
        return false;
    }
    return true;
}

