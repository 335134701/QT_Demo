#include "UIMethod.h"


UIMethod::UIMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("UIMethod() 构造函数执行!");
    this->Init();
}
/**
 * @brief UIMethod::Init
 */
void UIMethod::Init()
{
    carTmpPath="";
    carTmpPathflag=false;
    errList=new QList<ERRORTable>();
    fileThread=new FileThread();
    dealFileFileThread=new QThread();
    fileThread->moveToThread(dealFileFileThread);
    excelOperateThread=new ExcelOperateThread();
    excelThread=new QThread();
    excelOperateThread->moveToThread(excelThread);

    //不同线程之间通过信号和槽来传递自定义数据类型QList<SOFTNUMBERTable>的时候
    qRegisterMetaType<QList<SOFTNUMBERTable>>("QList<SOFTNUMBERTable>");//注册SOFTNUMBERTable类型
    //不同线程之间通过信号和槽来传递自定义数据类型QList<CONFIGTable>的时候
    qRegisterMetaType<QList<CONFIGTable>>("QList<CONFIGTable>");//注册CONFIGTable类型

    connect(this,&UIMethod::ActiveThreadSignal,this,&UIMethod::SelectFileSlot);
    //激活文件查找线程，以信号槽的方式
    connect(this,&UIMethod::FindFileThreadSignal,fileThread,&FileThread::FindFileThreadSlot);
    //线程处理完，返回主函数，以信号槽方式
    connect(fileThread,&FileThread::EndFindFileThreadSignal,this,&UIMethod::EndFindFileThreadSlot);
    //LogView显示
    connect(this,&UIMethod::ShowIDmessageSignal,this,&UIMethod::ShowIDmessageSlot);
    //连接解析excel线程，以信号槽的方式
    connect(this,&UIMethod::ExcelOperateThreadSignal,excelOperateThread,&ExcelOperateThread::ExcelOperateThreadSlot);
    connect(this,&UIMethod::EEExcelWriteSignal,excelOperateThread,&ExcelOperateThread::EEExcelWriteSlot);
    connect(this,&UIMethod::ReadyExcelWriteSignal,excelOperateThread,&ExcelOperateThread::ReadyExcelWriteSlot);
    //解析excel完成后回调函数连接，以信号槽的方式
    connect(excelOperateThread,&ExcelOperateThread::EndExcelOperateThreadSoftSignal,this,&UIMethod::EndExcelOperateThreadSoftSlot);
    connect(excelOperateThread,&ExcelOperateThread::EndExcelOperateThreadConfSignal,this,&UIMethod::EndExcelOperateThreadConfSlot);
    connect(excelOperateThread,&ExcelOperateThread::EndEEExcelWriteSignal,this,&UIMethod::EndEEExcelWriteSlot);
    connect(excelOperateThread,&ExcelOperateThread::EndReadyExcelWriteSignal,this,&UIMethod::EndReadyExcelWriteSlot);

    //连接文件打包信号槽，子线程处理
    connect(this,&UIMethod::FileCompressionSignal,fileThread,&FileThread::FileCompressionSlot);
    connect(fileThread,&FileThread::EndFileCompressionSignal,this,&UIMethod::EndFileCompressionSlot);
}

QTextEdit *UIMethod::getTextEdit() const
{
    return textEdit;
}

void UIMethod::setTextEdit(QTextEdit *value)
{
    textEdit = value;
}

void UIMethod::setComBean(CommonBean *value)
{
    comBean = value;
}

/**
 * @def LogView 界面操作显示
 * @brief UIMethod::ShowIDmessageSlot
 * @param Edit
 * @param flag
 */
void UIMethod::ShowIDmessageSlot(int flag)
{
    bool tmpflag=true;
    if(flag==DefineConfig){
        tmpflag=false;
    }
    if(tmpflag)this->getTextEdit()->append(DATAStytle+"=======================================");
    int i=0,tmp=0;
    switch (flag) {
    case IDflag:
        this->getTextEdit()->append(DATAStytle+"机种番号: "+comBean->getID());
        this->getTextEdit()->append(DATAStytle+"机种类型: "+comBean->getIDType());
        comBean->getMessageViewModel()->setItem(IDflag-1, 1, new QStandardItem(*comBean->getID()));
        comBean->getMessageViewModel()->item(IDflag-1,1)->setEditable(false);
        comBean->getMessageViewModel()->setItem(IDflag, 1, new QStandardItem(*comBean->getIDType()));
        comBean->getMessageViewModel()->item(IDflag,1)->setEditable(false);
        break;
    case RelyIDflag:
        this->getTextEdit()->append(DATAStytle+"依赖机种番号: "+comBean->getRelyID());
        this->getTextEdit()->append(DATAStytle+"依赖机种类型: "+comBean->getRelyIDType());
        comBean->getMessageViewModel()->setItem(RelyIDflag-1, 1, new QStandardItem(*comBean->getRelyID()));
        comBean->getMessageViewModel()->item(RelyIDflag-1,1)->setEditable(false);
        comBean->getMessageViewModel()->setItem(RelyIDflag, 1, new QStandardItem(*comBean->getRelyIDType()));
        comBean->getMessageViewModel()->item(RelyIDflag,1)->setEditable(false);
        break;
    case RelyFileflag:
        this->getTextEdit()->append(DATAStytle+"量产管理表路径: "+comBean->getRelyFilePath());
        comBean->getMessageViewModel()->setItem(RelyFileflag-1, 1, new QStandardItem(*comBean->getRelyFilePath()));
        break;
    case IniFileflag:
        this->getTextEdit()->append(DATAStytle+"ini配置文件路径: "+comBean->getIniFilePath());
        comBean->getMessageViewModel()->setItem(IniFileflag-1, 1, new QStandardItem(*comBean->getIniFilePath()));
        break;
    case PFileflag:
        this->getTextEdit()->append(DATAStytle+"P票文件路径: "+comBean->getPFilePath());
        comBean->getMessageViewModel()->setItem(PFileflag-1, 1, new QStandardItem(*comBean->getPFilePath()));
        break;
    case SWFileflag:
        this->getTextEdit()->append(DATAStytle+"SW確認文件路径: "+comBean->getSWFilePath());
        comBean->getMessageViewModel()->setItem(SWFileflag-1, 1, new QStandardItem(*comBean->getSWFilePath()));
        break;
    case CarInfoFileflag:
        this->getTextEdit()->append(DATAStytle+"CarInfo文件路径: "+comBean->getCarInfoFilePath());
        comBean->getMessageViewModel()->setItem(CarInfoFileflag-1, 1, new QStandardItem(*comBean->getCarInfoFilePath()));
        break;
    case CarMapFileflag:
        this->getTextEdit()->append(DATAStytle+"CarMAP文件路径: "+comBean->getCarMapFilePath());
        comBean->getMessageViewModel()->setItem(CarMapFileflag-1, 1, new QStandardItem(*comBean->getCarMapFilePath()));
        break;
    case CarOSDFileflag:
        this->getTextEdit()->append(DATAStytle+"OSD文件路径: "+comBean->getCarOSDFilePath());
        comBean->getMessageViewModel()->setItem(CarOSDFileflag-1, 1, new QStandardItem(*comBean->getCarOSDFilePath()));
        break;
    case APPFileflag:
        this->getTextEdit()->append(DATAStytle+"APP Mot文件路径: "+comBean->getAPPMot());
        comBean->getMessageViewModel()->setItem(APPFileflag-1, 1, new QStandardItem(*comBean->getAPPMot()));
        break;
    case JoinFileflag:
        this->getTextEdit()->append(DATAStytle+"join Mot文件路径: "+comBean->getJoinMot());
        comBean->getMessageViewModel()->setItem(JoinFileflag-1, 1, new QStandardItem(*comBean->getJoinMot()));
        break;
    case EEFileflag:
        this->getTextEdit()->append(DATAStytle+"EE-A002-1000 DR会議運用手順文件路径: "+comBean->getEEFilePath());
        comBean->getMessageViewModel()->setItem(EEFileflag-1, 1, new QStandardItem(*comBean->getEEFilePath()));
        break;
    case ReadyFileflag:
        this->getTextEdit()->append(DATAStytle+"確認シート文件路径: "+comBean->getReadyFilePath());
        comBean->getMessageViewModel()->setItem(ReadyFileflag-1, 1, new QStandardItem(*comBean->getReadyFilePath()));
        break;
    case ConfigFileflag:
        this->getTextEdit()->append(DATAStytle+"EntryAVM採用車種コンフィグ詳細文件路径: "+comBean->getConfigFilePath());
        comBean->getMessageViewModel()->setItem(ConfigFileflag-1, 1, new QStandardItem(*comBean->getConfigFilePath()));
        break;
    case DefineConfig:
        foreach (SOFTNUMBERTable table, *(comBean->getSoftNumberTable())) {
            comBean->getMessageViewModel()->setItem(DefineConfig-1+i, 0, new QStandardItem(table.Productionstage+" 宏定义:"));
            comBean->getMessageViewModel()->item(DefineConfig-1+i,0)->setTextAlignment(Qt::AlignRight);
            i++;
        }
        break;
    case RelyMessageflag:
        tmp=DefineConfig-1+comBean->getSoftNumberTable()->size();
        foreach (SOFTNUMBERTable table, *(comBean->getSoftNumberTable())) {
            comBean->getMessageViewModel()->setItem(tmp+i*18+0, 0, new QStandardItem("クラリオン機種番号:"));
            comBean->getMessageViewModel()->item(tmp+i*18+0,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->item(tmp+i*18+0,0)->setEditable(false);
            comBean->getMessageViewModel()->setItem(tmp+i*18+0, 1, new QStandardItem(table.ModelNumber));
            comBean->getMessageViewModel()->item(tmp+i*18+0, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"クラリオン機種番号: "+table.ModelNumber);
            comBean->getMessageViewModel()->setItem(tmp+i*18+1, 0, new QStandardItem("車種仕向け:"));
            comBean->getMessageViewModel()->item(tmp+i*18+1,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+1,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+1, 1, new QStandardItem(table.CarModels));
            comBean->getMessageViewModel()->item(tmp+i*18+1, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"車種仕向け: "+table.CarModels);
            comBean->getMessageViewModel()->setItem(tmp+i*18+2, 0, new QStandardItem("日産部番:"));
            comBean->getMessageViewModel()->item(tmp+i*18+2,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+2,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+2, 1, new QStandardItem(table.PartNumber));
            comBean->getMessageViewModel()->item(tmp+i*18+2, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"日産部番: "+table.PartNumber);
            comBean->getMessageViewModel()->setItem(tmp+i*18+3, 0, new QStandardItem("CAN世代:"));
            comBean->getMessageViewModel()->item(tmp+i*18+3,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+3,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+3, 1, new QStandardItem(table.CANGen));
            comBean->getMessageViewModel()->item(tmp+i*18+3, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"CAN世代: "+table.CANGen);
            comBean->getMessageViewModel()->setItem(tmp+i*18+4, 0, new QStandardItem("生産段階:"));
            comBean->getMessageViewModel()->item(tmp+i*18+4,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+4,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+4, 1, new QStandardItem(table.Productionstage));
            comBean->getMessageViewModel()->item(tmp+i*18+4, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"生産段階: "+table.Productionstage);
            comBean->getMessageViewModel()->setItem(tmp+i*18+5, 0, new QStandardItem("Application PartsNo:"));
            comBean->getMessageViewModel()->item(tmp+i*18+5,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+5,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+5, 1, new QStandardItem(table.ApplicationPartNo));
            comBean->getMessageViewModel()->item(tmp+i*18+5, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"Application PartsNo: "+table.ApplicationPartNo);
            comBean->getMessageViewModel()->setItem(tmp+i*18+6, 0, new QStandardItem("Application Ver:"));
            comBean->getMessageViewModel()->item(tmp+i*18+6,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+6,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+6, 1, new QStandardItem(table.ApplicationVer));
            comBean->getMessageViewModel()->item(tmp+i*18+6, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"Application Ver: "+table.ApplicationVer);
            comBean->getMessageViewModel()->setItem(tmp+i*18+7, 0, new QStandardItem("Car_Info PartsNo:"));
            comBean->getMessageViewModel()->item(tmp+i*18+7,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+7,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+7, 1, new QStandardItem(table.CarInfoPartNo));
            comBean->getMessageViewModel()->item(tmp+i*18+7, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"Car_Info PartsNo: "+table.CarInfoPartNo);
            comBean->getMessageViewModel()->setItem(tmp+i*18+8, 0, new QStandardItem("Car_Info Ver:"));
            comBean->getMessageViewModel()->item(tmp+i*18+8,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+8,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+8, 1, new QStandardItem(table.CarInfoVer));
            comBean->getMessageViewModel()->item(tmp+i*18+8, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"Car_Info Ver: "+table.CarInfoVer);
            comBean->getMessageViewModel()->setItem(tmp+i*18+9, 0, new QStandardItem("CameraMAP PartsNo:"));
            comBean->getMessageViewModel()->item(tmp+i*18+9,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+9,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+9, 1, new QStandardItem(table.CameraMAPPartNo));
            comBean->getMessageViewModel()->item(tmp+i*18+9, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"CameraMAP PartsNo: "+table.CameraMAPPartNo);
            comBean->getMessageViewModel()->setItem(tmp+i*18+10, 0, new QStandardItem("CameraMAP Ver:"));
            comBean->getMessageViewModel()->item(tmp+i*18+10,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+10,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+10, 1, new QStandardItem(table.CameraMAPVer));
            comBean->getMessageViewModel()->item(tmp+i*18+10, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"CameraMAP Ver: "+table.CameraMAPVer);
            comBean->getMessageViewModel()->setItem(tmp+i*18+11, 0, new QStandardItem("OSD PartsNo:"));
            comBean->getMessageViewModel()->item(tmp+i*18+11,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+11,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+11, 1, new QStandardItem(table.OSDPartNo));
            comBean->getMessageViewModel()->item(tmp+i*18+11, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"OSD PartsNo: "+table.OSDPartNo);
            comBean->getMessageViewModel()->setItem(tmp+i*18+12, 0, new QStandardItem("OSD Ver:"));
            comBean->getMessageViewModel()->item(tmp+i*18+12,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+12,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+12, 1, new QStandardItem(table.OSDVer));
            comBean->getMessageViewModel()->item(tmp+i*18+12, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"OSD Ver: "+table.OSDVer);
            comBean->getMessageViewModel()->setItem(tmp+i*18+13, 0, new QStandardItem("CANfbl PartsNo:"));
            comBean->getMessageViewModel()->item(tmp+i*18+13,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+13,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+13, 1, new QStandardItem(table.CANfblPartNo));
            comBean->getMessageViewModel()->item(tmp+i*18+13, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"CANfbl PartsNo: "+table.CANfblPartNo);
            comBean->getMessageViewModel()->setItem(tmp+i*18+14, 0, new QStandardItem("CANfbl Ver:"));
            comBean->getMessageViewModel()->item(tmp+i*18+14,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+14,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+14, 1, new QStandardItem(table.CANfblVer));
            comBean->getMessageViewModel()->item(tmp+i*18+14, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"CANfbl Ver: "+table.CANfblVer);
            comBean->getMessageViewModel()->setItem(tmp+i*18+15, 0, new QStandardItem("Bootloader PartsNo:"));
            comBean->getMessageViewModel()->item(tmp+i*18+15,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+15,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+15, 1, new QStandardItem(table.BootloaderPartNo));
            comBean->getMessageViewModel()->item(tmp+i*18+15, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"Bootloader PartsNo: "+table.BootloaderPartNo);
            comBean->getMessageViewModel()->setItem(tmp+i*18+16, 0, new QStandardItem("Bootloader Ver:"));
            comBean->getMessageViewModel()->item(tmp+i*18+16,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+16,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+16, 1, new QStandardItem(table.BootloaderVer));
            comBean->getMessageViewModel()->item(tmp+i*18+16, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"Bootloader Ver: "+table.BootloaderVer);
            comBean->getMessageViewModel()->setItem(tmp+i*18+17, 0, new QStandardItem("診断識別コード:"));
            comBean->getMessageViewModel()->item(tmp+i*18+17,0)->setEditable(false);
            comBean->getMessageViewModel()->item(tmp+i*18+17,0)->setTextAlignment(Qt::AlignRight);
            comBean->getMessageViewModel()->setItem(tmp+i*18+17, 1, new QStandardItem(table.DiagnosticCode));
            comBean->getMessageViewModel()->item(tmp+i*18+17, 1)->setEditable(false);
            this->getTextEdit()->append(DATAStytle+"診断識別コード: "+table.DiagnosticCode);
            if(i!=(comBean->getSoftNumberTable()->size()-1))this->getTextEdit()->append("");
            i++;
        }
        break;
    case ConfigMessageflag:
        foreach (CONFIGTable conf, *(comBean->getConfigTable())) {
            this->getTextEdit()->append(DATAStytle+"車種 Vehicle: "+conf.Vehicletype);
            this->getTextEdit()->append(DATAStytle+"CAN世代: "+conf.CANGen);
            this->getTextEdit()->append(DATAStytle+"ITS: "+conf.ITS);
            this->getTextEdit()->append(DATAStytle+"接続先・方式: "+conf.PickMethod);
            this->getTextEdit()->append(DATAStytle+"仕向け: "+conf.Destination);
            this->getTextEdit()->append(DATAStytle+"車輪速パルス: "+conf.Wheelspeedpulse);
            this->getTextEdit()->append(DATAStytle+"CAN仕様: "+conf.CANspecifications);
            this->getTextEdit()->append(DATAStytle+"カメラシステム: "+conf.Camerasystem);
            this->getTextEdit()->append(DATAStytle+"4WAS有無: "+conf.WAS);
            this->getTextEdit()->append(DATAStytle+"ステアリングギア比: "+conf.Steeringgearratio);
            this->getTextEdit()->append(DATAStytle+"V-CANソナー有無: "+conf.VCANsonar);
            this->getTextEdit()->append(DATAStytle+"ソナー割り込み表示機能有無: "+conf.Sonarinterrupt);
            this->getTextEdit()->append(DATAStytle+"予想進路線の有無: "+conf.Expectedadroute);
            this->getTextEdit()->append(DATAStytle+"左右ステアリング仕様: "+conf.Steerspecifications);
            this->getTextEdit()->append(DATAStytle+"ミッション: "+conf.Mission);
            this->getTextEdit()->append(DATAStytle+"BCI機能有無: "+conf.BCIFunction);
            this->getTextEdit()->append(DATAStytle+"RR　REB機能有無: "+conf.RRREBfunction);
            this->getTextEdit()->append(DATAStytle+"BCW機能有無: "+conf.BCWfunction);
            this->getTextEdit()->append(DATAStytle+"ブザー: "+conf.Buzzer);
            this->getTextEdit()->append(DATAStytle+"メーターSW: "+conf.MeterSW);
            this->getTextEdit()->append(DATAStytle+"OFF ROAD MODE: "+conf.OFFROADMODE);
            this->getTextEdit()->append(DATAStytle+"駆動方式: "+conf.Movingway);
            this->getTextEdit()->append(DATAStytle+"DAS機能有無: "+conf.DAS);
            this->getTextEdit()->append(DATAStytle+"PSR機能有無: "+conf.PSRfunction);
            this->getTextEdit()->append(DATAStytle+"リアノーマルビュー有無: "+conf.Rearnormalview);
            this->getTextEdit()->append(DATAStytle+"エンジン仕様: "+conf.Enginespecifications);
            this->getTextEdit()->append(DATAStytle+"タイヤサイズ: "+conf.Tiresize);
            this->getTextEdit()->append(DATAStytle+"コンフィグ部番: "+conf.Configpartnumber);
            if(i!=(comBean->getConfigTable()->size()-1))this->getTextEdit()->append("");
            i++;
        }
        break;
    case EEFileWrieflag:
    case ReadyFileWriteflag:
    case EEFileReadflag:
    case ReadyFileReadflag:
        this->getTextEdit()->setTextColor(0xFF0000);
        foreach (ERRORTable err, *errList) {
            this->getTextEdit()->append(DATAStytle+"文件名称: "+err.fileName);
            if(!err.sheetName.isEmpty()){
                this->getTextEdit()->append(DATAStytle+"sheet名称: "+err.sheetName);
            }
            if(err.row!=0||err.col!=0)
            {
                this->getTextEdit()->append(DATAStytle+"第: "+QString::number(err.row)+ " 行, 第: "+QString::number(err.col)+" 列");
            }
            this->getTextEdit()->append(DATAStytle+"错误描述: "+err.errMessage);
            if(i!=(errList->size()-1))this->getTextEdit()->append("");
            i++;
        }
        this->getTextEdit()->setTextColor(0xFFFFFF);
        break;
    case ZIPFile:
        if(!comBean->getZIPFilePath()->isEmpty()){
            this->getTextEdit()->append(DATAStytle+"ソフト一式("+comBean->getID()+") 压缩成功");
        }else{
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+"ソフト一式("+comBean->getID()+") 压缩失败");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
        break;
    }
    if(tmpflag)this->getTextEdit()->append(DATAStytle+" =======================================");
}
/**
 * @def 判断机种名称是否符合要求
 *      如果机种符合要求，则记录下来，字体设置为正常，如果存在错误码则移除错误码
 *      如果机种不符合要求，记录为空，字体设置为红色，设置错误码
 * @brief UIMethod::JudgeIDSlot
 * @param Edit
 */
void UIMethod::JudgeIDSlot(QLineEdit *Edit,QString *objectID)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDSlot() 函数执行!");
    //每次更新机种番号，临时的变量需要初始化
    carTmpPath="";
    carTmpPathflag=false;

    //设置正则表达式对象
    QRegExp rx(comBean->getRExpression().value(Edit->objectName()));
    if(rx.indexIn(Edit->text())!=0){
        Edit->setStyleSheet(QString(errFontColor)); //字体相关设置;
        return;
    }
    //对指定对象赋值
    *objectID=Edit->text();
    Edit->setStyleSheet(QString(nomFontColor)); //字体相关设置;
}
/**
 * @def 判断机种类型
 *      以作成机种(ID)为主导地位
 *      对比依赖机种与作成机种的类型是否一致
 *      如果不一致，则依赖机种显示红色字体
 * @brief UIMethod::JudgeIDTypeSlot
 * @param Edit
 * @param srcobject
 * @param desobject
 */
void UIMethod::JudgeIDTypeSlot(QLineEdit *Edit,QString *srcobject,QString *desobject)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDTypeSlot() 函数执行!");
    QString ret=comBean->getComMethod()->JudgeIDType(Edit->text());
    (*srcobject)=ret;
    //对比机种不为空，当前机种类型和对比机种不一致
    if(!(*desobject).isEmpty()&&ret!=(*desobject))
    {
        //错误处理，设置标记位false
        comBean->setIDRelyIDflag(false);
        return;
    }
    Edit->setStyleSheet(QString(nomFontColor));
    //正确处理，设置标记位true
    comBean->setIDRelyIDflag(true);
}
/**
 * @def 按钮选择目录并赋值
 * @brief UIMethod::SelectDirSlot
 * @param label
 * @param objectID
 */
void UIMethod::SelectDirSlot(QLabel *label,QString *objectID)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectDirSlot() 函数执行!");
    label->setText("");
    QString dirName =  QFileDialog::getExistingDirectory(label, tr("Open Directory"),comBean->desktopDirPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirName.isEmpty()) {
        label->setStyleSheet(QString(errFontColor));
        QMessageBox::warning(label, "Warn", tr("No directory selected!"));
        return;
    }
    label->setStyleSheet(QString(nomFontColor));
    label->setText(dirName);
    *objectID=dirName;
}
/**
 * @def 获取相应的文件路径
 * @brief UIMethod::SelectFileSlot
 * @param filters
 */
void UIMethod::SelectFileSlot(QString dirPath,unsigned int flag, bool goOn)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectFileSlot() 函数执行!");
    if(!QDir(dirPath).exists()){
        return;
    }
    QStringList filters;
    if(!dealFileFileThread->isRunning())
    {
        comBean->setStatusflag(1);
        dealFileFileThread->start();
    }
    switch (flag) {
    case RelyFileflag:
        if(*comBean->getIDType()=="EntryAVM2"){
            filters.append("*EntryAVM*ソフトウエア部品番号管理表(量産)_AKM対応用*.xls");
        }else{
            filters.append("*"+*(comBean->getIDType())+"*ソフトウエア部品番号管理表(量産)_AKM対応用*.xls");
        }
        break;
    case IniFileflag:
        filters.append("LOGZONE_*_2nd.ini");
        break;
    case PFileflag:
        if(!comBean->getRelyID()->isEmpty()){
            filters.append("*_"+*(comBean->getID())+"_AKM火災対応_P票.xls");
            filters.append(*(comBean->getRelyID())+"*と共有する.txt");
        }else{
            filters.append("*_"+*(comBean->getID())+"_AKM火災対応_P票.xls");
            //filters.append("*と共有する.txt");
        }
        break;
    case SWFileflag:
        if(!comBean->getRelyID()->isEmpty()){
            filters.append("コンパイルSW確認結果_*"+*(comBean->getID())+".xlsx");
            filters.append(*(comBean->getRelyID())+"*と共有する.txt");
        }else{
            filters.append("コンパイルSW確認結果_*"+*(comBean->getID())+".xlsx");
            //filters.append("*と共有する.txt");
        }
        break;
    case CarInfoFileflag:
        if(!carTmpPath.isEmpty()){
            dirPath=carTmpPath.left(carTmpPath.lastIndexOf("/"));
            filters.append("*_CarInfo.mot");
        }else{
            if(!comBean->getRelyID()->isEmpty()){
                filters.append("*"+*(comBean->getID())+"*_CarInfo.mot");
                filters.append("*"+*(comBean->getRelyID())+"*_CarInfo.mot");
            }else{
                filters.append("*"+*(comBean->getID())+"*_CarInfo.mot");
            }
        }
        break;
    case CarMapFileflag:
        if(!carTmpPath.isEmpty()){
            dirPath=carTmpPath.left(carTmpPath.lastIndexOf("/"));
            filters.append("*_CameraMAP.mot");
        }else{
            if(!comBean->getRelyID()->isEmpty()){
                filters.append("*"+*(comBean->getID())+"*_CameraMAP.mot");
                filters.append("*"+*(comBean->getRelyID())+"*_CameraMAP.mot");
            }else{
                filters.append("*"+*(comBean->getID())+"*_CameraMAP.mot");
            }
        }
        break;
    case CarOSDFileflag:
        if(!carTmpPath.isEmpty()){
            dirPath=carTmpPath.left(carTmpPath.lastIndexOf("/"));
            filters.append("*_OSD.mot");
        }else{
            if(!comBean->getRelyID()->isEmpty()){
                filters.append("*"+*(comBean->getID())+"*_OSD.mot");
                filters.append("*"+*(comBean->getRelyID())+"*_OSD.mot");
            }else{
                filters.append("*"+*(comBean->getID())+"*_OSD.mot");
            }
        }
        break;
    case JoinFileflag:
        filters.append("join_*"+*comBean->getID()+"*.mot");
        break;
    case APPFileflag:
        if(carTmpPath.contains(comBean->getRelyID())&&carTmpPathflag){
            filters.append("APP_*"+*comBean->getRelyID()+"*.mot");
        }else{
            filters.append("APP_*"+*comBean->getID()+"*.mot");
        }
        if(!comBean->getRelyID()->isEmpty()){
            filters.append("*"+*comBean->getRelyID()+"*.txt");
        }
        break;
    case EEFileflag:
        filters.append("EE-A002-1000 DR会議運用手順_様式7_20190320_EntryAVM*.xlsx");
        break;
    case ReadyFileflag:
        filters.append("*確認シート.xlsx");
        break;
    case ConfigFileflag:
        if(*comBean->getIDType()=="EntryAVM2"){
            filters.append("*EntryAVM採用車種コンフィグ詳細*.xlsx");
        }
        filters.append("*"+*(comBean->getIDType())+"*採用車種コンフィグ詳細*.xlsx");
        break;
    }
    dirPath=comBean->getComMethod()->AnalyzePath(dirPath,*(comBean->getID()),*(comBean->getIDType()),*(comBean->getRelyID()),flag);
    QLogHelper::instance()->LogDebug("寻找文件序号:"+QString::number(flag)+"   "+dirPath);
    //根据需求发送
    emit FindFileThreadSignal(dirPath,comBean->getComMethod(),filters,flag,goOn);
}
/**
 * @def 此函数功能是子线程查找文件后回调函数
 * @brief UIMethod::EndFindFileThreadSignal
 * @param st
 * @param flag
 */
void UIMethod::EndFindFileThreadSlot(QStringList st, unsigned int flag, bool goOn)
{
    QLogHelper::instance()->LogInfo("UIMethod->EndFindFileThreadSignal() 函数执行!");
    switch (flag) {
    case RelyFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getRelyFilePath(),flag,*comBean->getID());
        //如果文件存在，则开启线程解析文件表
        if(!comBean->getRelyFilePath()->isEmpty()){
            if(!excelThread->isRunning()){
                excelThread->start();
            }
            emit ExcelOperateThreadSignal(comBean->getExcelOption(),*(comBean->getRelyFilePath()),*(comBean->getID()),*(comBean->getIDType()),*(comBean->getRelyID()),flag);
        }
        break;
    case IniFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getIniFilePath(),flag,*comBean->getID());
        break;
    case PFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getPFilePath(),flag,*comBean->getID());
        break;
    case SWFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getSWFilePath(),flag,*comBean->getID());
        break;
    case CarInfoFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getCarInfoFilePath(),flag,*comBean->getID());
        if(carTmpPath.isEmpty()&&!comBean->getCarInfoFilePath()->isEmpty()){
            carTmpPath=*comBean->getCarInfoFilePath();
        }else if(!carTmpPath.isEmpty()&&comBean->getCarInfoFilePath()->isEmpty()){
            carTmpPath="";
        }
        break;
    case CarMapFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getCarMapFilePath(),flag,*comBean->getID());
        if(carTmpPath.isEmpty()&&!comBean->getCarMapFilePath()->isEmpty()){
            carTmpPath=*comBean->getCarMapFilePath();
        }else if(!carTmpPath.isEmpty()&&comBean->getCarMapFilePath()->isEmpty()){
            carTmpPath="";
        }
        break;
    case CarOSDFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getCarOSDFilePath(),flag,*comBean->getID());
        if(carTmpPath.isEmpty()&&!comBean->getCarOSDFilePath()->isEmpty()){
            carTmpPath=*comBean->getCarOSDFilePath();
        }else if(!carTmpPath.isEmpty()&&comBean->getCarOSDFilePath()->isEmpty()){
            carTmpPath="";
        }
        //再次对没有找到的文件进行查找处理
        if((comBean->getCarInfoFilePath()->isEmpty()||comBean->getCarMapFilePath()->isEmpty()||comBean->getCarOSDFilePath()->isEmpty())&&!carTmpPath.isEmpty()&&!carTmpPathflag)
        {
            flag=flag-2;
            carTmpPathflag=true;
            emit ActiveThreadSignal(*(comBean->getSVNDirPath()),flag,goOn);
            return;
        }
        if(carTmpPathflag){carTmpPathflag=false;carTmpPath="";}
        break;
    case JoinFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getJoinMot(),flag,*comBean->getID());
        break;
    case APPFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getAPPMot(),flag,*comBean->getID());
        //说明有留用关系，app.mot需要使用留用的app.mot
        if(comBean->getAPPMot()->contains(".txt")&&!carTmpPathflag)
        {
            carTmpPath=*comBean->getRelyID();
            carTmpPathflag=true;
            emit ActiveThreadSignal(*(comBean->getSVNDirPath()),flag,goOn);
            return;
        }
        if(carTmpPathflag){carTmpPathflag=false;}
        break;
    case EEFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getEEFilePath(),flag,*comBean->getID());
        break;
    case ReadyFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getReadyFilePath(),flag,*comBean->getID());
        break;
    case ConfigFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getConfigFilePath(),flag,*comBean->getID());
        //如果文件存在，则开启线程解析文件表
        if(!comBean->getConfigFilePath()->isEmpty()){
            if(!excelThread->isRunning()){
                excelThread->start();
            }
            emit ExcelOperateThreadSignal(comBean->getExcelOption(),*(comBean->getConfigFilePath()),*(comBean->getID()),*(comBean->getConfigFilePath()),*(comBean->getRelyID()),flag);
        }
        goOn=false;
        break;
    }
    emit ShowIDmessageSignal(flag);
    if(goOn){
        emit ActiveThreadSignal(*(comBean->getSVNDirPath()),flag+1,goOn);
        //emit ActiveThreadSignal(*(comBean->getSVNDirPath()),ConfigFileflag,goOn);
    }
    else{
        dealFileFileThread->quit();
        dealFileFileThread->wait();
        comBean->setStatusflag(0);
    }
}

/**
 * @def MessageView单元格数据修改触发函数
 * @brief UIMethod::MessageViewModelEditedSlot
 * @param item
 */
void UIMethod::MessageViewModelEditedSlot(QStandardItem *item)
{
    QStringList tmp;
    QString demo;
    if(comBean->getTableViewEditflag()&&!comBean->getID()->isEmpty()){
        QLogHelper::instance()->LogInfo("AutomationTool->MessageViewModelEditedSlot() 函数触发执行!");
        QLogHelper::instance()->LogDebug("column: "+QString::number(item->column())+"    row:"+QString::number(item->row()));
        if(item->text().contains("\\")){
            item->setText(comBean->getComMethod()->StringALLReplace(item->text(),"\\","/"));
        }
        if(item->column()==1){
            if(item->row()<=DefineTableSize){
                switch (item->row()) {
                case (RelyFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getRelyFilePath());
                    //如果文件存在，则开启线程解析文件表
                    if(!comBean->getRelyFilePath()->isEmpty()&&comBean->getRelyFilePath()->contains("ソフトウエア部品番号管理表(量産)_AKM対応用")&&comBean->getRelyFilePath()->contains(".xls")){
                        if(!excelThread->isRunning()){
                            excelThread->start();
                        }
                        emit ExcelOperateThreadSignal(comBean->getExcelOption(),*(comBean->getRelyFilePath()),*(comBean->getID()),*(comBean->getIDType()),*(comBean->getRelyID()),RelyFileflag);
                    }
                    break;
                case (IniFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getIniFilePath());
                    break;
                case (PFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getPFilePath());
                    break;
                case (SWFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getSWFilePath());
                    break;
                case (CarInfoFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getCarInfoFilePath());
                    break;
                case (CarMapFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getCarMapFilePath());
                    break;
                case (CarOSDFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getCarOSDFilePath());
                    break;
                case (APPFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getAPPMot());
                    break;
                case (JoinFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getJoinMot());
                    break;
                case (EEFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getEEFilePath());
                    break;
                case (ReadyFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getReadyFilePath());
                    break;
                case (ConfigFileflag-1):
                    comBean->getComMethod()->MessageFileTableChangeDeal(item,comBean->getConfigFilePath());
                    //如果文件存在，则开启线程解析文件表
                    if(!comBean->getConfigFilePath()->isEmpty()&&comBean->getConfigFilePath()->contains("採用車種コンフィグ詳細")&&comBean->getConfigFilePath()->contains(".xlsx")){
                        if(!excelThread->isRunning()){
                            excelThread->start();
                        }
                        emit ExcelOperateThreadSignal(comBean->getExcelOption(),*(comBean->getConfigFilePath()),*(comBean->getID()),*(comBean->getConfigFilePath()),*(comBean->getRelyID()),ConfigFileflag);
                    }
                    break;
                }
            }else{
                //宏定义修改
                if(item->row()>DefineTableSize&&item->row()<=(comBean->getSoftNumberTable()->size()+DefineTableSize)){
                    tmp=comBean->getDefineConfigList();
                    tmp.insert(item->row()-DefineTableSize-1,item->text());
                    comBean->setDefineConfigList(tmp);
                }
            }
        }
    }else if(comBean->getTableViewEditflag()&&comBean->getID()->isEmpty()){
        item->setText("");
    }
    comBean->setTableViewEditflag(false);
}

/**
 * @def 接收Button按钮传输信号
 * @brief UIMethod::CreateSlot
 */
void UIMethod::CreateSlot()
{
    QLogHelper::instance()->LogInfo("AutomationTool->CreateSlot() 函数触发执行!");
    this->getTextEdit()->append(DATAStytle+"=======================================");
    QDir *folder = new QDir();
    QFile *file=new QFile();
    QString fileName,tmpPath;
    bool flag=false;
    SOFTNUMBERTable soft;
    QDateTime current_date_time =QDateTime::currentDateTime();
    if(comBean->getSoftNumberTable()->size()>0){
        soft=comBean->getSoftNumberTable()->value(0);
    }
    comBean->setStatusflag(3);
    //如果输出路径不存在，则默认为桌面路径
    if(comBean->getOutputDirPath()->isEmpty()){
        *(comBean->getOutputDirPath())=comBean->desktopDirPath;
    }
    //目录结构创建
    tmpPath=*(comBean->getOutputDirPath())+"/"+*(comBean->getID());
    if(folder->exists(tmpPath))
    {
        folder->setPath(tmpPath);
        folder->removeRecursively();
        this->getTextEdit()->append(DATAStytle+tmpPath+" 目录文件夹删除成功!");
    }
    flag=folder->mkpath(tmpPath);
    if(!flag){this->getTextEdit()->append(DATAStytle+tmpPath+" 目录文件夹创建失败!");return;}
    this->getTextEdit()->append(DATAStytle+tmpPath+" 目录文件夹创建成功!");
    tmpPath=tmpPath+"/01_AKM対応";
    flag=folder->mkpath(tmpPath);
    if(!flag){
        this->getTextEdit()->setTextColor(0xFF0000);
        this->getTextEdit()->append(DATAStytle+tmpPath+" 目录文件夹创建失败!");
        this->getTextEdit()->setTextColor(0xFFFFFF);
        return;
    }
    this->getTextEdit()->append(DATAStytle+tmpPath+" 目录文件夹创建成功!");
    flag=folder->mkpath(tmpPath+"/ソフト一式("+comBean->getID()+")");
    if(!flag){
        this->getTextEdit()->setTextColor(0xFF0000);
        this->getTextEdit()->append(DATAStytle+tmpPath+"/ソフト一式("+comBean->getID()+")"+" 目录文件夹创建失败!");
        this->getTextEdit()->setTextColor(0xFFFFFF);
        return;
    }
    this->getTextEdit()->append(DATAStytle+tmpPath+"/ソフト一式("+comBean->getID()+")"+" 目录文件夹创建成功!");
    flag=folder->mkpath(tmpPath+"/ソフト一式("+comBean->getID()+")/結合版");
    if(!flag){
        this->getTextEdit()->setTextColor(0xFF0000);
        this->getTextEdit()->append(DATAStytle+tmpPath+"/ソフト一式("+comBean->getID()+")/結合版"+" 目录文件夹创建失败!");
        this->getTextEdit()->setTextColor(0xFFFFFF);
        return;
    }
    this->getTextEdit()->append(DATAStytle+tmpPath+"/ソフト一式("+comBean->getID()+")/結合版"+" 目录文件夹创建成功!");

    //CarInfo文件复制
    if(!comBean->getCarInfoFilePath()->isEmpty()&&file->exists(*comBean->getCarInfoFilePath()))
    {
        fileName=(*comBean->getCarInfoFilePath()).mid((*comBean->getCarInfoFilePath()).lastIndexOf("/")+1);
        if(QFile::copy(*comBean->getCarInfoFilePath(),tmpPath+"/ソフト一式("+comBean->getID()+")/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getCarInfoFilePath())+" 文件复制成功!");
        }else
        {
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+(*comBean->getCarInfoFilePath())+" 文件复制失败!");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
    }
    //CarMap文件复制
    if(!comBean->getCarMapFilePath()->isEmpty()&&file->exists(*comBean->getCarMapFilePath()))
    {
        fileName=(*comBean->getCarMapFilePath()).mid((*comBean->getCarMapFilePath()).lastIndexOf("/")+1);
        if(QFile::copy(*comBean->getCarMapFilePath(),tmpPath+"/ソフト一式("+comBean->getID()+")/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getCarMapFilePath())+" 文件复制成功!");
        }else
        {
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+(*comBean->getCarMapFilePath())+" 文件复制失败!");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
    }
    //OSD文件复制
    if(!comBean->getCarOSDFilePath()->isEmpty()&&file->exists(*comBean->getCarOSDFilePath()))
    {
        fileName=(*comBean->getCarOSDFilePath()).mid((*comBean->getCarOSDFilePath()).lastIndexOf("/")+1);
        if(QFile::copy(*comBean->getCarOSDFilePath(),tmpPath+"/ソフト一式("+comBean->getID()+")/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getCarOSDFilePath())+" 文件复制成功!");
        }else
        {
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+(*comBean->getCarOSDFilePath())+" 文件复制失败!");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
    }
    //APPmot文件复制
    if(!comBean->getAPPMot()->isEmpty()&&file->exists(*comBean->getAPPMot())&&!(*comBean->getAPPMot()).contains(".txt"))
    {
        fileName=(*comBean->getAPPMot()).mid((*comBean->getAPPMot()).lastIndexOf("/")+1);
        if(QFile::copy(*comBean->getAPPMot(),tmpPath+"/ソフト一式("+comBean->getID()+")/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getAPPMot())+" 文件复制成功!");
        }else
        {
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+(*comBean->getAPPMot())+" 文件复制失败!");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
    }
    //INI文件复制
    if(!comBean->getIniFilePath()->isEmpty()&&file->exists(*comBean->getIniFilePath())&&!soft.PartNumber.isEmpty())
    {
        fileName="LOGZONE_";
        if(*comBean->getIDType()=="EntryAVM2"){
            fileName=fileName+"Entry2AVM_";
        }else{
            fileName=fileName+comBean->getIDType()+"_";
        }
        fileName=fileName+comBean->getID()+"_2nd.ini";
        if(QFile::copy(*comBean->getIniFilePath(),tmpPath+"/ソフト一式("+comBean->getID()+")/結合版/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getIniFilePath())+" 文件复制成功!");
            comBean->getComMethod()->INIFileWrite(tmpPath+"/ソフト一式("+comBean->getID()+")/結合版/"+fileName,comBean->getSoftNumberTable()->value(0).PartNumber,comBean->getSoftNumberTable()->value(0).DiagnosticCode);
        }else
        {
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+(*comBean->getIniFilePath())+" 文件复制失败!");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
    }
    //Join Mot文件复制
    if(!comBean->getJoinMot()->isEmpty()&&file->exists(*comBean->getJoinMot()))
    {
        fileName=(*comBean->getJoinMot()).mid((*comBean->getJoinMot()).lastIndexOf("/")+1);
        if(QFile::copy(*comBean->getJoinMot(),tmpPath+"/ソフト一式("+comBean->getID()+")/結合版/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getJoinMot())+" 文件复制成功!");
            *(comBean->getZIPFilePath())="";
            if(!dealFileFileThread->isRunning())
            {
                dealFileFileThread->start();
            }
            //打包文件
            emit FileCompressionSignal(comBean->getComMethod()->Get7zInstallPath(),tmpPath+"/ソフト一式("+comBean->getID()+")");
        }else
        {
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+(*comBean->getJoinMot())+" 文件复制失败!");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
    }
    //01_AKM対応目录下文件
    //P票文件复制
    if(!comBean->getPFilePath()->isEmpty()&&file->exists(*comBean->getPFilePath())&&!(*comBean->getPFilePath()).contains(".txt"))
    {
        fileName=(*comBean->getPFilePath()).mid((*comBean->getPFilePath()).lastIndexOf("/")+1);
        if(QFile::copy(*comBean->getPFilePath(),tmpPath+"/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getPFilePath())+" 文件复制成功!");
        }else
        {
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+(*comBean->getPFilePath())+" 文件复制失败!");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
    }
    //SW确认文件复制
    if(!comBean->getSWFilePath()->isEmpty()&&file->exists(*comBean->getSWFilePath())&&!(*comBean->getSWFilePath()).contains(".txt"))
    {
        fileName=(*comBean->getSWFilePath()).mid((*comBean->getSWFilePath()).lastIndexOf("/")+1);
        if(QFile::copy(*comBean->getSWFilePath(),tmpPath+"/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getSWFilePath())+" 文件复制成功!");
        }else
        {
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+(*comBean->getSWFilePath())+" 文件复制失败!");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
    }
    //EntryAVM採用車種コンフィグ詳細_Ver2.26_20210416修正.xlsx文件复制
    if(!comBean->getConfigFilePath()->isEmpty()&&file->exists(*comBean->getConfigFilePath()))
    {
        fileName=(*comBean->getConfigFilePath()).mid((*comBean->getConfigFilePath()).lastIndexOf("/")+1);
        if(QFile::copy(*comBean->getConfigFilePath(),tmpPath+"/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getConfigFilePath())+" 文件复制成功!");
        }else
        {
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+(*comBean->getConfigFilePath())+" 文件复制失败!");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
    }
    //EE-A002-1000 DR会議運用手順_様式7文件复制 并填写完成
    if(!comBean->getEEFilePath()->isEmpty()&&file->exists(*comBean->getEEFilePath())){
        fileName="EE-A002-1000 DR会議運用手順_様式7_20190320_";
        if(*comBean->getIDType()=="EntryAVM2"){
            fileName=fileName+"Entry2AVM_";
        }else{
            fileName=fileName+comBean->getIDType()+"_";
        }
        fileName=fileName+comBean->getID()+"_"+current_date_time.toString("yyyyMMdd")+".xlsx";
        if(QFile::copy(*comBean->getEEFilePath(),tmpPath+"/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getEEFilePath())+" 文件复制成功!");
            if(!excelThread->isRunning()){
                excelThread->start();
            }
            emit EEExcelWriteSignal(comBean->getExcelOption(),tmpPath+"/"+fileName,*(comBean->getID()),*(comBean->getIDType()),*(comBean->getRelyID()),comBean->getSoftNumberTable());
        }else
        {
            this->getTextEdit()->append(DATAStytle+(*comBean->getEEFilePath())+" 文件复制失败!");
        }
    }
    //確認シート.xlsx文件复制
    if(!comBean->getReadyFilePath()->isEmpty()&&file->exists(*comBean->getReadyFilePath())&&!soft.PartNumber.isEmpty()){
        if(!soft.CarModels.isEmpty()){
            fileName=soft.CarModels+" "+soft.PartNumber+"確認シート.xlsx";
        }else{
            fileName="確認シート.xlsx";
        }
        if(QFile::copy(*comBean->getReadyFilePath(),tmpPath+"/"+fileName)){
            this->getTextEdit()->append(DATAStytle+(*comBean->getReadyFilePath())+" 文件复制成功!");
            if(!excelThread->isRunning()){
                excelThread->start();
            }
            emit ReadyExcelWriteSignal(comBean->getExcelOption(),tmpPath+"/"+fileName,comBean->getSoftNumberTable(),comBean->getConfigTable(),comBean->getDefineConfigList(),*(comBean->getRelyID()),*(comBean->getIDType()));
            //emit ReadyExcelWriteSignal(comBean->getExcelOption(),tmpPath+"/"+fileName,*(comBean->getID()),*(comBean->getIDType()),comBean->getSoftNumberTable(),comBean->getConfigTable(),*(comBean->getRelyID(),*(comBean->getIDType())));
        }else
        {
            this->getTextEdit()->setTextColor(0xFF0000);
            this->getTextEdit()->append(DATAStytle+(*comBean->getReadyFilePath())+" 文件复制失败!");
            this->getTextEdit()->setTextColor(0xFFFFFF);
        }
    }
    this->getTextEdit()->append(DATAStytle+" =======================================");
}


/**
 * @def 量产管理表解析完成后回调函数
 * @brief UIMethod::EndExcelOperateThreadSoftSlot
 * @param list
 */
void UIMethod::EndExcelOperateThreadSoftSlot(QList<SOFTNUMBERTable> list)
{
    QLogHelper::instance()->LogInfo("UIMethod->EndExcelOperateThreadSoftSlot() 函数执行!");
    //QLogHelper::instance()->LogDebug(QString::number(list.size()));
    *(comBean->getSoftNumberTable())=list;
    emit ShowIDmessageSignal(DefineConfig);
    emit ShowIDmessageSignal(RelyMessageflag);
    excelThread->quit();
    excelThread->wait();
}
/**
 * @def EntryAVM採用車種コンフィグ詳細表解析完成后回调函数
 * @brief UIMethod::EndExcelOperateThreadConfSlot
 * @param list
 */
void UIMethod::EndExcelOperateThreadConfSlot(QList<CONFIGTable> list)
{
    QLogHelper::instance()->LogInfo("UIMethod->EndExcelOperateThreadConfSlot() 函数执行!");
    //QLogHelper::instance()->LogDebug(QString::number(list.size()));
    *(comBean->getConfigTable())=list;
    emit ShowIDmessageSignal(ConfigMessageflag);
    excelThread->quit();
    excelThread->wait();
}
/**
 * @def EE-A002-1000 DR会議運用手順_様式7_20190320*.xlsx表 修改完成后回调函数
 * @brief UIMethod::EndEEExcelWriteSlot
 * @param errTableList
 */
void UIMethod::EndEEExcelWriteSlot(QList<ERRORTable> *errTableList)
{
    QLogHelper::instance()->LogInfo("UIMethod->EndEEExcelWriteSlot() 函数执行!");
    this->getTextEdit()->append(DATETIME+"=======================================");
    this->getTextEdit()->append(DATAStytle+comBean->getEEFilePath()->mid(comBean->getEEFilePath()->lastIndexOf("/")+1)+" 文件写入完成");
    this->getTextEdit()->append(DATETIME+"=======================================");
    errList->clear();
    (*errList)=(*errTableList);
    emit ShowIDmessageSignal(EEFileWrieflag);
    //excelThread->quit();
    //excelThread->wait();
}
/**
 * @def *確認シート.xlsx 修改完成后回调函数
 * @brief UIMethod::EndReadyExcelWriteSlot
 * @param errTableList
 */
void UIMethod::EndReadyExcelWriteSlot(QList<ERRORTable> *errTableList)
{
    QLogHelper::instance()->LogInfo("UIMethod->EndReadyExcelWriteSlot() 函数执行!");
    //QLogHelper::instance()->LogDebug("excelThread-------"+QString::number(excelThread->isFinished()));
    //QLogHelper::instance()->LogDebug("excelThread-------"+QString::number(excelThread->isRunning()));
    excelThread->quit();
    excelThread->wait();
    this->getTextEdit()->append(DATETIME+"=======================================");
    this->getTextEdit()->append(DATAStytle+comBean->getReadyFilePath()->mid(comBean->getReadyFilePath()->lastIndexOf("/")+1)+" 文件写入完成");
    this->getTextEdit()->append(DATETIME+"=======================================");
    errList->clear();
    (*errList)=(*errTableList);
    emit ShowIDmessageSignal(ReadyFileWriteflag);
    if(!dealFileFileThread->isRunning()){
        comBean->setStatusflag(0);
    }
}
/**
 * @def 文件压缩完成后回调函数
 * @brief UIMethod::EndFileCompressionSlot
 * @param filePath
 */
void UIMethod::EndFileCompressionSlot(const QString filePath)
{
    QLogHelper::instance()->LogInfo("AutomationTool->EndFileCompressionSlot() 函数触发执行!");
    //QLogHelper::instance()->LogDebug("dealFileFileThread-------"+QString::number(dealFileFileThread->isFinished()));
    //QLogHelper::instance()->LogDebug("dealFileFileThread-------"+QString::number(dealFileFileThread->isRunning()));
    dealFileFileThread->quit();
    dealFileFileThread->wait();
    if(!excelThread->isRunning()){
        comBean->setStatusflag(0);
    }
    if(!filePath.isEmpty()){
        *(comBean->getZIPFilePath())=filePath;
    }
    emit ShowIDmessageSignal(ZIPFile);
}
