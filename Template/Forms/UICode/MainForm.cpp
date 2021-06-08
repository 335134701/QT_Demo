#include "MainForm.h"
#include "ui_MainForm.h"

/**
 * @brief MainForm::MainForm
 * @param parent
 */
MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    QLogHelper::instance()->LogInfo("MainForm() 构造函数执行!");
    this->InitStytle();
    ui->setupUi(this);
    this->Init();
    this->BetweenUI_Init();
    this->ConnectSlot();
}

/**
 * @brief MainForm::~MainForm
 */
MainForm::~MainForm()
{
    QLogHelper::instance()->LogInfo("MainForm->~MainForm() 函数执行!");
    delete ui;
}

/**
 * @def 调用.css文件美化UI
 * @brief MainForm::InitStytle
 */
void MainForm::InitStytle()
{
    QLogHelper::instance()->LogInfo("MainForm->InitStytle() 函数执行!");
    //加载样式表
    QFile file(":/MainForm.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        file.close();
    }
}

/**
 * @def 初始化处理函数
 * @brief MainForm::Init
 */
void MainForm::Init()
{
    QLogHelper::instance()->LogInfo("MainForm->Init() 函数执行!");
    mainFormBean=new MainFormBean();
    mainFormMethod=new MainFormMethod();
}

/**
 * @def UI之间页面初始化
 * @brief MainForm::BetweenUI_Init
 */
void MainForm::BetweenUI_Init()
{
    QLogHelper::instance()->LogInfo("MainForm->BetweenUI_Init() 函数执行!");
    //UI初始化过程中存在一定顺序，避免出现空指针异常，赋值语句放在主窗口中进行
    ui->SI->setLogFormMethod(ui->LogWgedit->getLogFormMethod());
    ui->SI->getSiFormMethod()->setLogFormMethod(ui->LogWgedit->getLogFormMethod());
    ui->SI->getSiFormBean()->setCommonMethod(mainFormBean->getCommonMethod());
    ui->SI->getSiFormMethod()->ConnectOtherUISlot();

    //ui->LogWgedit->getLogFormBean()->setLastIndex(ui->FunctionWgedit->currentIndex());

    ui->Automation->setLogFormMethod(ui->LogWgedit->getLogFormMethod());
    ui->Automation->getAutomationFormMethod()->setLogFormMethod(ui->LogWgedit->getLogFormMethod());
    ui->Automation->getAutomationFormBean()->setCommonMethod(mainFormBean->getCommonMethod());
    ui->Automation->getAutomationFormMethod()->ConnectOtherUISlot();
}

/**
 * @def 处理连接信号槽函数
 * @brief MainForm::ConnectSlot
 */
void MainForm::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("MainForm->ConnectSlot() 函数执行!");
}

/**
 * @brief MainForm::on_FunctionWgedit_tabBarClicked
 * @param index
 */
void MainForm::on_FunctionWgedit_tabBarClicked(int index)
{
    QLogHelper::instance()->LogInfo("MainForm->on_FunctionWgedit_tabBarClicked() 函数触发执行!");
    /*
    if(index!=ui->LogWgedit->getLogFormBean()->getLastIndex()){
        ui->LogWgedit->getLogFormBean()->getTextList()->replace(ui->LogWgedit->getLogFormBean()->getLastIndex(),ui->LogWgedit->getLogFormBean()->getLogView()->toPlainText());
        ui->LogWgedit->getLogFormBean()->getLogView()->clear();
        ui->LogWgedit->getLogFormBean()->getLogView()->append(ui->LogWgedit->getLogFormBean()->getTextList()->value(index));
        ui->LogWgedit->getLogFormBean()->setLastIndex(index);
    }*/
}
