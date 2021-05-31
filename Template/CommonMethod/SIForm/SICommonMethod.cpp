#include "SICommonMethod.h"

/**
 * @brief SICommonMethod::SICommonMethod
 * @param parent
 */
SICommonMethod::SICommonMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SICommonMethod 构造函数执行!");
}

/**
 * @def 判断机种类型
 *      根据ID字符串，第三四位字符判断
 * @brief SICommonMethod::JudgeIDType
 * @param ID
 * @return
 */
QString SICommonMethod::JudgeIDType(const QString ID)
{
    QLogHelper::instance()->LogInfo("SICommonMethod->JudgeIDType() 函数执行!");
    QString ret;
    if(!ID.contains("EN")){return NULL;}
    //字符转数字
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
        break;
    }
    return ret;
}
