#ifndef SIFORMBEAN_H
#define SIFORMBEAN_H

#include <QObject>

#include "COMMONDEFINE.h"
#include "CommonMethod.h"

/**
 * @def 初始化标记位，根据标记位，初始化相关参数
 * @brief The RestParameter enum
 */
enum RestParameter{
    RET_ID                  =   1,
    RET_SVNFilePath         =   2,
    RET_OutPutFilePath      =   3
};

/**
 * @def 标记位，Log 全部显示，Log输出显示，Table显示
 * @brief The Log_Flag enum
 */
enum Log_Flag{
    LOG_ALL     =   1,
    LOG_LOG     =   2,
    LOG_TABLE   =   3
};

class SIFormBean : public QObject
{
    Q_OBJECT

#define errFontColor "color:red;"
#define nomFontColor "color:white;"

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

    void ResetParameter(unsigned int flag);

    bool getIDRelyIDflag() const;
    void setIDRelyIDflag(bool value);

    CommonMethod *getCommonMethod() const;
    void setCommonMethod(CommonMethod *value);

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
    //P票相关路径
    QString *PFilePath;
    //SW确认表路径
    QString *SWFilePath;
    //CarInfo路径
    QString *CarInfoFilePath;


    //公共方法对象
    CommonMethod *commonMethod;

    void Init();
};

#endif // SIFORMBEAN_H
