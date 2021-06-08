#include "SIConfig.h"

/**
 * @brief SIConfig::SIConfig
 * @param parent
 */
SIConfig::SIConfig(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIConfig 构造函数执行!");
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

QList<SI_DEFINEMESSAGE> *SIConfig::getEntryIPA() const
{
    return entryIPA;
}

void SIConfig::setEntryIPA(QList<SI_DEFINEMESSAGE> *value)
{
    entryIPA = value;
}

/**
 * @brief SIConfig::Init
 */
void SIConfig::Init()
{
    QLogHelper::instance()->LogInfo("SIConfig->Init() 函数执行!");
    this->SetEntryAVM();
    this->SetEntryAVM2();
    this->SetEntryIPA();
    this->SetNextPH3();
}

/**
 * @brief SIConfig::SetEntryAVM
 */
void SIConfig::SetEntryAVM()
{
    QLogHelper::instance()->LogInfo("SIConfig->SetEntryAVM() 函数执行!");
    entryAVM1=new QList<SI_DEFINEMESSAGE>();
}

/**
 * @brief SIConfig::SetEntryAVM2
 */
void SIConfig::SetEntryAVM2()
{
    QLogHelper::instance()->LogInfo("SIConfig->SetEntryAVM2() 函数执行!");
    entryAVM2=new QList<SI_DEFINEMESSAGE>();
    //entryAVM2->append(new SI_DEFINEMESSAGE{});
}

/**
 * @brief SIConfig::SetEntryIPA
 */
void SIConfig::SetEntryIPA()
{
    QLogHelper::instance()->LogInfo("SIConfig->SetEntryIPA() 函数执行!");
    entryIPA=new QList<SI_DEFINEMESSAGE>();
}

/**
 * @brief SIConfig::SetNextPH3
 */
void SIConfig::SetNextPH3()
{
    QLogHelper::instance()->LogInfo("SIConfig->SetNextPH3() 函数执行!");
    nextPH3=new QList<SI_DEFINEMESSAGE>();
}


