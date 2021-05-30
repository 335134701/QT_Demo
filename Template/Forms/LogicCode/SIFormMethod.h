#ifndef SIFORMMETHOD_H
#define SIFORMMETHOD_H

#include <QObject>
#include <QLineEdit>
#include <QTableView>
#include <QFileDialog>
#include <QLabel>
#include "COMMONDEFINE.h"
#include "SIFormBean.h"
#include "LogFormMetod.h"
#include "CommonMethod.h"
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

public slots:

    void JudgeIDSlot(QLineEdit *Edit,QString *ID);

    void JudgeIDTypeSlot(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowMessageProcessSlot(const unsigned int flag, const unsigned int Log_Flag);

    void SelectDirSlot(QLabel *label,QString *objectDir);

    void SearchFileSlot(unsigned int flag, bool isGoON);

private :

    QTableView *tableView;

    SIFormBean *siFormBean;

    LogFormMetod *logFormMethod;

    SIExcelOperateThread *excelOperateThread;

    SIFileOperateThread *fileOperateThread;

    void Init();

    void ConnectSlot();
};

#endif // SIFORMMETHOD_H
