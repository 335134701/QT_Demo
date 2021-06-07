#ifndef SIFORMBEAN_H
#define SIFORMBEAN_H

#include <QObject>
#include <QList>

#include "COMMONDEFINE.h"
#include "CommonMethod.h"

/**
 * @def
 * @brief The SI_CheckMessage enum
 */
enum SI_CheckMessage{
    SI_CHECKMESSAGE_FileSearch          =       1,
    SI_CHECKMESSAGE_Pretreatment        =       2,
    SI_CHECKMESSAGE_FileCompression     =       3
};

/**
 * @def 程序执行状态标记
 * @brief The SI_Task_Status enum
 */
enum SI_Task_Status{
    SI_READY                =       0,
    SI_SVNUPDATE            =       1,
    SI_FILESEARCH           =       2,
    SI_FILEREAD             =       3,
    SI_PRETREAMENT          =       4,
    SI_FILEUNZIP            =       5,
    SI_FILECODECOPY         =       6,
    SI_FILCHECKBA           =       7,
    SI_FILCHECKCL           =       8,
    SI_FILCOMPRESSION       =       9,
    SI_FILEZIP              =       10
};

typedef struct SI_SoftNumberTable{
    QString ModelNumber;            //クラリオン機種番号
    QString CarModels;              //車種仕向け
    QString PartNumber;             //日産部番
    QString CANGen;                 //CAN世代
    QString Productionstage;        //生産段階
    QString ApplicationPartNo;      //Application PartsNo
    QString ApplicationVer;         //Application Ver
    QString CarInfoPartNo;          //Car_Info PartsNo
    QString CarInfoVer;             //Car_Info Ver
}SI_SOFTNUMBERTable;

typedef struct SI_ErrorTable{
    QString fileName;
    QString sheetName;
    unsigned int row;
    unsigned int col;
    QString errMessage;
}SI_ERRORTable;

typedef struct SI_DefineMessage{
    QString stageName;
    QString defineName;
    bool isUse;
}SI_DEFINEMESSAGE;

class SIFormBean : public QObject
{
    Q_OBJECT

#define errFontColor "color:red;"
#define nomFontColor "color:white;"

#define SILibName       "Tool"
#define SILibPW         "windows-2b21230302c2ee006fb56d68a1s2sbic"

#define ProjectName     "SH7766_PF"
#define ProducTionstage "AKM対応"
#define SUMBAT          "SumAddCP.bat"
#define JOINBAT         "joinmot.bat"

public:
    explicit SIFormBean(QObject *parent = nullptr);

    unsigned int getSIStatus() const;
    void setSIStatus(unsigned int value);

    QString *getID() const;
    void setID(QString *value);

    QString *getIDType() const;
    void setIDType(QString *value);

    QString *getRelyID() const;
    void setRelyID(QString *value);

    QString *getRelyIDType() const;
    void setRelyIDType(QString *value);

    QString *getSVNDirPath() const;
    void setSVNDirPath(QString *value);

    QString *getOutputDirPath() const;
    void setOutputDirPath(QString *value);

    QString *getRelyFilePath() const;
    void setRelyFilePath(QString *value);

    QString *getPFilePath() const;
    void setPFilePath(QString *value);

    QString *getSWFilePath() const;
    void setSWFilePath(QString *value);

    QString *getCarInfoFilePath() const;
    void setCarInfoFilePath(QString *value);

    bool getSVNUpdateStatus() const;
    void setSVNUpdateStatus(bool value);

    bool getIDRelyIDflag() const;
    void setIDRelyIDflag(bool value);

    CommonMethod *getCommonMethod() const;
    void setCommonMethod(CommonMethod *value);

    QString *getCodeFilePath() const;
    void setCodeFilePath(QString *value);

    QString *getBuildFilePath() const;
    void setBuildFilePath(QString *value);

    QString *getSHDefineFilePath() const;
    void setSHDefineFilePath(QString *value);

    QList<SI_ERRORTable> *getErrList() const;
    void setErrList(QList<SI_ERRORTable> *value);

    QList<SI_SOFTNUMBERTable> *getSoftList() const;
    void setSoftList(QList<SI_SOFTNUMBERTable> *value);

    QList<SI_DEFINEMESSAGE> *getDefineList() const;
    void setDefineList(QList<SI_DEFINEMESSAGE> *value);

    bool getBAflag() const;
    void setBAflag(bool value);

    bool getUnzipflag() const;
    void setUnzipflag(bool value);

    bool getIsSearchRelyIDflag() const;
    void setIsSearchRelyIDflag(bool value);

    bool getCopyCodeflag() const;
    void setCopyCodeflag(bool value);

    bool getJoinMotflag() const;
    void setJoinMotflag(bool value);

    bool getZIPflag() const;
    void setZIPflag(bool value);

    void ResetParameter(unsigned int flag);

    QList<SI_SOFTNUMBERTable> *getRelyIDSoftList() const;
    void setRelyIDSoftList(QList<SI_SOFTNUMBERTable> *value);

    bool getNEWCLflag() const;
    void setNEWCLflag(bool value);

signals:

public slots:

private:

    //SI程序执行状态
    unsigned int SIStatus;
    //机种番号
    QString *ID;
    //机种类型
    QString *IDType;
    //依赖机种番号
    QString *RelyID;
    //依赖机种类型
    QString *RelyIDType;
    //判断ID与依赖ID是否同一种类型
    bool IDRelyIDflag;
    //SVN路径
    QString *SVNDirPath;
    //SVN更新状态
    bool SVNUpdateStatus;
    //生成文件路径
    QString *OutputDirPath;
    //依赖文件路径
    QString *RelyFilePath;
    //项目源码路径
    QString *CodeFilePath;
    //P票模板路径
    QString *PFilePath;
    //SW确认表模板路径
    QString *SWFilePath;
    //Build文件路径
    QString *BuildFilePath;
    //CarInfo路径
    QString *CarInfoFilePath;
    //SH7766_コンパイルSW一覧表路径
    QString *SHDefineFilePath;
    //Before After路径标记
    bool BAflag;
    //项目源码解压状态
    bool Unzipflag;
    //量产管理表中对应机种相关信息
    QList<SI_SOFTNUMBERTable> *SoftList;
    QList<SI_SOFTNUMBERTable> *RelyIDSoftList;
    //错误消息集合
    QList<SI_ERRORTable> *ErrList;
    //宏定义集合
    QList<SI_DEFINEMESSAGE> *DefineList;
    //留用关系程序自找标记位
    bool isSearchRelyIDflag;
    //项目源码复制标记位
    bool CopyCodeflag;
    //项目源码处理后压缩状态标记位
    bool ZIPflag;
    //新规作成标记位
    bool NEWCLflag;

    //公共方法对象
    CommonMethod *commonMethod;

    void Init();
};

#endif // SIFORMBEAN_H
