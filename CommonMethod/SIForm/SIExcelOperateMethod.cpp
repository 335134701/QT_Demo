#include "SIExcelOperateMethod.h"

/**
 * @brief SIExcelOperateMethod::SIExcelOperateMethod
 * @param parent
 */
SIExcelOperateMethod::SIExcelOperateMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIExcelOperateMethod 构造函数执行!");
    this->Init();
}

/**
 * @brief SIExcelOperateMethod::Init
 */
void SIExcelOperateMethod::Init()
{
    QLogHelper::instance()->LogInfo("SIExcelOperateMethod->Init() 函数执行!");
    codec = QTextCodec::codecForName("GBK");
}


/**
 * @def 初始化LibXL库
 * @brief SIExcelOperateMethod::InitLIBXL
 * @param filePath
 * @return
 */
bool SIExcelOperateMethod::InitLIBXL(const QString filePath)
{
    QLogHelper::instance()->LogInfo("SIExcelOperateMethod->InitLIBXL() 函数执行!");
    if(!QFile::exists(filePath)){return false;}
    if(filePath.endsWith("xls",Qt::CaseInsensitive))//判断是否是.xls文件，不区分大小写
    {
        book = xlCreateBook(); // xlCreateBook() for xls
    }
    else if(filePath.endsWith("xlsx",Qt::CaseInsensitive))//判断是否是.xlsx文件，不区分大小写
    {
        book = xlCreateXMLBook(); // xlCreateXMLBook() for xlsx
    }
    book->setKey(SILibName, SILibPW);
    return true;
}

/**
 * @def 读取量产管理表
 * @brief SIExcelOperateMethod::ReadSoftExcel
 * @param filePath
 * @param ID
 * @param IDType
 * @return
 */
QList<SI_SOFTNUMBERTable> SIExcelOperateMethod::ReadSoftExcel(const QString filePath, const QString ID, const QString IDType,QList<SI_ERRORTable> *errList)
{
    QLogHelper::instance()->LogInfo("SIExcelOperateMethod->ReadSoftExcel() 函数执行!");
    bool flag=false;
    unsigned int lastRow=0,firstRow=0,firstCol=0;
    Sheet *sheetread;
    QList<SI_SOFTNUMBERTable> *softlist=new QList<SI_SOFTNUMBERTable>();
    QList<SI_SOFTNUMBERTable> *tmpsoftlist=new QList<SI_SOFTNUMBERTable>();
    QString CarModels,PartNumber,CANGen;
    if(this->InitLIBXL(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        if(IDType!="EntryAVM2"){
            sheetread = book->getSheet(0);
        }else{
            sheetread = book->getSheet(1);
        }
        if(NULL==sheetread){
            errList->append(SICommonMethod::SetERRMessage(filePath.mid(filePath.lastIndexOf("/")+1),"Sheet获取异常"));
            return *softlist;
        }
        firstCol=sheetread->firstCol();
        firstRow=sheetread->firstRow();
        lastRow=sheetread->lastRow();
        for (unsigned int i = firstRow; i < lastRow; ++i)
        {
            if(ID==QString::fromLocal8Bit(sheetread->readStr(i,firstCol))||(flag&&QString::fromLocal8Bit(sheetread->readStr(i,firstCol)).isEmpty())){
                flag=true;
            }else{flag=false;}
            if(flag&&sheetread->cellFormat(i,5)->font()->strikeOut()!=1&&!QString(sheetread->readStr(i,5)).isEmpty())
            {
                if(CarModels.isEmpty()){CarModels=QString(sheetread->readStr(i,firstCol+1));}
                if(PartNumber.isEmpty()){PartNumber=QString(sheetread->readStr(i,firstCol+2));}
                if(CANGen.isEmpty()){CANGen=QString(sheetread->readStr(i,firstCol+3));}
                SI_SOFTNUMBERTable soft;
                soft.ModelNumber=ID;
                soft.CarModels=CarModels;
                soft.PartNumber=PartNumber;
                soft.CANGen=CANGen;
                soft.Productionstage=codec->toUnicode(*(new QByteArray(sheetread->readStr(i,firstCol+4))));
                soft.ApplicationPartNo=QString(sheetread->readStr(i,firstCol+10));
                soft.ApplicationVer=QString(sheetread->readStr(i,firstCol+11));
                soft.CarInfoPartNo=QString(sheetread->readStr(i,firstCol+12));
                soft.CarInfoVer=QString(sheetread->readStr(i,firstCol+13));
                if((sheetread->cellFormat(i,5)->patternForegroundColor()==13||sheetread->cellFormat(i,5)->patternForegroundColor()==51))
                {
                    softlist->append(soft);
                }else{
                    tmpsoftlist->append(soft);
                }
            }
        }
        book->release();
    }
    if(softlist->size()==0){
        return *tmpsoftlist;
    }else{
        return *softlist;
    }
}

/**
 * @def 根据条件读取量产管理表
 * @brief SIExcelOperateMethod::RelyReadSoftExcel
 * @param filePath
 * @param IDType
 * @param condition
 * @return
 */
QList<SI_SOFTNUMBERTable> SIExcelOperateMethod::RelyReadSoftExcel(const QString filePath, const QString IDType, const QString condition)
{
    QLogHelper::instance()->LogInfo("SIExcelOperateMethod->RelyReadSoftExcel() 函数执行!");
    /*
    unsigned int lastRow=0,firstRow=0,firstCol=0;
    Sheet *sheetread;
    QList<SI_SOFTNUMBERTable> *softlist=new QList<SI_SOFTNUMBERTable>();
    QString CarModels,PartNumber,CANGen;
    if(this->InitLIBXL(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        if(IDType!="EntryAVM2"){
            sheetread = book->getSheet(0);
        }else{
            sheetread = book->getSheet(1);
        }
        if(NULL==sheetread){ return *softlist; }
        firstCol=sheetread->firstCol();
        firstRow=sheetread->firstRow();
        lastRow=sheetread->lastRow();
        for (unsigned int i = firstRow; i < lastRow; ++i)
        {
            if(CarModels.isEmpty()){CarModels=QString(sheetread->readStr(i,firstCol+1));}
            if(PartNumber.isEmpty()){PartNumber=QString(sheetread->readStr(i,firstCol+2));}
            if(CANGen.isEmpty()){CANGen=QString(sheetread->readStr(i,firstCol+3));}
            SI_SOFTNUMBERTable soft;
            soft.ModelNumber=ID;
            soft.CarModels=CarModels;
            soft.PartNumber=PartNumber;
            soft.CANGen=CANGen;
            soft.Productionstage=codec->toUnicode(*(new QByteArray(sheetread->readStr(i,firstCol+4))));
            soft.ApplicationPartNo=QString(sheetread->readStr(i,firstCol+10));
            soft.ApplicationVer=QString(sheetread->readStr(i,firstCol+11));
            soft.CarInfoPartNo=QString(sheetread->readStr(i,firstCol+12));
            soft.CarInfoVer=QString(sheetread->readStr(i,firstCol+13));
        }
        book->release();
    }
    */
}

/**
 * @def 根据条件对量产管理表进行二次处理
 * @brief SIExcelOperateMethod::DealSoftTable
 * @param softList
 * @param condition
 * @return
 */
QList<SI_SOFTNUMBERTable> SIExcelOperateMethod::DealSoftTable(QList<SI_SOFTNUMBERTable> softList, const QString condition)
{
    QLogHelper::instance()->LogInfo("SIExcelOperateMethod->DealSoftTable() 函数执行!");
    bool tmpflag=false;
    if(softList.size()<=0){return softList;}
    QList<SI_SOFTNUMBERTable> *tmpsoftlist=new QList<SI_SOFTNUMBERTable>();
    SI_SOFTNUMBERTable so;
    //对未写入数据初始化
    for(int i=0;i<softList.size();i++){
        if(!softList[i].ApplicationPartNo.isEmpty()){so.ApplicationPartNo=softList[i].ApplicationPartNo;}
        else {softList[i].ApplicationPartNo=so.ApplicationPartNo;}

        if(!softList[i].ApplicationVer.isEmpty()){so.ApplicationVer=softList[i].ApplicationVer;}
        else {softList[i].ApplicationVer=so.ApplicationVer;}

        if(!softList[i].CarInfoPartNo.isEmpty()){so.CarInfoPartNo=softList[i].CarInfoPartNo;}
        else {softList[i].CarInfoPartNo=so.CarInfoPartNo;}

        if(!softList[i].CarInfoVer.isEmpty()){so.CarInfoVer=softList[i].CarInfoVer;}
        else {softList[i].CarInfoVer=so.CarInfoVer;}
    }
    foreach (SI_SOFTNUMBERTable soft, softList) {
        if(soft.Productionstage==condition){ tmpflag=true;}
        tmpsoftlist->append(soft);
    }
    if(!tmpflag){
        tmpsoftlist->clear();
        tmpsoftlist->append(softList[softList.size()-1]);
    }
    return *tmpsoftlist;
}

/**
 * @brief SIExcelOperateMethod::ReadDefineExcel
 * @param filePath
 * @param ID
 * @param errList
 * @return
 */
QList<SI_DEFINEMESSAGE> SIExcelOperateMethod::ReadDefineExcel(const QString filePath, const QString ID, const QString IDType, QList<SI_ERRORTable> *errList)
{
    QLogHelper::instance()->LogInfo("SIExcelOperateMethod->ReadDefineExcel() 函数执行!");
    unsigned int lastRow=0,lastCol=0,firstRow=0,firstCol=0,i=0,j=0;
    Sheet *sheetread;
    QList<SI_DEFINEMESSAGE> *defineList=new QList<SI_DEFINEMESSAGE>();
    QString tmp;
    if(this->InitLIBXL(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        if(IDType!="EntryIPA"){
            sheetread = book->getSheet(1);
        }else{
            sheetread = book->getSheet(2);
        }
        if(NULL==sheetread){
            errList->append(SICommonMethod::SetERRMessage(filePath.mid(filePath.lastIndexOf("/")+1),"Sheet获取异常"));
            return *defineList;
        }
        lastRow=sheetread->lastRow();
        lastCol=sheetread->lastCol();
        firstCol=sheetread->firstCol();
        firstRow=sheetread->firstRow();
        //目前做的是绝对定位，后续可能作成相对定位
        if(IDType.contains("EntryAVM1")||IDType.contains("EntryAVM2")){i=i+2;}
        else{i=i+3;}
        for (; i < lastCol; ++i)
        {
            if(ID==QString::fromLocal8Bit(sheetread->readStr(firstRow+2,i))){
                for(j=firstRow+3;j<lastRow;j++){
                    SI_DEFINEMESSAGE define;
                    define.stageName=codec->toUnicode(*(new QByteArray(sheetread->readStr(j,firstCol))));
                    if(define.stageName.isEmpty()){
                        define.stageName=tmp;
                    }else{
                        tmp=define.stageName;
                    }
                    define.defineName=codec->toUnicode(*(new QByteArray(sheetread->readStr(j,firstCol+1))));
                    if(codec->toUnicode(*(new QByteArray(sheetread->readStr(j,i))))=="○"){
                        define.isUse=true;
                    }else{
                        define.isUse=false;
                    }
                    defineList->append(define);
                }
            }
        }
        book->release();
    }

    return *defineList;
}


