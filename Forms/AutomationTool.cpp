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
    connect(this,&AutomationTool::SelectDirSignal,this->uiMethod,&UIMethod::SelectDirSlot);
    connect(this,&AutomationTool::SelectExampleSignal,this->uiMethod,&UIMethod::SelectExampleSlot);
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
    ui->LogView->clear();
    //判断机种名称是否符合要求
    emit JudgeIDSignal(ui->IDEdit,comBean->getID());
    if(comBean->getID()->isEmpty()){return;}
    //分析机种类型
    emit JudgeIDTypeSignal(ui->IDEdit);
    if(!comBean->getErrCode()->value(IDRelyID).ID.isEmpty()){
         ui->RelyIDEdit->setStyleSheet(QString(errFontColor));
     }
     if(comBean->getIDType().isEmpty()){return;}
     //根据机种类型找到对应的依赖文件路径
     emit SelectExampleSignal(comBean->exampleDirPath,false);
     if(comBean->getRelyFilePath().isEmpty()){return;}
     //发送信号，读取依赖文件的相关信息

     //信息读取完成后显示
}
/**
 * @def 机种RelyIDEdit文本改变完成触发函数
 * @brief AutomationTool::on_RelyIDEdit_editingFinished
 */
void AutomationTool::on_RelyIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_RelyIDEdit_editingFinished() 函数触发执行!");
    //如果RelyIDEdit文本输入为空，则说明不依赖任何机种
    if(ui->RelyIDEdit->text().isEmpty()){
        //如果错误码存在,则删除错误码
        uiMethod->ErrorCodeDeal(ui->RelyIDEdit->objectName(),false);
        return;
    }
    //判断机种名称是否符合要求
    emit JudgeIDSignal(ui->RelyIDEdit,comBean->getRelyID());
    if(comBean->getRelyID()->isEmpty()){return;}
    //判断依赖机种是否和作成机种同一种类型
    emit JudgeIDTypeSignal(ui->RelyIDEdit);
}
/**
 * @def
 * @brief AutomationTool::on_SVNButton_clicked
 */
void AutomationTool::on_SVNButton_clicked()
{
     QLogHelper::instance()->LogInfo("AutomationTool->on_SVNButton_clicked() 函数触发执行!");
     //获取相应文件路径
     emit SelectDirSignal(ui->SVNLabel,comBean->getSVNDirPath());
     //QStringList st=comBean->getComMethod()->FinFile(dirName,QStringList() << "*.c");
}
/**
 * @def
 * @brief AutomationTool::on_OutputButton_clicked
 */
void AutomationTool::on_OutputButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_OutputButton_clicked() 函数触发执行!");
    //生成路径获取
    emit SelectDirSignal(ui->OutputLabel,comBean->getOutputDirPath());
    if(comBean->getOutputDirPath()->isEmpty()){return;}
}
/**
 * @def
 * @brief AutomationTool::on_CreateButton_clicked
 */
void AutomationTool::on_CreateButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_CreateButton_clicked() 函数触发执行!");
    if(comBean->getErrCode()->size()==0){

    }else{
        //根据错误码，弹出错误显示
    }
}


