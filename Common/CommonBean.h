#ifndef COMMONBEAN_H
#define COMMONBEAN_H

#include <QObject>
#include <QStandardPaths>
#include "QLoghelper.h"
#include "CommonMethod.h"
#include "XMLOperate.h"


class CommonBean : public QObject
{
    Q_OBJECT

public:
    explicit CommonBean(QObject *parent = nullptr);

    const QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    const QString applicationPath=QApplication::applicationDirPath();
    //一些特殊参数 初始化方法
    void ParameterInit();

    QMap<QString, QString> getRExpression() const;

    QMap<QString, ERRCODETYPE> getErrCode() const;

    QString getID() const;
    void setID(const QString &value);

    QString getIDType() const;
    void setIDType(const QString &value);

    QString getRelyID() const;
    void setRelyID(const QString &value);

    QString getRelyFilePath() const;
    void setRelyFilePath(const QString &value);

    QString getResultDirPath() const;
    void setResultDirPath(const QString &value);

    QString getMotDirPath() const;
    void setMotDirPath(const QString &value);

    QString getOutputDirPath() const;
    void setOutputDirPath(const QString &value);

    CommonMethod *getComMethod() const;
    void setComMethod(CommonMethod *value);

    XMLOperate *getXmlOperate() const;
    void setXmlOperate(XMLOperate *value);

private :

    QMap<QString,QString> RExpression;
    //错误状态码集合
    QMap<QString,ERRCODETYPE> errCode;
    //机种番号
    QString ID;
    //机种类型
    QString IDType;
    //依赖机种番号
    QString RelyID;
    //依赖文件路径
    QString RelyFilePath;
    //成果物路径
    QString ResultDirPath;
    //Mot文件路径
    QString MotDirPath;
    //生成文件路径
    QString OutputDirPath;
    //公共的方法类对象
    CommonMethod *comMethod;
    //处理XML的对象
    XMLOperate *xmlOperate;
    //初始化函数
    void Init();

};

#endif // COMMONBEAN_H
