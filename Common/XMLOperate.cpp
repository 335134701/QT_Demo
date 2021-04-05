#include "XMLOperate.h"

XMLOperate::XMLOperate(QObject *parent) : QObject(parent)
{

}
/**
 * @def 读取XML配置文件
 * @brief XMLOperate::ReadXML
 */
void XMLOperate::ReadXML()
{

}

QMap<QString, ERRCODETYPE> XMLOperate::getErrCodeType() const
{
    return errCodeType;
}
