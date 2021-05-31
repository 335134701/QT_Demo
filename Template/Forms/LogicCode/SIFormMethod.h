#ifndef SIFORMMETHOD_H
#define SIFORMMETHOD_H

#include <QObject>
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

    void UpdateSVNSignal(const QString exeFilePath, const QString dirPath);

    void SearchFileSignal(unsigned int flag, bool isGoON);

    void FileSearchSignal(const QString dirPath,const QStringList filters,const QString ID,QString IDType,unsigned int flag, bool isGoON);

public slots:

    void JudgeIDSlot(QLineEdit *Edit,QString *ID);

    void JudgeIDTypeSlot(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowMessageProcessSlot(const unsigned int flag, const unsigned int Log_Flag);

    void UpdateSVNSlot();

    void EndUpdateSVNSlot(const bool result);

    void SelectDirSlot(QLabel *label,QString *objectDir);

    void SearchFileSlot(unsigned int flag, bool isGoON);

    void EndFileSearcSlot(const QString filePath,unsigned int flag, bool isGoON);

private :

    QTableView *tableView;

    SIFormBean *siFormBean;

    SICommonMethod *siCommonMethod;

    LogFormMetod *logFormMethod;

    QThread *fileThread;

    QThread *excelThread;

    SIExcelOperateThread *excelOperateThread;

    SIFileOperateThread *fileOperateThread;

    //任务执行状态标记位
    unsigned int TmpProcessStatus;

    void Init();

    void ConnectSlot();
};

#endif // SIFORMMETHOD_H
