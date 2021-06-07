#include "SIConfig.h"

/**
 * @brief SIConfig::SIConfig
 * @param parent
 */
SIConfig::SIConfig(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIConfig 构造函数执行!");
    this->InitParameter();
    this->Init();
}

/**
 * @brief SIConfig::getEntryAVM1
 * @return
 */
QList<SI_DEFINEMESSAGE> *SIConfig::getEntryAVM1() const
{
    return entryAVM1;
}

/**
 * @brief SIConfig::setEntryAVM1
 * @param value
 */
void SIConfig::setEntryAVM1(QList<SI_DEFINEMESSAGE> *value)
{
    entryAVM1 = value;
}

/**
 * @brief SIConfig::getEntryAVM2
 * @return
 */
QList<SI_DEFINEMESSAGE> *SIConfig::getEntryAVM2() const
{
    return entryAVM2;
}

/**
 * @brief SIConfig::setEntryAVM2
 * @param value
 */
void SIConfig::setEntryAVM2(QList<SI_DEFINEMESSAGE> *value)
{
    entryAVM2 = value;
}

/**
 * @brief SIConfig::getIpa
 * @return
 */
QList<SI_DEFINEMESSAGE> *SIConfig::getIpa() const
{
    return ipa;
}

/**
 * @brief SIConfig::setIpa
 * @param value
 */
void SIConfig::setIpa(QList<SI_DEFINEMESSAGE> *value)
{
    ipa = value;
}

/**
 * @brief SIConfig::getNextPH3
 * @return
 */
QList<SI_DEFINEMESSAGE> *SIConfig::getNextPH3() const
{
    return nextPH3;
}

/**
 * @brief SIConfig::setNextPH3
 * @param value
 */
void SIConfig::setNextPH3(QList<SI_DEFINEMESSAGE> *value)
{
    nextPH3 = value;
}

/**
 * @brief SIConfig::InitParameter
 */
void SIConfig::InitParameter()
{
    QLogHelper::instance()->LogInfo("SIConfig->InitParameter() 函数执行!");
    entryAVM1=new QList<SI_DEFINEMESSAGE>();
    entryAVM2=new QList<SI_DEFINEMESSAGE>();
    ipa=new QList<SI_DEFINEMESSAGE>();
    nextPH3=new QList<SI_DEFINEMESSAGE>();
}
/**
 * @brief SIConfig::Init
 */
void SIConfig::Init()
{
    QLogHelper::instance()->LogInfo("SIConfig->Init() 函数执行!");
    //ConfigFile=QString("%1/%2").arg(QApplication::applicationDirPath()).arg(ConfigFile);
    //if(!QFile::exists(ConfigFile)){this->NewConfig(ConfigFile);}

}

/**
 * @brief SIConfig::ReadConfig
 * @param filePath
 * @param flag
 */
void SIConfig::ReadConfig(const QString filePath, const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIConfig->ReadConfig() 函数执行!");
}

/**
 * @brief SIConfig::NewConfig
 * @param filePath
 */
void SIConfig::NewConfig(const QString filePath)
{
    QLogHelper::instance()->LogInfo("SIConfig->NewConfig() 函数执行!");

    QSettings set(filePath, QSettings::IniFormat);

    set.beginGroup("TcpClientConfig");
    set.setValue("HexSendTcpClient",2);
    set.setValue("HexReceiveTcpClient", 2);
    set.setValue("DebugTcpClient", 3);
    set.setValue("AutoSendTcpClient", 4);
    set.setValue("IntervalTcpClient", 5);
    set.setValue("TcpServerIP", 6);
    set.setValue("TcpServerPort", 7);
    set.endGroup();
}



