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
}
