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
 * @param softNumberTable
 * @return
 */
bool ExcelOperation::EEFileWrite(const QString filePath, const QString ID,const QString IDType, QList<SOFTNUMBERTable> *softNumberTable)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->EEFileWrite() 函数执行!");
    Sheet *sheetread;
    QString tmp;
    QDateTime curDateTime=QDateTime::currentDateTime();
    int Years=curDateTime.toString("yyyy").toInt();
    int Mounth=curDateTime.toString("M").toInt();
    int Day=curDateTime.toString("d").toInt();
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        QLogHelper::instance()->LogDebug("-------------------------------");
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
        QLogHelper::instance()->LogDebug(sheetread->readStr(40,27));
        QLogHelper::instance()->LogDebug(sheetread->readStr(40,28));
        QLogHelper::instance()->LogDebug(sheetread->readStr(40,29));
        sheetread->writeStr(7,6,ID.left(7).mid(2).toLatin1().data());
        sheetread->writeStr(7,9,ID.right(1).toLatin1().data());
        sheetread->writeStr(15,21,ID.left(7).mid(2).toLatin1().data());
        sheetread->writeStr(15,24,ID.right(1).toLatin1().data());
        if(softNumberTable->size()>0){
            sheetread->writeStr(8,17,softNumberTable->value(0).CarModels.toLatin1().data());
            tmp=softNumberTable->value(softNumberTable->size()-1).ApplicationVer;
            sheetread->writeStr(10,23,(tmp.left(2)+"."+tmp.left(4).mid(2)+"."+tmp.right(2)).toLatin1().data());
        }
        if(IDType!="NextPh3")
        {

        }else{

        }
    }
    book->save(filePath.toLocal8Bit());
    book->release();
}
/**
 * @def P02F-PRC 5R00A 確認シート.xlsx 文件写入
 * @brief ExcelOperation::ReadyFileWrite
 * @param filePath
 * @param ID
 * @param softNumberTable
 * @param configTable
 * @return
 */
bool ExcelOperation::ReadyFileWrite(const QString filePath, const QString ID,const QString IDType, QList<SOFTNUMBERTable> *softNumberTable, QList<CONFIGTable> *configTable)
{
    QLogHelper::instance()->LogInfo("ExcelOperation->ReadyFileWrite() 函数执行!");
}

