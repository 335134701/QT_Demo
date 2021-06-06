#ifndef AUTOMATIONFORMBEAN_H
#define AUTOMATIONFORMBEAN_H

#include <QObject>

#include "COMMONDEFINE.h"
#include "CommonMethod.h"


/**
 * @def
 * @brief The SI_CheckMessage enum
 */
enum AU_CheckMessage{
    AU_CHECKMESSAGE_FileSearch          =       1,
    AU_CHECKMESSAGE_FileCreate          =       2
};

/**
 * @def 程序执行状态标记
 * @brief The SI_Task_Status enum
 */
enum Au_Task_Status{
    AU_READY        =       0,
    AU_SVNUPDATE    =       1,
};

class AutomationFormBean : public QObject
{
    Q_OBJECT

#define errFontColor "color:red;"
#define nomFontColor "color:white;"

public:
    explicit AutomationFormBean(QObject *parent = nullptr);

    QStringList getDefineConfigList() const;
    void setDefineConfigList(const QStringList &value);

    QString *getID() const;
    void setID(QString *value);

    QString *getRelyID() const;
    void setRelyID(QString *value);

    bool getIDRelyIDflag() const;
    void setIDRelyIDflag(bool value);

    QString *getSVNDirPath() const;
    void setSVNDirPath(QString *value);

    QString *getOutputDirPath() const;
    void setOutputDirPath(QString *value);

    QString *getIDType() const;
    void setIDType(QString *value);

    QString *getRelyIDType() const;
    void setRelyIDType(QString *value);

    QString *getRelyFilePath() const;
    void setRelyFilePath(QString *value);

    QString *getIniFilePath() const;
    void setIniFilePath(QString *value);

    QString *getPFilePath() const;
    void setPFilePath(QString *value);

    QString *getSWFilePath() const;
    void setSWFilePath(QString *value);

    QString *getCarInfoFilePath() const;
    void setCarInfoFilePath(QString *value);

    QString *getCarMapFilePath() const;
    void setCarMapFilePath(QString *value);

    QString *getCarOSDFilePath() const;
    void setCarOSDFilePath(QString *value);

    QString *getJoinFileMot() const;
    void setJoinFileMot(QString *value);

    QString *getAPPFileMot() const;
    void setAPPFileMot(QString *value);

    QString *getEEFilePath() const;
    void setEEFilePath(QString *value);

    QString *getReadyFilePath() const;
    void setReadyFilePath(QString *value);

    QString *getConfigFilePath() const;
    void setConfigFilePath(QString *value);

    CommonMethod *getComMethod() const;
    void setComMethod(CommonMethod *value);

    QString *getZIPFilePath() const;
    void setZIPFilePath(QString *value);

    void ResetParameter(unsigned int flag);

    CommonMethod *getCommonMethod() const;
    void setCommonMethod(CommonMethod *value);

    unsigned int getAumationStatus() const;
    void setAumationStatus(unsigned int value);

signals:

public slots:

private :

    //SI程序执行状态
    unsigned int AumationStatus;
    //宏定义集合
    QStringList DefineConfigList;
    //机种番号
    QString *ID;
    //依赖机种番号
    QString *RelyID;
    //判断ID与依赖ID是否同一种类型
    bool IDRelyIDflag;
    //SVN路径
    QString *SVNDirPath;
    //生成文件路径
    QString *OutputDirPath;
    //机种类型
    QString *IDType;
    //依赖机种类型
    QString *RelyIDType;
    //依赖文件路径
    QString *RelyFilePath;
    //ini文件路径
    QString *IniFilePath;
    //P票相关路径
    QString *PFilePath;
    //SW确认表路径
    QString *SWFilePath;
    //CarInfo路径
    QString *CarInfoFilePath;
    //CarMap路径
    QString *CarMapFilePath;
    //CarOSD路径
    QString *CarOSDFilePath;
    //joinMot路径
    QString *JoinFileMot;
    //APPMot路径
    QString *APPFileMot;
    //EE模板文件路径
    QString *EEFilePath;
    //确认文件路径
    QString *ReadyFilePath;
    //EntryAVM採用車種コンフィグ詳細
    QString *ConfigFilePath;
    //7z生成压缩文件路径
    QString *ZIPFilePath;

    //公共方法对象
    CommonMethod *commonMethod;

    void Init();
};

#endif // AUTOMATIONFORMBEAN_H
