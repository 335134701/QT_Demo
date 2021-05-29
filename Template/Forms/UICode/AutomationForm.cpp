#include "AutomationForm.h"
#include "ui_AutomationForm.h"

/**
 * @brief AutomationForm::AutomationForm
 * @param parent
 */
AutomationForm::AutomationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AutomationForm)
{
    QLogHelper::instance()->LogInfo("AutomationForm() 构造函数执行!");
    ui->setupUi(this);
    this->InitStytle();
    this->Init();
    this->ConnectSlot();
}

/**
 * @brief AutomationForm::~AutomationForm
 */
AutomationForm::~AutomationForm()
{
    QLogHelper::instance()->LogInfo("AutomationForm->~AutomationForm() 函数执行!");
    delete ui;
}

AutomationFormBean *AutomationForm::getAutomationFormBean() const
{
    return automationFormBean;
}

void AutomationForm::setAutomationFormBean(AutomationFormBean *value)
{
    automationFormBean = value;
}

AutomationFormMethod *AutomationForm::getAutomationFormMethod() const
{
    return automationFormMethod;
}

void AutomationForm::setAutomationFormMethod(AutomationFormMethod *value)
{
    automationFormMethod = value;
}

LogFormMetod *AutomationForm::getLogFormMethod() const
{
    return logFormMethod;
}

void AutomationForm::setLogFormMethod(LogFormMetod *value)
{
    logFormMethod = value;
}

/**
 * @brief AutomationForm::InitStytle
 */
void AutomationForm::InitStytle()
{
    QLogHelper::instance()->LogInfo("AutomationForm->InitStytle() 函数执行!");
}

/**
 * @brief AutomationForm::Init
 */
void AutomationForm::Init()
{
    QLogHelper::instance()->LogInfo("AutomationForm->Init() 函数执行!");
    automationFormBean=new AutomationFormBean();
    automationFormMethod=new AutomationFormMethod();
    automationFormMethod->setAutomationFormBean(automationFormBean);
}

/**
 * @brief AutomationForm::ConnectSlot
 */
void AutomationForm::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("AutomationForm->ConnectSlot() 函数执行!");
}


/**
 * @brief AutomationForm::PromptInformation
 * @return
 */
bool AutomationForm::PromptInformation()
{
    QLogHelper::instance()->LogInfo("AutomationForm->PromptInformation() 函数执行!");
    return true;
}
