#ifndef AUTOMATIONFORMMETHOD_H
#define AUTOMATIONFORMMETHOD_H

#include <QObject>
#include <QLabel>
#include <QLineEdit>
#include <QTableView>
#include <QFileDialog>

#include "COMMONDEFINE.h"
#include "LogFormMetod.h"
#include "AutomationFormBean.h"
#include "AuExcelOperateThread.h"
#include "AuFileOperateThread.h"

class AutomationFormMethod : public QObject
{
    Q_OBJECT
public:
    explicit AutomationFormMethod(QObject *parent = nullptr);


    QTableView *getTableView() const;
    void setTableView(QTableView *value);

    AutomationFormBean *getAutomationFormBean() const;
    void setAutomationFormBean(AutomationFormBean *value);

    LogFormMetod *getLogFormMethod() const;
    void setLogFormMethod(LogFormMetod *value);

    void ConnectOtherUISlot();

    void InitTableView();

    void ShowTableView(const QStringList message, const unsigned int flag);

signals:

    void ShowLogMessageSignal(const QStringList message,const unsigned int level);

    void ShowMessageProcessSignal(const unsigned int flag, const unsigned int Log_Flag);

public slots:

    void JudgeIDSlot(QLineEdit *Edit, QString *ID);

    void JudgeIDTypeSlot(QLineEdit *Edit, QString *srcobject, QString *desobject);

    void ShowMessageProcessSlot(const unsigned int flag, const unsigned int Log_Flag);

    void SelectDirSlot(QLabel *label, QString *objectDir);

    void SearchFileSlot(unsigned int flag, bool isGoON);

private :

    QTableView *tableView;

    AutomationFormBean *automationFormBean;

    LogFormMetod *logFormMethod;

    AuExcelOperateThread *excelOperateThread;

    AuFileOperateThread *fileOperateThread;

    void Init();

    void ConnectSlot();
};

#endif // AUTOMATIONFORMMETHOD_H
