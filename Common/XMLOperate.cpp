#include "XMLOperate.h"

XMLOperate::XMLOperate(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("XMLOperate() 构造函数执行!");
    errCodeType=ReadXML(xmlPath);
    QLogHelper::instance()->LogDebug(QString::number(getErrCodeType().count()));


    QMapIterator<QString,ERRCODETYPE> i(errCodeType);
    while(i.hasNext()) {
        i.next();
         QLogHelper::instance()->LogDebug(i.key());
    }
}


/**
 * @brief XMLOperate::getErrCodeType
 * @return
 */
QMap<QString, ERRCODETYPE> XMLOperate::getErrCodeType() const
{
    return errCodeType;
}

/**
 * @def 读取XML配置文件
 * @brief XMLOperate::ReadXML
 * @param filePath
 * @return
 */
QMap<QString,ERRCODETYPE> XMLOperate::ReadXML(const QString filePath)
{
    QLogHelper::instance()->LogInfo("XMLOperate->ReadXML() 函数执行!");
    QMap<QString,ERRCODETYPE> errMap;
    QFile xmlFile(filePath);
    if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){return errMap;}
    QXmlStreamReader reader(&xmlFile);
    while(!reader.atEnd())
    {
        ERRCODETYPE err;
        //判断是否是节点的开始
        if(reader.isStartElement())
        {
            if(reader.name() == "ID")       //判断当前节点的名字是否为ID
            {
                err.ID= reader.readElementText().replace(QString("\""), QString(""));
            }
            else if(reader.name() == "Def")  //判断当前节点的名字是否为Def
            {
                err.Def= reader.readElementText();
            }
            else if(reader.name() == "ErrDef")  //判断当前节点的名字是否为ErrDef
            {
                err.ErrDef =reader.readElementText();
            }
            else if(reader.name() == "Level")  //判断当前节点的名字是否为Level
            {
                err.Level=reader.readElementText();
            }
            else if(reader.name() == "Code")  //判断当前节点的名字是否为Code
            {
                err.Code =reader.readElementText();
            }
        }
        if(!err.ID.isEmpty()&& errMap.value(err.ID).ID.isEmpty()){
            errMap.insert(err.ID,err);
        }
        reader.readNext();
    }
    xmlFile.close();
    return errMap;
}
