#ifndef AUTOMATIONFORM_H
#define AUTOMATIONFORM_H

#include <QWidget>


#include "COMMONDEFINE.h"

namespace Ui {
class AutomationForm;
}

class AutomationForm : public QWidget
{
    Q_OBJECT

public:
    explicit AutomationForm(QWidget *parent = 0);
    ~AutomationForm();

private:
    Ui::AutomationForm *ui;


    void InitStytle();

    void Init();

    void ConnectSlot();

    bool PromptInformation();
};

#endif // AUTOMATIONFORM_H
