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
    SOFTNUMBERTable soft;
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
                    soft.DiagnosticCode=QString(sheetread->readStr(i,23));
                    QLogHelper::instance()->LogDebug(soft.DiagnosticCode);
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
    QList<CONFIGTable> *conflist=new QList<CONFIGTable>();
    CONFIGTable conf;
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
                /*
                conf.Vehicletype=sheetread->readStr(i,4);            //車種 Vehicle
                conf.ITS=sheetread->readStr(i,10);                    //ITS
                conf.PickMethod=sheetread->readStr(i,12);;             //接続先・方式
                conf.Destination=sheetread->readStr(i,14);;            //仕向け
                conf.Wheelspeedpulse=sheetread->readStr(i,16);;        //車輪速パルス
                conf.CANspecifications=sheetread->readStr(i,18);;      //CAN仕様
                conf.Camerasystem=sheetread->readStr(i,20);;           //カメラシステム
                conf.WAS=sheetread->readStr(i,22);;                    //4WAS有無
                conf.Steeringgearratio=sheetread->readStr(i,24);;      //ステアリングギア比
                conf.VCANsonar=sheetread->readStr(i,26);;              //V-CANソナー有無
                conf.Sonarinterrupt=sheetread->readStr(i,28);;         //ソナー割り込み表示機能有無
                conf.Expectedadroute=sheetread->readStr(i,1);;        //予想進路線の有無
                conf.Steerspecifications=sheetread->readStr(i,1);;    //左右ステアリング仕様
                conf.Mission=sheetread->readStr(i,1);;                //ミッション
                conf.RRREBfunction=sheetread->readStr(i,1);;          //RR　REB機能有無
                conf.BCWfunction=sheetread->readStr(i,1);;            //BCW機能有無
                conf.Buzzer=sheetread->readStr(i,1);;                 //ブザー
                conf.MeterSW=sheetread->readStr(i,1);;                //メーターSW
                conf.OFFROADMODE=sheetread->readStr(i,1);;            //OFF ROAD MODE
                conf.Movingway=sheetread->readStr(i,1);;              //駆動方式
                conf.PSRfunction=sheetread->readStr(i,1);;            //PSR機能有無
                conf.Rearnormalview=sheetread->readStr(i,1);;         //リアノーマルビュー有無
                conf.Enginespecifications=sheetread->readStr(i,1);;   //エンジン仕様
                conf.Tiresize=sheetread->readStr(i,1);;               //タイヤサイズ
                conf.Configpartnumber=sheetread->readStr(i,1);;       //コンフィグ部番
                conflist->append(conf)=sheetread->readStr(i,1);;
                */
            }
        }
    }
    return *conflist;
}

