#ifndef SIFORMMETHOD_H
#define SIFORMMETHOD_H

#include <QObject>
#include <QLineEdit>
#include "COMMONDEFINE.h"
#include "SIFormBean.h"
#include "LogFormMetod.h"
#include "CommonMethod.h"
#include "ExcelOperateThread.h"
#include "FileOperateThread.h"

class SIFormMethod : public QObject
{
    Q_OBJECT
public:
    explicit SIFormMethod(QObject *parent = nullptr);

    SIFormBean *getSiFormBean() const;
    void setSiFormBean(SIFormBean *value);

    LogFormMetod *getLogFormMethod() const;
    void setLogFormMethod(LogFormMetod *value);

    void ConnectOtherUISlot();

signals:
    void ShowLogMessageSignal(const QStringList message,const unsigned int level);

    void ShowMessageProcessSignal(const unsigned int flag);

public slots:

    void JudgeIDSlot(QLineEdit *Edit,QString *ID);

    void JudgeIDTypeSlot(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowMessageProcessSlot(const unsigned int flag);

private :

    SIFormBean *siFormBean;

    LogFormMetod *logFormMethod;

    ExcelOperateThread *excelOperateThread;

    FileOperateThread *fileOperateThread;

    void Init();

    void ConnectSlot();
};

#endif // SIFORMMETHOD_H
