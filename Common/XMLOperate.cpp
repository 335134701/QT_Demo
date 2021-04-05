#include "XMLOperate.h"

XMLOperate::XMLOperate(QObject *parent) : QObject(parent)
{

}
/**
 * @def 读取XML文件
 * @brief XMLOperate::OpenXML
 * @param filePath
 * @return
 */
bool XMLOperate::OpenXML(const QString filePath)
{
    if(!filePath.isEmpty()){return false;}
    return false;
}

QDomNode *XMLOperate::listDom(QDomElement *docElem)
{

}
