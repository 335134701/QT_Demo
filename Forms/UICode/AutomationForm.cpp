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
    if(!PromptInformation()){return;}
    //初始化机种相关信息
    automationFormBean->ResetParameter(RET_ID);
    this->automationFormMethod->InitTableView();
    //初始化Table UI显示
    emit JudgeIDSignal(ui->AuIDEdit,automationFormBean->getID());
    if(automationFormBean->getID()->isEmpty()){return;}
    //判断机种类型
    emit JudgeIDTypeSignal(ui->AuIDEdit,automationFormBean->getIDType(),automationFormBean->getRelyIDType());
    //如果ID和依赖ID类型不一致，则依赖ID会显示红色
    if(!automationFormBean->getIDRelyIDflag()){
        ui->AuIDEdit->setStyleSheet(QString(errFontColor));
        automationFormBean->setIDRelyIDflag(true);
    }else{
        ui->AuIDEdit->setStyleSheet(QString(nomFontColor));
    }
    emit ShowMessageProcessSignal(SIIDflag,LOG_LOG);
}

/**
 * @brief AutomationForm::on_AuRelyIDEdit_editingFinished
 */
void AutomationForm::on_AuRelyIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationForm->on_AuRelyIDEdit_editingFinished() 函数触发执行!");
    if(!PromptInformation()){return;}
    //如果RelyIDEdit文本输入为空或者ID与依赖ID一致，则说明不依赖任何机种
    if(ui->AuRelyIDEdit->text().isEmpty()||ui->AuRelyIDEdit->text()==(*automationFormBean->getID())){
        ui->AuRelyIDEdit->setText("");
        return;
    }
    //判断机种名称是否符合要求
    emit JudgeIDSignal(ui->AuRelyIDEdit,automationFormBean->getRelyID());
    if(automationFormBean->getRelyID()->isEmpty()){return;}
    //判断依赖机种是否和作成机种同一种类型
    emit JudgeIDTypeSignal(ui->AuRelyIDEdit,automationFormBean->getRelyIDType(),automationFormBean->getIDType());
    //如果当前机种与依赖机种类型不一致，则设置依赖机种字体为红色字体
    if(!automationFormBean->getIDRelyIDflag()){
        ui->AuRelyIDEdit->setStyleSheet(QString(errFontColor));
        automationFormBean->setIDRelyIDflag(true);
    }else{
        ui->AuRelyIDEdit->setStyleSheet(QString(nomFontColor));
    }
    emit ShowMessageProcessSignal(SIRelyIDflag,LOG_ALL);
}

/**
 * @brief AutomationForm::on_AuSVNButton_clicked
 */
void AutomationForm::on_AuSVNButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationForm->on_AuSVNButton_clicked() 函数触发执行!");
    if(!PromptInformation()){return;}
    automationFormBean->ResetParameter(RET_SVNFilePath);
    ui->AuSVNLabel->setText(*automationFormBean->getSVNDirPath());
    emit SelectDirSignal(ui->AuSVNLabel,automationFormBean->getSVNDirPath());
}

/**
 * @brief AutomationForm::on_AuOutputButton_clicked
 */
void AutomationForm::on_AuOutputButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationForm->on_AuOutputButton_clicked() 函数触发执行!");
    if(!PromptInformation()){return;}
    automationFormBean->ResetParameter(RET_OutPutFilePath);
    ui->AuOutputLabel->setText(*automationFormBean->getOutputDirPath());
    emit SelectDirSignal(ui->AuOutputLabel,automationFormBean->getOutputDirPath());
}

/**
 * @brief AutomationForm::on_AuFileSearchButton_clicked
 */
void AutomationForm::on_AuFileSearchButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationForm->on_AuFileSearchButton_clicked() 函数触发执行!");
    QLogHelper::instance()->LogInfo("SIForm->on_SIFileSearchButton_clicked() 函数触发执行!");
    if(!PromptInformation()||!CheckMessage(AU_CHECKMESSAGE_FileSearch)){return;}
    emit SearchFileSignal(SIRelyFileflag,false);
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
     if(automationFormBean->getID()->isEmpty()){
         QMessageBox::warning(this,"Warn","未设置机种番号,无法执行文件检索任务!");
         return false;
     }
     if(automationFormBean->getSVNDirPath()->isEmpty()){
         QMessageBox::warning(this,"Warn","未设置SVN路径,无法执行文件检索任务!");
         return false;
     }
     switch (flag) {
     case AU_CHECKMESSAGE_FileSearch:

         break;
     case AU_CHECKMESSAGE_FileCreate:

         break;
     }
     return true;
}
