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

    void ShowMessageProcessSignal(const unsigned int flag, const unsigned int Log_Flag);

    void UpdateSVNSignal();

    void SelectDirSignal(QLabel *label,QString *objectDir);

    void SearchFileSignal(unsigned int flag, bool isGoON);


private slots:

    void on_SIIDEdit_editingFinished();

    void on_SIRelyIDEdit_editingFinished();

    void on_SISVNButton_clicked();

    void on_SIOutputButton_clicked();

    void on_SIFileSearchButton_clicked();

    void on_SIPretreatmentButton_clicked();

    void on_SIFileCompressionButton_clicked();

private:
    Ui::SIForm *ui;

    SIFormBean *siFormBean;

    SIFormMethod *siFormMethod;

    LogFormMetod *logFormMethod;

    void InitStytle();

    void Init();

    void ConnectSlot();

    bool PromptInformation();

    bool CheckMessage(const unsigned int flag);
};

#endif // SIFORM_H
