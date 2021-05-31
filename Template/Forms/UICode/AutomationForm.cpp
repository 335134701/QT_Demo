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
    //机种番号处理相关槽函数
    connect(this,&AutomationForm::JudgeIDSignal,this->automationFormMethod,&AutomationFormMethod::JudgeIDSlot);
    connect(this,&AutomationForm::JudgeIDTypeSignal,this->automationFormMethod,&AutomationFormMethod::JudgeIDTypeSlot);
    connect(this,&AutomationForm::ShowMessageProcessSignal,this->automationFormMethod,&AutomationFormMethod::ShowMessageProcessSlot);
    //路径处理相关槽函数
    connect(this,&AutomationForm::SelectDirSignal,this->automationFormMethod,&AutomationFormMethod::SelectDirSlot);
    //文件查找处理相关槽函数
    connect(this,&AutomationForm::SearchFileSignal,this->automationFormMethod,&AutomationFormMethod::SearchFileSlot);
}

/**
 * @brief AutomationForm::on_AuIDEdit_editingFinished
 */
void AutomationForm::on_AuIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationForm->on_AuIDEdit_editingFinished() 函数触发执行!");
}

/**
 * @brief AutomationForm::on_AuRelyIDEdit_editingFinished
 */
void AutomationForm::on_AuRelyIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationForm->on_AuRelyIDEdit_editingFinished() 函数触发执行!");
}

/**
 * @brief AutomationForm::on_AuSVNButton_clicked
 */
void AutomationForm::on_AuSVNButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationForm->on_AuSVNButton_clicked() 函数触发执行!");
}

/**
 * @brief AutomationForm::on_AuOutputButton_clicked
 */
void AutomationForm::on_AuOutputButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationForm->on_AuOutputButton_clicked() 函数触发执行!");
}

/**
 * @brief AutomationForm::on_AuFileSearchButton_clicked
 */
void AutomationForm::on_AuFileSearchButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationForm->on_AuFileSearchButton_clicked() 函数触发执行!");
}

/**
 * @brief AutomationForm::on_AuCreateButton_clicked
 */
void AutomationForm::on_AuCreateButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationForm->on_AuCreateButton_clicked() 函数触发执行!");
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

/**
 * @brief AutomationForm::CheckMessage
 * @param flag
 * @return
 */
bool AutomationForm::CheckMessage(const unsigned int flag)
{
     QLogHelper::instance()->LogInfo("AutomationForm->CheckMessage() 函数执行!");
     /*
     if(siFormBean->getID()->isEmpty()){
         QMessageBox::warning(this,"Warn","未设置机种番号,无法执行文件检索任务!");
         return false;
     }
     if(siFormBean->getSVNDirPath()->isEmpty()){
         QMessageBox::warning(this,"Warn","未设置SVN路径,无法执行文件检索任务!");
         return false;
     }
     switch (flag) {
     case SI_CHECKMESSAGE_FileSearch:

         break;
     case SI_CHECKMESSAGE_Pretreatment:

         break;
     case SI_CHECKMESSAGE_FileCompression:

         break;
     }
     return true;
     */
}
