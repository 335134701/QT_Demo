#ifndef SIFORMMETHOD_H
#define SIFORMMETHOD_H

#include <QObject>
#include <QMetaType>
#include <QLineEdit>
#include <QTableView>
#include <QFileDialog>
#include <QLabel>
#include <QThread>

#include "COMMONDEFINE.h"
#include "SIFormBean.h"
#include "LogFormMetod.h"
#include "CommonMethod.h"
#include "SICommonMethod.h"
#include "SIExcelOperateThread.h"
#include "SIFileOperateThread.h"
#include "SIConfig.h"

class SIFormMethod : public QObject
{
    Q_OBJECT
public:
    explicit SIFormMethod(QObject *parent = nullptr);

    QTableView *getTableView() const;
    void setTableView(QTableView *value);

    SIFormBean *getSiFormBean() const;
    void setSiFormBean(SIFormBean *value);

    LogFormMetod *getLogFormMethod() const;
    void setLogFormMethod(LogFormMetod *value);

    void ConnectOtherUISlot();

    void InitTableView();

    void ShowTableView(const QStringList message, const unsigned int flag);

signals:
    void ShowLogMessageSignal(const QStringList message,const unsigned int level);

    void ShowMessageProcessSignal(const unsigned int flag, const unsigned int Log_Flag);

    void RunOrderSignal(const unsigned int flag);

    void UpdateSVNSignal(const QString exeFilePath, const QString dirPath,const unsigned flag);

    void UNZipCodeFileSignal(const QString exeFilePath, const QString filePath,const QString desDirPath,const unsigned flag);

    void SearchFileSignal(unsigned int flag, bool isGoON);

    void CopyFileSignal(const QString srcFilePath,const QString desFilePath,const unsigned int flag);

    //void CopyDirSignal(const QString srcDirPath,const QString desDirPath,const unsigned int flag);

    void FileSearchSignal(const QString dirPath,const QStringList filters,const QString ID,QString IDType,unsigned int flag, bool isGoON);

    void ReadExcelThreadSignal(const QString filePath,const QString ID,const QString IDType,const unsigned int flag);

    void CheckFileSignal(const QString dirPath,const QString ID,const QString IDType,const unsigned int flag,const QList<SI_SOFTNUMBERTable> softList);

    void PretreatmentSignal();

    void FileCompressionSignal();

public slots:

    void JudgeIDSlot(QLineEdit *Edit,QString *ID);

    void JudgeIDTypeSlot(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowMessageProcessSlot(const unsigned int flag, const unsigned int Log_Flag);

    void RunOrderSlot(const unsigned int flag);

    void EndRunOrderSlot(const bool result,const unsigned int flag);

    void SelectDirSlot(QLabel *label,QString *objectDir);

    void SearchFileSlot(unsigned int flag, bool isGoON);

    void EndCopyFileSlot(const QString filePath,const unsigned int flag, const bool result);

    //void EndCopyDirSlot(const QString dirPath,const unsigned int flag, const bool result);

    void EndFileSearcSlot(const QString filePath,unsigned int flag, bool isGoON);

    void EndReadSoftExcelSlot(const QList<SI_SOFTNUMBERTable> softList,const QList<SI_ERRORTable> errList);

    void PretreatmentSlot();

    void EndCheckFileSlot(const unsigned int flag,const bool result,const QList<SI_ERRORTable> errList);

    void EndReadDefineFileExcelSlot(const QList<SI_DEFINEMESSAGE> defineList, const QList<SI_ERRORTable> errList);

    void FileCompressionSlot();

private :

    QTableView *tableView;

    SIFormBean *siFormBean;

    SICommonMethod *siCommonMethod;

    LogFormMetod *logFormMethod;

    QThread *fileThread;

    QThread *excelThread;

    SIExcelOperateThread *excelOperateThread;

    SIFileOperateThread *fileOperateThread;

    SIConfig *siConfig;

    //任务执行状态标记位
    unsigned int TmpProcessStatus;

    void Init();

    void ConnectSlot();

    void SendConMessageLog(const QString txt,const unsigned int level);

    void RetSIStatus(const QString txt,const unsigned int level);
};

#endif // SIFORMMETHOD_H
