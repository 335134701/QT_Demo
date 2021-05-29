#include "SIForm.h"
#include "ui_SIForm.h"

/**
 * @brief SIForm::SIForm
 * @param parent
 */
SIForm::SIForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SIForm)
{
    QLogHelper::instance()->LogInfo("SIForm() 构造函数执行!");
    ui->setupUi(this);
    this->InitStytle();
    this->Init();
    this->ConnectSlot();
}

/**
 * @brief SIForm::~SIForm
 */
SIForm::~SIForm()
{
    QLogHelper::instance()->LogInfo("SIForm->~SIForm() 函数执行!");
    delete ui;
}

/**
 * @brief SIForm::getLogFormMethod
 * @return
 */
LogFormMetod *SIForm::getLogFormMethod() const
{
    return logFormMethod;
}

/**
 * @brief SIForm::setLogFormMethod
 * @param value
 */
void SIForm::setLogFormMethod(LogFormMetod *value)
{
    logFormMethod = value;
}

/**
 * @brief SIForm::getSiFormMethod
 * @return
 */
SIFormMethod *SIForm::getSiFormMethod() const
{
    return siFormMethod;
}

/**
 * @brief SIForm::setSiFormMethod
 * @param value
 */
void SIForm::setSiFormMethod(SIFormMethod *value)
{
    siFormMethod = value;
}
/**
 * @brief SIForm::getSiFormBean
 * @return
 */
SIFormBean *SIForm::getSiFormBean() const
{
    return siFormBean;
}
/**
 * @brief SIForm::setSiFormBean
 * @param value
 */
void SIForm::setSiFormBean(SIFormBean *value)
{
    siFormBean = value;
}

/**
 * @brief SIForm::InitStytle
 */
void SIForm::InitStytle()
{
    QLogHelper::instance()->LogInfo("SIForm->InitStytle() 函数执行!");
}

/**
 * @def 初始化函数
 * @brief SIForm::Init
 */
void SIForm::Init()
{
    QLogHelper::instance()->LogInfo("SIForm->Init() 函数执行!");
    siFormBean  = new SIFormBean();
    siFormMethod = new SIFormMethod();
    siFormMethod->setSiFormBean(siFormBean);
}

/**
 * @def 处理连接信号槽函数
 * @brief SIForm::ConnectSlot
 */
void SIForm::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("SIForm->ConnectSlot() 函数执行!");
    connect(this,&SIForm::JudgeIDSignal,this->siFormMethod,&SIFormMethod::JudgeIDSlot);
    connect(this,&SIForm::JudgeIDTypeSignal,this->siFormMethod,&SIFormMethod::JudgeIDTypeSlot);
    connect(this,&SIForm::ShowMessageProcessSignal,this->siFormMethod,&SIFormMethod::ShowMessageProcessSlot);
}

/**
 * @brief SIForm::on_IDEdit_editingFinished
 */
void SIForm::on_IDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("SIForm->on_IDEdit_editingFinished() 函数触发执行!");
    if(PromptInformation()){return;}
    //初始化机种相关信息
    siFormBean->ResetParameter(RET_ID);
    //初始化Table UI显示
    emit JudgeIDSignal(ui->IDEdit,siFormBean->getID());
    if(siFormBean->getID()->isEmpty()){return;}
    //判断机种类型
    emit JudgeIDTypeSignal(ui->IDEdit,siFormBean->getIDType(),siFormBean->getRelyIDType());
    //如果ID和依赖ID类型不一致，则依赖ID会显示红色
    if(!siFormBean->getIDRelyIDflag()){
        ui->RelyIDEdit->setStyleSheet(QString(errFontColor));
        siFormBean->setIDRelyIDflag(true);
    }else{
        ui->RelyIDEdit->setStyleSheet(QString(nomFontColor));
    }
    emit ShowMessageProcessSignal(IDflag,LOG_LOG);
}

/**
 * @brief SIForm::on_RelyIDEdit_editingFinished
 */
void SIForm::on_RelyIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("SIForm->on_RelyIDEdit_editingFinished() 函数触发执行!");
    if(PromptInformation()){return;}
    //如果RelyIDEdit文本输入为空或者ID与依赖ID一致，则说明不依赖任何机种
    if(ui->RelyIDEdit->text().isEmpty()||ui->RelyIDEdit->text()==(*siFormBean->getID())){
        ui->RelyIDEdit->setText("");
        return;
    }
    //判断机种名称是否符合要求
    emit JudgeIDSignal(ui->RelyIDEdit,siFormBean->getRelyID());
    if(siFormBean->getRelyID()->isEmpty()){return;}
    //判断依赖机种是否和作成机种同一种类型
    emit JudgeIDTypeSignal(ui->RelyIDEdit,siFormBean->getRelyIDType(),siFormBean->getIDType());
    //如果当前机种与依赖机种类型不一致，则设置依赖机种字体为红色字体
    if(!siFormBean->getIDRelyIDflag()){
        ui->RelyIDEdit->setStyleSheet(QString(errFontColor));
        siFormBean->setIDRelyIDflag(true);
    }else{
        ui->RelyIDEdit->setStyleSheet(QString(nomFontColor));
    }
    emit ShowMessageProcessSignal(RelyIDflag,LOG_ALL);
}

/**
 * @brief SIForm::on_SVNButton_clicked
 */
void SIForm::on_SVNButton_clicked()
{
    QLogHelper::instance()->LogInfo("SIForm->on_SVNButton_clicked() 函数触发执行!");
    if(PromptInformation()){return;}
    emit ShowMessageProcessSignal(IDflag,LOG_ALL);
}

/**
 * @brief SIForm::on_OutputButton_clicked
 */
void SIForm::on_OutputButton_clicked()
{
    QLogHelper::instance()->LogInfo("SIForm->on_OutputButton_clicked() 函数触发执行!");
    if(PromptInformation()){return;}
}


/**
 * @def 执行某项操作时,其他操作不可执行提示
 *      关于 Statusflag 表示说明：
 *          0 表示无任何操作
 *          1 表示正在查找文件
 *          2 表示正在解析文件
 *          3 表示正在生成相应的文件目录结构
 * @brief SIForm::PromptInformation
 * @return
 */
bool SIForm::PromptInformation()
{
    QLogHelper::instance()->LogInfo("SIForm->PromptInformation() 函数执行!");
    if(siFormBean->getSIStatus()!=0)
    {
        switch (siFormBean->getSIStatus()) {
        case 1:
            QMessageBox::warning(this,"Warn","正在执行文件查找任务，其他任务暂时无法执行!");
            break;
        case 2:
            QMessageBox::warning(this,"Warn","正在执行文件解析任务，其他任务暂时无法执行!");
            break;
        case 3:
            QMessageBox::warning(this,"Warn","正在执行生成任务，其他任务暂时无法执行!");
            break;
        default:
            break;
        }
        return true;
    }
    return false;
}









