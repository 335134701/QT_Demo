#ifndef EXCELOPERATION_H
#define EXCELOPERATION_H

#include <QObject>
#include <QDateTime>
#include <QTextCodec>
#include "libxl.h"
#include "QLoghelper.h"
#include "DEFINE.h"

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

typedef struct ConfigTable{
    QString Vehicletype;            //車種 Vehicle
    QString CANGen;                 //CAN世代
    QString ITS;                    //ITS
    QString PickMethod;             //接続先・方式
    QString Destination;            //仕向け
    QString Wheelspeedpulse;        //車輪速パルス
    QString CANspecifications;      //CAN仕様
    QString Camerasystem;           //カメラシステム
    QString WAS;                    //4WAS有無
    QString Steeringgearratio;      //ステアリングギア比
    QString VCANsonar;              //V-CANソナー有無
    QString Sonarinterrupt;         //ソナー割り込み表示機能有無
    QString Expectedadroute;        //予想進路線の有無
    QString Steerspecifications;    //左右ステアリング仕様
    QString Mission;                //ミッション
    QString BCIFunction;            //BCI機能有無
    QString RRREBfunction;          //RR　REB機能有無
    QString BCWfunction;            //BCW機能有無
    QString Buzzer;                 //ブザー
    QString MeterSW;                //メーターSW
    QString OFFROADMODE;            //OFF ROAD MODE
    QString Movingway;              //駆動方式
    QString DAS;                    //DAS機能有無
    QString PSRfunction;            //PSR機能有無
    QString Rearnormalview;         //リアノーマルビュー有無
    QString Enginespecifications;   //エンジン仕様
    QString Tiresize;               //タイヤサイズ
    QString Configpartnumber;       //コンフィグ部番
}CONFIGTable;

typedef struct ErrorTable{
    QString fileName;
    QString sheetName;
    unsigned int row;
    unsigned int col;
    QString errMessage;
}ERRORTable;

using namespace libxl;

class ExcelOperation : public QObject
{
    Q_OBJECT
public:
    QTextCodec *codec = QTextCodec::codecForName("GBK") ;

    explicit ExcelOperation(QObject *parent = nullptr);

    bool Init(const QString filePath);

    QList<SOFTNUMBERTable> ReadSoftExcel(const QString filePath,const QString ID,const QString IDType);

    QList<CONFIGTable> ReadConfExcel(const QString filePath,const QString ID);

    bool EEFileWrite(const QString filePath,const QString ID,const QString IDType,const QString RelyID, QList<SOFTNUMBERTable> *softNumberTable);

    bool ReadyFileWrite(const QString filePath,QList<SOFTNUMBERTable> *softNumberTable,QList<CONFIGTable> *configTable,QStringList DefineConfigList,const QString RelyID);
signals:

public slots:

private :
   Book *book;

   bool ReadyFileFirstSheet(const QString filePath, QList<SOFTNUMBERTable> *softNumberTable,QList<CONFIGTable> *configTable,const QString RelyID);
   bool ReadyFileSecondSheet(const QString filePath,QList<CONFIGTable> *configTable);
   bool ReadyFileThirdSheet(const QString filePath,QList<SOFTNUMBERTable> *softNumberTable,QStringList DefineConfigList,bool flag);
};

#endif // EXCELOPERATION_H
