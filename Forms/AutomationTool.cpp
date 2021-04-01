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
      comBean=new CommonBean();


      //connect(ui->IDEdit,&QLineEdit::);
}


/**
 * @def 机种lineEdit文本改变完成触发函数
 *      正则表达式判断IDEdit内容是否符合要求
 *      如果不符合，设置字体为红色，添加错误码
 *      如果符合，显示无变化，判断是否存在指定错误码，如果存在，删除错误码
 * @brief AutomationTool::on_IDEdit_editingFinished
 */
void AutomationTool::on_IDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_IDEdit_editingFinished() 函数触发执行!");
    QRegExp rx(comBean->getRExpression().value(ui->IDEdit->objectName()));
    if(rx.indexIn(ui->IDEdit->text())==0){
        ui->IDEdit->setStyleSheet(nomalFontStytle); //字体相关设置
        if(!QString(comBean->getErrCode().value(ui->IDEdit->objectName())).isEmpty()){
            comBean->getErrCode().remove(ui->IDEdit->objectName());
        }
        comBean->setID(ui->IDEdit->text());
    }else{
        ui->IDEdit->setStyleSheet(errFontStytle); //字体相关设置
        comBean->getErrCode().insert(ui->IDEdit->objectName(),0x01);
    }
}
/**
 * @brief AutomationTool::mousePressEvent
 * @param event
 */
void AutomationTool::mousePressEvent(QMouseEvent *event)
{
   QLogHelper::instance()->LogInfo("AutomationTool->mousePressEvent() 函数触发执行!");
   if(event->button() == Qt::LeftButton){
       emit IDEditMouse_Signal();
   }
}
/**
 * @brief AutomationTool::IDEditMouse_Clicked
 */
void AutomationTool::IDEditMouse_Clicked()
{

}
/**
 * @def 机种lineEdit文本改变触发函数
 * @brief AutomationTool::on_RelyIDEdit_editingFinished
 */
void AutomationTool::on_RelyIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_RelyIDEdit_editingFinished() 函数触发执行!");
    QRegExp rx(comBean->getRExpression().value(ui->IDEdit->objectName()));
    if(rx.indexIn(ui->RelyIDEdit->text())==0){
        ui->RelyIDEdit->setStyleSheet(nomalFontStytle); //字体相关设置
        if(!QString(comBean->getErrCode().value(ui->RelyIDEdit->objectName())).isEmpty()){
            comBean->getErrCode().remove(ui->RelyIDEdit->objectName());
        }
        comBean->setRelyID(ui->RelyIDEdit->text());
    }else{
        ui->RelyIDEdit->setStyleSheet(errFontStytle); //字体相关设置
        comBean->getErrCode().insert(ui->RelyIDEdit->objectName(),0x02);
    }
}
/**
 * @def 导入依赖文件触发函数
 * @brief AutomationTool::on_RelyButton_clicked
 */
void AutomationTool::on_RelyButton_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(NULL, "请选择数据文件", "", "CSV(*.CSV)");
    if (filepath.isEmpty())
    {
        QMessageBox::warning(NULL, "提示", "未选择数据文件", "确定");
    }
}

void AutomationTool::on_ResultButton_clicked()
{

}

void AutomationTool::on_MotButton_clicked()
{

}

void AutomationTool::on_OutputButton_clicked()
{

}
