#include "AutomationTool.h"
#include "ui_AutomationTool.h"

AutomationTool::AutomationTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AutomationTool)
{
    QLogHelper::instance()->LogInfo("AutomationTool构造函数执行中!");
    ui->setupUi(this);
    this->init();
}

AutomationTool::~AutomationTool()
{
    QLogHelper::instance()->LogInfo("AutomationTool执行结束,删除UI对象!");
    delete ui;
}

void AutomationTool::init()
{
      QLogHelper::instance()->LogInfo("AutomationTool->init() 函数执行中!");
}

/**
 * @def 机种lineEdit文本改变触发函数
 * @brief AutomationTool::on_IDEdit_editingFinished
 */
void AutomationTool::on_IDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_IDEdit_editingFinished() 函数触发执行!");
}

