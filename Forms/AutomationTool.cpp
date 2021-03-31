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
/**
 * @def UI初始化过程中需要预处理操作
 * @brief AutomationTool::init
 */
void AutomationTool::init()
{
      QLogHelper::instance()->LogInfo("AutomationTool->init() 函数执行中!");
}


/**
 * @def 机种lineEdit文本改变触发函数
 *      正则表达式判断IDEdit内容是否符合要求
 *      如果不符合，设置字体为红色，添加错误码
 *      如果符合，显示无变化，判断是否存在指定错误码，如果存在，删除错误码
 * @brief AutomationTool::on_IDEdit_editingFinished
 */
void AutomationTool::on_IDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_IDEdit_editingFinished() 函数触发执行!");
    QRegExp rx(CommonBean::instance()->getRExpression().value(ui->IDEdit->objectName()));
    if(QString::number(rx.indexIn(ui->IDEdit->text()))==0){
        CommonBean::instance()->setID(ui->IDEdit->text());
    }else{
        ui->IDEdit->setStyleSheet(errFontStytle); //字体颜色
        CommonBean::instance()->getErrCode().insert(ui->IDEdit->objectName(),0x01);
    }
}


void AutomationTool::on_IDEdit_textChanged(const QString &arg1)
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_IDEdit_textChanged() 函数触发执行!");
    QLogHelper::instance()->LogDebug(arg1);
//    if(CommonBean::instance()->getErrCode().indexOf(0))
//    ui->IDEdit->setStyleSheet(nomalFontStytle); //字体颜色
}
