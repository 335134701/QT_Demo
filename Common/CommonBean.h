#ifndef COMMONBEAN_H
#define COMMONBEAN_H

#include <QObject>
#include <QStandardPaths>
#include <QDateTime>
#include "QLoghelper.h"
#include "CommonMethod.h"
#include "XMLOperate.h"
#include "ExcelOperation.h"

#define DATETIME                QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
#define DATAStytle              "[ "+DATETIME+" ] "
#define IDRelyID                "IDRelyID"
#define RelyFileError           "RelyFileError"
#define SVNDirError             "SVNDirError"
#define IniFileError            "IniFileError"
#define APPFileError            "APPFileError"
#define JoinFileError           "JoinFileError"
#define PFileError              "PFileError"
#define SWFileError             "SWFileError"
#define CarInfoFileError        "CarInfoFileError"
#define CarMapFileError         "CarMapFileError"
#define CarOSDFileError         "CarOSDFileError"



class CommonBean : public QObject
{
    Q_OBJECT

public:
    explicit CommonBean(QObject *parent = nullptr);

    const QString desktopDirPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    const QString applicationDirPath=QApplication::applicationDirPath();

    const QString exampleDirPath=QApplication::applicationDirPath()+"/Example";
    //一些特殊参数 初始化方法
    void ParameterInit();
    //错误码初始化
    bool ErrorCodeInit();

    QString getRelyFilePath() const;
    void setRelyFilePath(const QString &value);

    XMLOperate *getXmlOperate() const;
    void setXmlOperate(XMLOperate *value);

    QMap<QString, QString> getRExpression() const;

    QString getIniFilePath() const;
    void setIniFilePath(const QString &value);

    ExcelOperation *getExcelOption() const;
    void setExcelOption(ExcelOperation *value);

    QMap<QString, ERRCODETYPE> *getErrCode() const;
    void setErrCode(QMap<QString, ERRCODETYPE> *value);

    QString *getID() const;
    void setID(QString *value);

    QString *getRelyID() const;
    void setRelyID(QString *value);

    QString *getOutputDirPath() const;
    void setOutputDirPath(QString *value);

    CommonMethod *getComMethod() const;
    void setComMethod(CommonMethod *value);

    QString getPFilePath() const;
    void setPFilePath(const QString &value);

    QString getSWFilePath() const;
    void setSWFilePath(const QString &value);

    QString getCarInfoFilePath() const;
    void setCarInfoFilePath(const QString &value);

    QString getCarMapFilePath() const;
    void setCarMapFilePath(const QString &value);

    QString getCarOSDFilePath() const;
    void setCarOSDFilePath(const QString &value);

    QString *getSVNDirPath() const;
    void setSVNDirPath(QString *value);

    QString *getIDType() const;
    void setIDType(QString *value);

    QString *getRelyIDType() const;
    void setRelyIDType(QString *value);

    unsigned int getStatusflag() const;
    void setStatusflag(unsigned int value);

private :
    unsigned int statusflag;

    QMap<QString,QString> RExpression;
    //错误状态码集合
    QMap<QString,ERRCODETYPE> *errCode;
    //机种番号
    QString *ID;
    //依赖机种番号
    QString *RelyID;
    //SVN路径
    QString *SVNDirPath;
    //生成文件路径
    QString *OutputDirPath;
    //机种类型
    QString *IDType;
    //依赖机种类型
    QString *RelyIDType;
    //依赖文件路径
    QString RelyFilePath;
    //ini文件路径
    QString IniFilePath;
    //P票相关路径
    QString PFilePath;
    //SW确认表路径
    QString SWFilePath;
    //CarInfo路径
    QString CarInfoFilePath;
    //CarMap路径
    QString CarMapFilePath;
    //CarOSD路径
    QString CarOSDFilePath;
    //公共的方法类对象
    CommonMethod *comMethod;
    //处理XML的对象
    XMLOperate *xmlOperate;
    //Excel操作函数
    ExcelOperation *excelOption;
    //初始化函数
    void Init();

};

#endif // COMMONBEAN_H
