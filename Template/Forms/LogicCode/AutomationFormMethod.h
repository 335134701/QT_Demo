#ifndef AUTOMATIONFORMMETHOD_H
#define AUTOMATIONFORMMETHOD_H

#include <QObject>


#include "COMMONDEFINE.h"
#include "LogFormMetod.h"
#include "AutomationFormBean.h"

class AutomationFormMethod : public QObject
{
    Q_OBJECT
public:
    explicit AutomationFormMethod(QObject *parent = nullptr);


    AutomationFormBean *getAutomationFormBean() const;
    void setAutomationFormBean(AutomationFormBean *value);

    LogFormMetod *getLogFormMethod() const;
    void setLogFormMethod(LogFormMetod *value);


    void ConnectOtherUISlot();

signals:

public slots:

private :

    AutomationFormBean *automationFormBean;

    LogFormMetod *logFormMethod;

    void Init();

    void ConnectSlot();
};

#endif // AUTOMATIONFORMMETHOD_H
