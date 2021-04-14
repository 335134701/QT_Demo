#ifndef COMMONBEAN_H
#define COMMONBEAN_H

#include <QObject>
#include <QStandardPaths>
#include <QDateTime>
#include "QLoghelper.h"
#include "CommonMethod.h"
#include "XMLOperate.h"


class CommonBean : public QObject
{
    Q_OBJECT

    #define DATETIME                QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
    #define DATAStytle              "[ "+DATETIME+" ] "
    #define IDRelyID                "IDRelyID"
    #define ExampleDirError         "ExampleDirError"
    #define RelyFileError           "RelyFileError"
    #define IniFileError            "IniFileError"


public:
    explicit CommonBean(QObject *parent = nullptr);

    const QString desktopDirPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    const QString applicationDirPath=QApplication::applicationDirPath();

    const QString exampleDirPath=QApplication::applicationDirPath()+"/Example";
    //一些特殊参数 初始化方法
    void ParameterInit();
    //错误码初始化
    bool ErrorCodeInit();
    //机种番号
    QString ID;
    //依赖机种番号
    QString RelyID;
    //成果物路径
    QString ResultDirPath;
    //Mot文件路径
    QString MotDirPath;
    //生成文件路径
    QString OutputDirPath;
    //错误状态码集合
    QMap<QString,ERRCODETYPE> errCode;

    QString getIDType() const;
    void setIDType(const QString &value);

    QString getRelyFilePath() const;
    void setRelyFilePath(const QString &value);

    CommonMethod *getComMethod() const;
    void setComMethod(CommonMethod *value);

    XMLOperate *getXmlOperate() const;
    void setXmlOperate(XMLOperate *value);

    QMap<QString, QString> getRExpression() const;

    QString getRelyIDType() const;
    void setRelyIDType(const QString &value);

    QString getIniFilePath() const;
    void setIniFilePath(const QString &value);

    QString getOutputType() const;
    void setOutputType(const QString &value);

private :
    QMap<QString,QString> RExpression;
    //机种类型
    QString IDType;
    //依赖机种类型
    QString RelyIDType;
    //依赖文件路径
    QString RelyFilePath;
    //ini文件路径
    QString IniFilePath;
    //生成文件类型
    QString OutputType;
    //公共的方法类对象
    CommonMethod *comMethod;
    //处理XML的对象
    XMLOperate *xmlOperate;
    //初始化函数
    void Init();

};

#endif // COMMONBEAN_H
