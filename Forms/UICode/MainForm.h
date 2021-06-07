#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>

#include "COMMONDEFINE.h"
#include "MainFormBean.h"
#include "MainFormMethod.h"
#include "CommonMethod.h"


namespace Ui {
class MainForm;
}

class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();


private:
    Ui::MainForm *ui;

    MainFormBean *mainFormBean;

    MainFormMethod *mainFormMethod;

    void InitStytle();

    void Init();

    void BetweenUI_Init();

    void ConnectSlot();
};

#endif // MAINFORM_H
