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
 * @def 获取本机zip程序安装位置
 * @brief CommonMethod::GetZIPInstallPath
 * @return
 */
QString CommonMethod::GetZIPInstallPath()
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
 * @param srcFilePath
 * @param desFilePath
 * @return
 */
bool CommonMethod::CopyFile(const QString srcFilePath,const QString desFilePath)
{
    QLogHelper::instance()->LogInfo("CommonMethod->CopyFile() 函数执行!");
    bool flag=false;
    QFile *file=new QFile();
    if(file->exists(srcFilePath))
    {
        if(file->exists(desFilePath)){file->remove(desFilePath);}
        flag=file->copy(srcFilePath,desFilePath);
    }
    return flag;
}

/**
 * @def 文件夹复制功能
 * @brief CommonMethod::CopyDir
 * @param srcPath
 * @param desPath
 * @return
 */
bool CommonMethod::CopyDir(QString srcPath, QString desPath)
{
    //QLogHelper::instance()->LogInfo("SICommonMethod->CopyFile() 函数执行!");
    QDir dir(srcPath);
    if (!dir.exists())return false;
    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = desPath + QDir::separator() + d;
        dir.mkpath(dst_path);
        CopyDir(srcPath+ QDir::separator() + d, dst_path);//use recursion
    }
    foreach (QString f, dir.entryList(QDir::Files)) {
        if(QFile(desPath + QDir::separator() + f).exists()){
            dir.remove(desPath + QDir::separator() + f);
        }
        if(!QFile::copy(srcPath + QDir::separator() + f, desPath + QDir::separator() + f)){
            return false;
        }
    }
    return true;
}

