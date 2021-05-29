#include "LogForm.h"
#include "ui_LogForm.h"

/**
 * @brief LogForm::LogForm
 * @param parent
 */
LogForm::LogForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogForm)
{
    QLogHelper::instance()->LogInfo("LogForm 构造函数执行!");
    ui->setupUi(this);
    this->InitStytle();
    this->Init();
    this->ConnectSlot();
}

/**
 * @brief LogForm::~LogForm
 */
LogForm::~LogForm()
{
    QLogHelper::instance()->LogInfo("LogForm->~LogForm() 函数执行!");
    delete ui;
}

/**
 * @brief LogForm::getLogFormMethod
 * @return
 */
LogFormMetod *LogForm::getLogFormMethod() const
{
    return logFormMethod;
}

/**
 * @brief LogForm::setLogFormMethod
 * @param value
 */
void LogForm::setLogFormMethod(LogFormMetod *value)
{
    logFormMethod = value;
}

/**
 * @brief LogForm::getLogFormBean
 * @return
 */
LogFormBean *LogForm::getLogFormBean() const
{
    return logFormBean;
}

/**
 * @brief LogForm::setLogFormBean
 * @param value
 */
void LogForm::setLogFormBean(LogFormBean *value)
{
    logFormBean = value;
}

/**
 * @brief LogForm::InitStytle
 */
void LogForm::InitStytle()
{
    QLogHelper::instance()->LogInfo("LogForm->InitStytle() 函数执行!");
    //添加右键清除菜单
    logViewClearAction=new QAction("清除显示");
    ui->LogView->addAction(logViewClearAction);
}

/**
 * @def 初始化函数
 * @brief LogForm::Init
 */
void LogForm::Init()
{
    QLogHelper::instance()->LogInfo("LogForm->Init() 函数执行!");
    logFormBean=new LogFormBean();
    logFormMethod=new LogFormMetod();

    logFormBean->setLogView(ui->LogView);
    logFormMethod->setLogFormBean(logFormBean);
}

/**
 * @def 处理连接信号槽函数
 * @brief LogForm::ConnectSlot
 */
void LogForm::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("LogForm->ConnectSlot() 函数执行!");
    //连接清除右键菜单槽函数
    connect(this->logViewClearAction,&QAction::triggered,this,&LogForm::LogViewClearSlot);
}

/**
 * @brief LogForm::LogViewClearSlot
 * @param logEdit
 */
void LogForm::LogViewClearSlot()
{
    QLogHelper::instance()->LogInfo("LogForm->LogViewClearSlot() 函数执行!");
    ui->LogView->clear();
}



