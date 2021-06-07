#ifndef SICONFIG_H
#define SICONFIG_H

#include <QObject>
#include <QSettings>
#include "COMMONDEFINE.h"
#include "SIFormBean.h"

class SIConfig : public QObject
{
    Q_OBJECT
public:
    explicit SIConfig(QObject *parent = nullptr);

    //QString ConfigFile="Tool.ini";

    QList<SI_DEFINEMESSAGE> *getEntryAVM1() const;
    void setEntryAVM1(QList<SI_DEFINEMESSAGE> *value);

    QList<SI_DEFINEMESSAGE> *getEntryAVM2() const;
    void setEntryAVM2(QList<SI_DEFINEMESSAGE> *value);

    QList<SI_DEFINEMESSAGE> *getIpa() const;
    void setIpa(QList<SI_DEFINEMESSAGE> *value);

    QList<SI_DEFINEMESSAGE> *getNextPH3() const;
    void setNextPH3(QList<SI_DEFINEMESSAGE> *value);

    //读写配置参数及其他操作
    void ReadConfig(const QString filePath,const unsigned int flag);           //读取配置参数
    void NewConfig(const QString filePath);            //以初始值新建配置文件



signals:

public slots:

private:

    //EntryAVM宏定义
    QList<SI_DEFINEMESSAGE> *entryAVM1;

    //EntryAVM2宏定义
    QList<SI_DEFINEMESSAGE> *entryAVM2;

    //IPA宏定义
    QList<SI_DEFINEMESSAGE> *ipa;

    //NextPH3宏定义
    QList<SI_DEFINEMESSAGE> *nextPH3;

    void InitParameter();

    void Init();
};

#endif // SICONFIG_H
