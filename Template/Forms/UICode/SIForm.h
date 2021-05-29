#ifndef SIFORM_H
#define SIFORM_H

#include <QWidget>


#include "COMMONDEFINE.h"
#include "SIFormBean.h"
#include "SIFormMethod.h"
#include "LogFormMetod.h"
#include "CommonMethod.h"

namespace Ui {
class SIForm;
}

class SIForm : public QWidget
{
    Q_OBJECT

public:
    explicit SIForm(QWidget *parent = 0);
    ~SIForm();

    LogFormMetod *getLogFormMethod() const;
    void setLogFormMethod(LogFormMetod *value);

    SIFormMethod *getSiFormMethod() const;
    void setSiFormMethod(SIFormMethod *value);

    SIFormBean *getSiFormBean() const;
    void setSiFormBean(SIFormBean *value);

signals:

    void JudgeIDSignal(QLineEdit *Edit,QString *ID);

    void JudgeIDTypeSignal(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowMessageProcessSignal(const unsigned int flag);

private slots:

    void on_SVNButton_clicked();

    void on_IDEdit_editingFinished();

    void on_RelyIDEdit_editingFinished();

    void on_OutputButton_clicked();

private:
    Ui::SIForm *ui;

    SIFormBean *siFormBean;

    SIFormMethod *siFormMethod;

    LogFormMetod *logFormMethod;

    void Init();

    void ConnectSlot();
    
    bool PromptInformation();
};

#endif // SIFORM_H
