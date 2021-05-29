#ifndef AUTOMATIONFORM_H
#define AUTOMATIONFORM_H

#include <QWidget>


#include "COMMONDEFINE.h"
#include "LogFormMetod.h"
#include "AutomationFormBean.h"
#include "AutomationFormMethod.h"

namespace Ui {
class AutomationForm;
}

class AutomationForm : public QWidget
{
    Q_OBJECT

public:
    explicit AutomationForm(QWidget *parent = 0);
    ~AutomationForm();

    AutomationFormBean *getAutomationFormBean() const;
    void setAutomationFormBean(AutomationFormBean *value);

    AutomationFormMethod *getAutomationFormMethod() const;
    void setAutomationFormMethod(AutomationFormMethod *value);

    LogFormMetod *getLogFormMethod() const;
    void setLogFormMethod(LogFormMetod *value);

private:
    Ui::AutomationForm *ui;

    AutomationFormBean *automationFormBean;

    AutomationFormMethod *automationFormMethod;

    LogFormMetod *logFormMethod;


    void InitStytle();

    void Init();

    void ConnectSlot();

    bool PromptInformation();
};

#endif // AUTOMATIONFORM_H
