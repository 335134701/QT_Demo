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
    //机种番号处理相关槽函数
    connect(this,&SIForm::JudgeIDSignal,this->siFormMethod,&SIFormMethod::JudgeIDSlot);
    connect(this,&SIForm::JudgeIDTypeSignal,this->siFormMethod,&SIFormMethod::JudgeIDTypeSlot);
    connect(this,&SIForm::ShowMessageProcessSignal,this->siFormMethod,&SIFormMethod::ShowMessageProcessSlot);
    //路径处理相关槽函数
    connect(this,&SIForm::SelectDirSignal,this->siFormMethod,&SIFormMethod::SelectDirSlot);
    //文件查找处理相关槽函数
    connect(this,&SIForm::SearchFileSignal,this->siFormMethod,&SIFormMethod::SearchFileSlot);
    //SVN更新处理相关槽函数
    connect(this,&SIForm::RunOrderSignal,this->siFormMethod,&SIFormMethod::RunOrderSlot);
    //预处理相关槽函数
    connect(this,&SIForm::PretreatmentSignal,this->siFormMethod,&SIFormMethod::PretreatmentSlot);
    //文件压缩处理相关槽函数
    connect(this,&SIForm::FileCompressionSignal,this->siFormMethod,&SIFormMethod::FileCompressionSlot);
}

/**
 * @brief SIForm::on_IDEdit_editingFinished
 */
void SIForm::on_SIIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("SIForm->on_IDEdit_editingFinished() 函数触发执行!");
    if(!PromptInformation()){return;}
    //初始化机种相关信息
    siFormBean->ResetParameter(RET_ID);
    this->siFormMethod->InitTableView();
    //初始化Table UI显示
    emit JudgeIDSignal(ui->SIIDEdit,siFormBean->getID());
    if(siFormBean->getID()->isEmpty()){return;}
    //判断机种类型
    emit JudgeIDTypeSignal(ui->SIIDEdit,siFormBean->getIDType(),siFormBean->getRelyIDType());
    //如果ID和依赖ID类型不一致，则依赖ID会显示红色
    if(!siFormBean->getIDRelyIDflag()){
        ui->SIRelyIDEdit->setStyleSheet(QString(errFontColor));
        siFormBean->setIDRelyIDflag(true);
    }else{
        ui->SIRelyIDEdit->setStyleSheet(QString(nomFontColor));
    }
    emit ShowMessageProcessSignal(SIIDflag,LOG_ALL);
}

/**
 * @brief SIForm::on_RelyIDEdit_editingFinished
 */
void SIForm::on_SIRelyIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("SIForm->on_RelyIDEdit_editingFinished() 函数触发执行!");
    if(!PromptInformation()){return;}
    //初始化机种相关信息
    siFormBean->ResetParameter(RET_RELYID);
    //如果RelyIDEdit文本输入为空或者ID与依赖ID一致，则说明不依赖任何机种
    if(ui->SIRelyIDEdit->text().isEmpty()||ui->SIRelyIDEdit->text()==(*siFormBean->getID())){
        ui->SIRelyIDEdit->setText("");
        return;
    }
    //判断机种名称是否符合要求
    emit JudgeIDSignal(ui->SIRelyIDEdit,siFormBean->getRelyID());
    if(siFormBean->getRelyID()->isEmpty()){return;}
    //判断依赖机种是否和作成机种同一种类型
    emit JudgeIDTypeSignal(ui->SIRelyIDEdit,siFormBean->getRelyIDType(),siFormBean->getIDType());
    //如果当前机种与依赖机种类型不一致，则设置依赖机种字体为红色字体
    if(!siFormBean->getIDRelyIDflag()){
        ui->SIRelyIDEdit->setStyleSheet(QString(errFontColor));
        siFormBean->setIDRelyIDflag(true);
    }else{
        ui->SIRelyIDEdit->setStyleSheet(QString(nomFontColor));
    }
    emit ShowMessageProcessSignal(SIRelyIDflag,LOG_ALL);
}

/**
 * @brief SIForm::on_SVNButton_clicked
 */
void SIForm::on_SISVNButton_clicked()
{
    QLogHelper::instance()->LogInfo("SIForm->on_SVNButton_clicked() 函数触发执行!");
    if(!PromptInformation()){return;}
    siFormBean->ResetParameter(RET_SVNFilePath);
    ui->SISVNLabel->setText(*siFormBean->getSVNDirPath());
    emit SelectDirSignal(ui->SISVNLabel,siFormBean->getSVNDirPath());
    //SVN状态更新
    if(!siFormBean->getSVNDirPath()->isEmpty()){
        //siFormBean->setSIStatus(SI_SVNUPDATE);
        //emit RunOrderSignal(SISVNUpdateflag);
    }
}

/**
 * @brief SIForm::on_OutputButton_clicked
 */
void SIForm::on_SIOutputButton_clicked()
{
    QLogHelper::instance()->LogInfo("SIForm->on_OutputButton_clicked() 函数触发执行!");
    if(!PromptInformation()){return;}
    siFormBean->ResetParameter(RET_OutPutFilePath);
    ui->SIOutputLabel->setText(*siFormBean->getOutputDirPath());
    emit SelectDirSignal(ui->SIOutputLabel,siFormBean->getOutputDirPath());
}

/**
 * @brief SIForm::on_SIFileSearchButton_clicked
 */
void SIForm::on_SIFileSearchButton_clicked()
{
    QLogHelper::instance()->LogInfo("SIForm->on_SIFileSearchButton_clicked() 函数触发执行!");
    if(!PromptInformation()||!CheckMessage(SI_CHECKMESSAGE_FileSearch)){return;}
    emit SearchFileSignal(SIRelyFileflag,true);
}

/**
 * @brief SIForm::on_SIPretreatmentButton_clicked
 */
void SIForm::on_SIPretreatmentButton_clicked()
{
    QLogHelper::instance()->LogInfo("SIForm->on_SIPretreatmentButton_clicked() 函数触发执行!");
    if(!PromptInformation()||!CheckMessage(SI_CHECKMESSAGE_Pretreatment)){return;}
    //emit PretreatmentSignal();
}


/**
 * @brief SIForm::on_SIFileCompressionButton_clicked
 */
void SIForm::on_SIFileCompressionButton_clicked()
{
    QLogHelper::instance()->LogInfo("SIForm->on_SIFileCompressionButton_clicked() 函数触发执行!");
    if(!PromptInformation()||!CheckMessage(SI_CHECKMESSAGE_FileCompression)){return;}
    //emit FileCompressionSignal();
}

/**
 * @def 执行某项操作时,其他操作不可执行提示
 *      关于 Statusflag 表示说明：
 *          0 表示无任何操作
 *          1 表示正在执行SVN更新任务
 *          2 表示正在执行文件检索任务
 *          3 表示正在执行文件读取任务
 *          4 表示正在执行文件解压任务
 * @brief SIForm::PromptInformation
 * @return
 */
bool SIForm::PromptInformation()
{
    QLogHelper::instance()->LogInfo("SIForm->PromptInformation() 函数执行!");
    if(siFormBean->getSIStatus()!=0)
    {
        switch (siFormBean->getSIStatus()) {
        case SI_SVNUPDATE:
            QMessageBox::warning(this,"Warn","正在执行SVN更新任务，其他任务暂时无法执行!");
            break;
        case SI_FILESEARCH:
            QMessageBox::warning(this,"Warn","正在执行文件检索任务，其他任务暂时无法执行!");
            break;
        case SI_FILEREAD:
            QMessageBox::warning(this,"Warn","正在文件读取任务，其他任务暂时无法执行!");
            break;
        case SI_PRETREAMENT:
            QMessageBox::warning(this,"Warn","正在执行预处理任务，其他任务暂时无法执行!");
            break;
        case SI_FILECODECOPY:
            QMessageBox::warning(this,"Warn","正在执行源码复制任务，其他任务暂时无法执行!");
            break;
        case SI_FILEUNZIP:
            QMessageBox::warning(this,"Warn","正在执行文件解压任务，其他任务暂时无法执行!");
            break;
        case SI_FILCHECKBA:
        case SI_FILCHECKCL:
            QMessageBox::warning(this,"Warn","正在执行文件校验任务，其他任务暂时无法执行!");
            break;
        case SI_FILCOMPRESSION:
            QMessageBox::warning(this,"Warn","正在执行项目生成后处理任务，其他任务暂时无法执行!");
            break;
        case SI_FILEZIP:
            QMessageBox::warning(this,"Warn","正在执行项目源码压缩处理任务，其他任务暂时无法执行!");
            break;
        default:
            break;
        }
        return false;
    }
    return true;
}

/**
 * @def 按钮按下时检查机种番号等信息是否填写
 * @brief SIForm::CheckMessage
 * @return
 */
bool SIForm::CheckMessage(const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIForm->CheckMessage() 函数执行!");
    if(siFormBean->getID()->isEmpty()){
        QMessageBox::warning(this,"Warn","未设置机种番号,无法继续执行任务!");
        return false;
    }
    if(siFormBean->getSVNDirPath()->isEmpty()){
        QMessageBox::warning(this,"Warn","未设置SVN路径,无法继续执行任务!");
        return false;
    }
    /*
    if(!siFormBean->getSVNUpdateStatus()){
        QMessageBox::warning(this,"Warn","SVN更新状态失败,无法继续执行任务!");
        return false;
    }*/
    switch (flag) {
    case SI_CHECKMESSAGE_FileSearch:

        break;
    case SI_CHECKMESSAGE_Pretreatment:
    case SI_CHECKMESSAGE_FileCompression:
        /*
        if(siFormBean->getSoftList()->size()<=0){
            QMessageBox::warning(this,"Warn","量产管理表解析失败,无法继续执行任务!");
            return false;
        }

        if(siFormBean->getCodeFilePath()->isEmpty()){
            QMessageBox::warning(this,"Warn","项目源码获取失败,无法继续执行任务!");
            return false;
        }

        if(siFormBean->getCarInfoFilePath()->isEmpty()){
            QMessageBox::warning(this,"Warn","CarInfo.Mot文件获取失败,无法继续执行任务!");
            return false;
        }
        */
        break;
    }
    return true;
}
