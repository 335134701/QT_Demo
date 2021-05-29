#include "CommonMethod.h"

/**
 * @brief CommonMethod::CommonMethod
 * @param parent
 */
CommonMethod::CommonMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("CommonMethod 构造函数执行!");
}

/**
 * @def 获取本机7z程序安装位置
 * @brief CommonMethod::Get7zInstallPath
 * @return
 */
QString CommonMethod::Get7zInstallPath()
{
    QLogHelper::instance()->LogInfo("CommonMethod->Get7zInstallPath() 函数执行!");
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\7-Zip",QSettings::NativeFormat);
    return settings.value("Path").toString()+"7z.exe";
}

/**
 * @def 判断机种类型
 *      根据ID字符串，第三四位字符判断
 * @brief CommonMethod::JudgeIDType
 * @param ID
 * @return
 */
QString CommonMethod::JudgeIDType(const QString ID)
{
    QLogHelper::instance()->LogInfo("CommonMethod->JudgeIDType() 函数执行!");
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
