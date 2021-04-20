#include "AutomationTool.h"
#include "ui_AutomationTool.h"

AutomationTool::AutomationTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AutomationTool)
{
    QLogHelper::instance()->LogInfo("AutomationTool() 构造函数执行!");
    ui->setupUi(this);
    this->initStyle();
    this->init();
    this->ConnectSlot();
}
/**
 * @brief AutomationTool::~AutomationTool
 */
AutomationTool::~AutomationTool()
{
    QLogHelper::instance()->LogInfo("AutomationTool() 执行结束,删除UI对象!");
    delete ui;
}
/**
 * @def UI初始化过程中需要预处理操作
 * @brief AutomationTool::init
 */
void AutomationTool::init()
{
    QLogHelper::instance()->LogInfo("AutomationTool->init() 函数执行!");
    comBean=new CommonBean();
    uiMethod=new UIMethod();
    uiMethod->setComBean(comBean);
    uiMethod->setTextEdit(ui->LogView);
}
/**
 * @def UI连接信号槽函数
 * @brief AutomationTool::ConnectSlot
 */
void AutomationTool::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("AutomationTool->ConnectSlot() 函数执行!");
    connect(this,&AutomationTool::JudgeIDSignal,this->uiMethod,&UIMethod::JudgeIDSlot);
    connect(this,&AutomationTool::JudgeIDTypeSignal,this->uiMethod,&UIMethod::JudgeIDTypeSlot);
    connect(this,&AutomationTool::ShowIDmessageSignal,this->uiMethod,&UIMethod::ShowIDmessageSlot);
    connect(this,&AutomationTool::SelectDirSignal,this->uiMethod,&UIMethod::SelectDirSlot);
    connect(logViewClearAction,&QAction::triggered,this,&AutomationTool::LogViewClearSlot);
    connect(this,&AutomationTool::SelectFileSignal,this->uiMethod,&UIMethod::SelectFileSlot);
}
/**
 * @def UI界面初始化函数，主要功能是美化UI
 * @brief AutomationTool::initStyle
 */
void AutomationTool::initStyle()
{
    QLogHelper::instance()->LogInfo("AutomationTool->initStyle() 函数执行!");
    //加载样式表
    QFile file(":/qss.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        file.close();
    }
    logViewClearAction=new QAction("清除显示");
    //为Logview添加清除操作
    ui->LogView->addAction(logViewClearAction);
    messageViewModel=new QStandardItemModel();
    messageViewModel->setItem(0, 0, new QStandardItem("张三"));
    messageViewModel->setItem(0, 1, new QStandardItem("20120202"));
    messageViewModel->setItem(1, 0, new QStandardItem("张三"));
    messageViewModel->setItem(1, 1, new QStandardItem("20120202"));
    ui->MessageView->setModel(messageViewModel);
}

/**
 * @def 机种IDEdit文本改变完成触发函数
 *      正则表达式判断IDEdit内容是否符合要求
 *      如果不符合，设置字体为红色，如果错误码中无对应的错误码，则添加错误码
 *      如果符合，显示无变化，判断是否存在指定错误码，如果存在，删除错误码
 * @brief AutomationTool::on_IDEdit_editingFinished
 */
void AutomationTool::on_IDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_IDEdit_editingFinished() 函数触发执行!");
    if(comBean==NULL||MessageWarn()){return;}
    //判断机种名称是否符合要求
    emit JudgeIDSignal(ui->IDEdit,comBean->getID());
    if(comBean->getID()->isEmpty()){return;}
    //分析机种类型
    emit JudgeIDTypeSignal(ui->IDEdit,comBean->getIDType(),comBean->getRelyIDType());
    if(!comBean->getErrCode()->value(IDRelyID).ID.isEmpty()){
         ui->RelyIDEdit->setStyleSheet(QString(errFontColor));
     }
    //LogView界面显示信息
    emit ShowIDmessageSignal(1);
}
/**
 * @def 机种RelyIDEdit文本改变完成触发函数
 * @brief AutomationTool::on_RelyIDEdit_editingFinished
 */
void AutomationTool::on_RelyIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_RelyIDEdit_editingFinished() 函数触发执行!");
    if(comBean==NULL||MessageWarn()){return;}
    //如果RelyIDEdit文本输入为空，则说明不依赖任何机种
    if(ui->RelyIDEdit->text().isEmpty()||ui->RelyIDEdit->text()==comBean->getID()){
        ui->RelyIDEdit->setStyleSheet(QString(errFontColor));
        //如果错误码存在,则删除错误码
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),ui->RelyIDEdit->objectName(),NULL,false);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),IDRelyID,NULL,false);
        return;
    }
    //判断机种名称是否符合要求
    emit JudgeIDSignal(ui->RelyIDEdit,comBean->getRelyID());
    if(comBean->getRelyID()->isEmpty()){return;}
    //判断依赖机种是否和作成机种同一种类型
    emit JudgeIDTypeSignal(ui->RelyIDEdit,comBean->getRelyIDType(),comBean->getIDType());
    emit ShowIDmessageSignal(2);
}
/**
 * @def
 * @brief AutomationTool::on_SVNButton_clicked
 */
void AutomationTool::on_SVNButton_clicked()
{
     QLogHelper::instance()->LogInfo("AutomationTool->on_SVNButton_clicked() 函数触发执行!");
     if(comBean==NULL||MessageWarn()){return;}
     //获取相应文件路径
     emit SelectDirSignal(ui->SVNLabel,comBean->getSVNDirPath(),SVNDirError);
     if(comBean->getSVNDirPath()->isEmpty()||comBean->getID()->isEmpty()){return;}
     emit SelectFileSignal(*(comBean->getSVNDirPath()),3);
}

/**
 * @def
 * @brief AutomationTool::on_OutputButton_clicked
 */
void AutomationTool::on_OutputButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_OutputButton_clicked() 函数触发执行!");
    if(comBean==NULL||MessageWarn()){return;}
    //生成路径获取
    emit SelectDirSignal(ui->OutputLabel,comBean->getOutputDirPath(),ui->OutputLabel->objectName());
}
/**
 * @def
 * @brief AutomationTool::on_CreateButton_clicked
 */
void AutomationTool::on_CreateButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_CreateButton_clicked() 函数触发执行!");
    if(comBean==NULL||MessageWarn()){return;}
    if(comBean->getErrCode()->size()==0){

    }else{
        //根据错误码，弹出错误显示
    }
}
/**
 * @brief AutomationTool::LogViewClearSlot
 */
void AutomationTool::LogViewClearSlot()
{
    QLogHelper::instance()->LogInfo("AutomationTool->LogViewClearSlot() 函数触发执行!");
    ui->LogView->clear();
}

/**
 * @brief AutomationTool::on_CheckButton_clicked
 */
void AutomationTool::on_CheckButton_clicked()
{
}
/**
 * @def 执行某项操作时,其他操作不可执行提示
 *      关于 Statusflag 表示说明：
 *          0 表示无任何操作
 *          1 表示正在查找文件
 *          2 表示正在解析文件
 *          3 表示正在生成相应的文件目录结构
 *          4 表示正在Check 相应的文件
 * @brief AutomationTool::MessageWarn
 * @return
 */
bool AutomationTool::MessageWarn()
{
    if(comBean->getStatusflag()!=0)
    {
        switch (comBean->getStatusflag()) {
        case 1:
            QMessageBox::warning(this,"Warn","正在执行文件查找任务，其他任务暂时无法执行!");
            break;
        case 2:
            QMessageBox::warning(this,"Title","Error Message");
            break;
        case 3:
            QMessageBox::warning(this,"Title","Error Message");
            break;
        default:
            break;
        }
        return true;
    }
    return false;
}
