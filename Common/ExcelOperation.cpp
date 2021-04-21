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
    QList<SOFTNUMBERTable> softlist;
    SOFTNUMBERTable soft;
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        if(IDType!="EntryAVM2"){
            sheetread = book->getSheet(0);
        }else{
            sheetread = book->getSheet(1);
        }
        int rowNum = sheetread->lastRow();
        int colNum = sheetread->lastCol();
        for (int i = 1; i < rowNum; ++i)
        {
            if(ID==QString::fromLocal8Bit(sheetread->readStr(i,1))||(flag&&QString::fromLocal8Bit(sheetread->readStr(i,1)).isEmpty())){
                QLogHelper::instance()->LogDebug(QString::number(i));
                flag=true;
            }else{flag=false;}
            for (int j = 0; (j < colNum)&&flag&&(sheetread->cellFormat(i,5)->patternBackgroundColor()==COLOR_AUTO); ++j)
            {
                //qDebug() << QString::fromLocal8Bit(sheetread->readStr(i,j));
                //进行赋值操作
            }
        }
    }
    return softlist;
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
    QList<CONFIGTable> conflist;
    CONFIGTable conf;
    if(this->Init(filePath)&&book->load(filePath.toLocal8Bit()))
    {
        sheetread = book->getSheet(1);
        int rowNum = sheetread->lastRow();
        int colNum = sheetread->lastCol();
        for (int i = 1; i < rowNum; ++i)
        {
            if(QString::fromLocal8Bit(sheetread->readStr(i,1)).contains(ID)){
                QLogHelper::instance()->LogDebug(QString::number(i));
                flag=true;
            }else{flag=false;}
            /*for (int j = 0; (j < colNum)&&flag&&(sheetread->cellFormat(i,5)->patternBackgroundColor()==COLOR_AUTO); ++j)
            {
                //qDebug() << QString::fromLocal8Bit(sheetread->readStr(i,j));
                //进行赋值操作
            }
            */
        }
    }
    return conflist;
}

