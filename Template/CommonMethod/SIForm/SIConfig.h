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

    QList<SI_DEFINEMESSAGE> *getNextPH3() const;
    void setNextPH3(QList<SI_DEFINEMESSAGE> *value);

    QList<SI_DEFINEMESSAGE> *getEntryIPA() const;
    void setEntryIPA(QList<SI_DEFINEMESSAGE> *value);

signals:

public slots:

private:

    //EntryAVM宏定义
    QList<SI_DEFINEMESSAGE> *entryAVM1;

    //EntryAVM2宏定义
    QList<SI_DEFINEMESSAGE> *entryAVM2;

    //IPA宏定义
    QList<SI_DEFINEMESSAGE> *entryIPA;

    //NextPH3宏定义
    QList<SI_DEFINEMESSAGE> *nextPH3;

    void Init();

    void SetEntryAVM();
    void SetEntryAVM2();
    void SetEntryIPA();
    void SetNextPH3();
};

#endif // SICONFIG_H
