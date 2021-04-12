#include "XMLOperate.h"

XMLOperate::XMLOperate(QObject *parent) : QObject(parent)
{

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
 */
void XMLOperate::ReadXML()
{
    QLogHelper::instance()->LogInfo("XMLOperate->ReadXML() 函数执行!");
    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){return;}
    QXmlStreamReader reader(&xmlFile);
    while(!reader.atEnd())
    {
        ERRCODETYPE err;
        //判断是否是节点的开始
        if(reader.isStartElement())
        {
            if(reader.name() == "ID")       //判断当前节点的名字是否为ID
            {
                err.ID= reader.readElementText();
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
        if(getErrCodeType().value(err.ID).ID.isEmpty()){
            errCodeType.insert(err.ID,err);
        }
        reader.readNext();
    }
    xmlFile.close();
}
