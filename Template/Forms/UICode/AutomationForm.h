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


signals:

    void JudgeIDSignal(QLineEdit *Edit,QString *ID);

    void JudgeIDTypeSignal(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowMessageProcessSignal(const unsigned int flag, const unsigned int Log_Flag);

    void SelectDirSignal(QLabel *label,QString *objectDir);

    void SearchFileSignal(unsigned int flag, bool isGoON);

public slots:

private slots:

    void on_AuIDEdit_editingFinished();

    void on_AuRelyIDEdit_editingFinished();

    void on_AuSVNButton_clicked();

    void on_AuOutputButton_clicked();

    void on_AuFileSearchButton_clicked();

    void on_AuCreateButton_clicked();

private:
    Ui::AutomationForm *ui;

    AutomationFormBean *automationFormBean;

    AutomationFormMethod *automationFormMethod;

    LogFormMetod *logFormMethod;


    void InitStytle();

    void Init();

    void ConnectSlot();

    bool PromptInformation();

    bool CheckMessage(const unsigned int flag);
};

#endif // AUTOMATIONFORM_H
