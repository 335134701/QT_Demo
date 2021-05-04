#ifndef COMMONBEAN_H
#define COMMONBEAN_H

#include <QObject>
#include <QStandardPaths>
#include <QDateTime>
#include <QStandardItem>
#include "QLoghelper.h"
#include "CommonMethod.h"
#include "XMLOperate.h"
#include "ExcelOperation.h"
#include "DEFINE.h"




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

    QMap<QString, QString> getRExpression() const;
    void setRExpression(const QMap<QString, QString> &value);

    QMap<QString, ERRCODETYPE> *getErrCode() const;
    void setErrCode(QMap<QString, ERRCODETYPE> *value);

    QString *getID() const;
    void setID(QString *value);

    QString *getRelyID() const;
    void setRelyID(QString *value);

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

    QString *getJoinMot() const;
    void setJoinMot(QString *value);

    QString *getAPPMot() const;
    void setAPPMot(QString *value);

    CommonMethod *getComMethod() const;
    void setComMethod(CommonMethod *value);

    XMLOperate *getXmlOperate() const;
    void setXmlOperate(XMLOperate *value);

    unsigned int getStatusflag() const;
    void setStatusflag(unsigned int value);

    ExcelOperation *getExcelOption() const;
    void setExcelOption(ExcelOperation *value);

    QString *getEEFilePath() const;
    void setEEFilePath(QString *value);
    QString *getReadyFilePath() const;
    void setReadyFilePath(QString *value);

    QString *getConfigFilePath() const;
    void setConfigFilePath(QString *value);

    QList<SOFTNUMBERTable> *getSoftNumberTable() const;
    void setSoftNumberTable(QList<SOFTNUMBERTable> *value);

    QList<CONFIGTable> *getConfigTable() const;
    void setConfigTable(QList<CONFIGTable> *value);

    QStandardItemModel *getMessageViewModel() const;
    void setMessageViewModel(QStandardItemModel *value);

private :
    unsigned int statusflag;

    //TableView数据模型
    QStandardItemModel *messageViewModel;

    QMap<QString,QString> RExpression;
    //错误状态码集合
    QMap<QString,ERRCODETYPE> *errCode;
    //ソフトウエア部品番号管理表(量産)_AKM対応用解析集合
    QList<SOFTNUMBERTable> *softNumberTable;
    //採用車種コンフィグ詳細文件解析集合
    QList<CONFIGTable> *configTable;
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
    QString *JoinMot;
    //APPMot路径
    QString *APPMot;
    //EE模板文件路径
    QString *EEFilePath;
    //确认文件路径
    QString *ReadyFilePath;
    //EntryAVM採用車種コンフィグ詳細
    QString *ConfigFilePath;
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
