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
 * @def 获取本机svn程序安装位置
 * @brief CommonMethod::GetSVNInstallPath
 * @return
 */
QString CommonMethod::GetSVNInstallPath()
{
    QLogHelper::instance()->LogInfo("CommonMethod->GetSVNInstallPath() 函数执行!");
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\TortoiseSVN",QSettings::NativeFormat);
    return settings.value("Directory").toString()+"bin\\svn.exe";
}

/**
 * @def 文件复制功能
 * @brief CommonMethod::CopyFile
 * @param desDirPath
 * @param srcDirPath
 * @return
 */
bool CommonMethod::CopyFile(const QString desDirPath, const QString srcDirPath)
{
    QLogHelper::instance()->LogInfo("CommonMethod->CopyFile() 函数执行!");
}
