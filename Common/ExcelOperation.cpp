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
    QList<SOFTNUMBERTable> *softlist=new QList<SOFTNUMBERTable>();
    QString CarModels,PartNumber,CANGen,Productionstage;
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
                if(Productionstage.isEmpty()){Productionstage=QString(sheetread->readStr(i,5));}
                if((sheetread->cellFormat(i,5)->patternForegroundColor()==13||sheetread->cellFormat(i,5)->patternForegroundColor()==51))
                {
                    SOFTNUMBERTable soft;
                    soft.ModelNumber=ID;
                    soft.CarModels=CarModels;
                    soft.PartNumber=PartNumber;
                    soft.CANGen=CANGen;
                    soft.Productionstage=Productionstage;
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
                    QByteArray byte(sheetread->readStr(i,23));
                    soft.DiagnosticCode = codec->toUnicode(byte);
                    softlist->append(soft);
                }
            }
        }
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
QList<CONFIGTable> ExcelOperation::ReadConfExcel(const QString filePath, const QString ID, const QString IDType)
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
            if(QString::fromLocal8Bit(sheetread->readStr(i,1)).contains(ID)){
                flag=true;
            }else{flag=false;}
            if(flag){
                CONFIGTable conf;
                byte.append(sheetread->readStr(i,4));
                conf.Vehicletype=codec->toUnicode(byte);            //車種 Vehicle
                byte.clear();
                byte.append(sheetread->readStr(i,10));
                conf.ITS=codec->toUnicode(byte);                    //ITS
                byte.clear();
                byte.append(sheetread->readStr(i,12));
                conf.PickMethod=codec->toUnicode(byte);              //接続先・方式
                byte.clear();
                byte.append(sheetread->readStr(i,14));
                conf.Destination=codec->toUnicode(byte);             //仕向け
                byte.clear();
                byte.append(sheetread->readStr(i,16));
                conf.Wheelspeedpulse=codec->toUnicode(byte);         //車輪速パルス
                byte.clear();
                byte.append(sheetread->readStr(i,18));
                conf.CANspecifications=codec->toUnicode(byte);       //CAN仕様
                byte.clear();
                byte.append(sheetread->readStr(i,20));
                conf.Camerasystem=codec->toUnicode(byte);            //カメラシステム
                byte.clear();
                byte.append(sheetread->readStr(i,22));
                conf.WAS=codec->toUnicode(byte);                     //4WAS有無
                byte.clear();
                byte.append(sheetread->readStr(i,24));
                conf.Steeringgearratio=codec->toUnicode(byte);       //ステアリングギア比
                byte.clear();
                byte.append(sheetread->readStr(i,26));
                conf.VCANsonar=codec->toUnicode(byte);              //V-CANソナー有無
                byte.clear();
                byte.append(sheetread->readStr(i,28));
                conf.Sonarinterrupt=codec->toUnicode(byte);          //ソナー割り込み表示機能有無
                byte.clear();
                byte.append(sheetread->readStr(i,30));
                conf.Expectedadroute=codec->toUnicode(byte);         //予想進路線の有無
                byte.clear();
                byte.append(sheetread->readStr(i,32));
                conf.Steerspecifications=codec->toUnicode(byte);     //左右ステアリング仕様
                byte.clear();
                byte.append(sheetread->readStr(i,34));
                conf.Mission=codec->toUnicode(byte);                 //ミッション
                byte.clear();
                byte.append(sheetread->readStr(i,46));
                conf.RRREBfunction=codec->toUnicode(byte);           //RR　REB機能有無
                byte.clear();
                byte.append(sheetread->readStr(i,52));
                conf.BCWfunction=codec->toUnicode(byte);             //BCW機能有無
                byte.clear();
                byte.append(sheetread->readStr(i,54));
                conf.Buzzer=codec->toUnicode(byte);                  //ブザー
                byte.clear();
                byte.append(sheetread->readStr(i,58));
                conf.MeterSW=codec->toUnicode(byte);                //メーターSW
                byte.clear();
                byte.append(sheetread->readStr(i,64));
                conf.OFFROADMODE=codec->toUnicode(byte);             //OFF ROAD MODE
                byte.clear();
                byte.append(sheetread->readStr(i,68));
                conf.Movingway=codec->toUnicode(byte);               //駆動方式
                byte.clear();
                byte.append(sheetread->readStr(i,74));
                conf.PSRfunction=codec->toUnicode(byte);             //PSR機能有無
                byte.clear();
                byte.append(sheetread->readStr(i,76));
                conf.Rearnormalview=codec->toUnicode(byte);          //リアノーマルビュー有無
                byte.clear();
                byte.append(sheetread->readStr(i,84));
                conf.Enginespecifications=codec->toUnicode(byte);    //エンジン仕様
                byte.clear();
                byte.append(sheetread->readStr(i,86));
                conf.Tiresize=codec->toUnicode(byte);                //タイヤサイズ
                byte.clear();
                byte.append(sheetread->readStr(i,90));
                conf.Configpartnumber=codec->toUnicode(byte);        //コンフィグ部番
                byte.clear();
                conflist->append(conf);
            }
        }
    }
    return *conflist;
}

