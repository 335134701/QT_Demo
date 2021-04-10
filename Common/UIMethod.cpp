#include "UIMethod.h"

UIMethod::UIMethod(QObject *parent) : QObject(parent)
{

}
/**
 * @def 判断机种类型
 * @brief UIMethod::JudgeIDType
 * @param ID
 * @return
 */
QString UIMethod::JudgeIDType(const QString ID)
{
    QString ret;
    if(!ID.contains("EN")){return NULL;}
    switch (ID.mid(2,2).toInt()) {
    case 33 :
    case 40 :
    case 42 :
        ret="EntryAVM";
        break;
    case 34:
        ret="EntryAVM2";
        break;
    case 35:
        ret="EntryIPA";
        break;
    case 36:
        ret="FAP";
        break;
    case 37:
        ret="NextPH3";
        break;
    default:
        ret="";
        break;
    }
    QLogHelper::instance()->LogDebug(ret);
    return ret;
}
