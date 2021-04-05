#include "AutomationTool.h"
#include "ui_AutomationTool.h"

AutomationTool::AutomationTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AutomationTool)
{
    QLogHelper::instance()->LogInfo("AutomationTool构造函数执行中!");
    ui->setupUi(this);
    this->initStyle();
    this->init();
}
/**
 * @brief AutomationTool::~AutomationTool
 */
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
    desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    comBean=new CommonBean();
    //初始化设置Error Code，通过解析XML方式获取错误码
    comBean->getErrCode().insert(ui->IDEdit->objectName(),0x01);
    comBean->getErrCode().insert(ui->RelyIDEdit->objectName(),0x02);
    comBean->getErrCode().insert(ui->ResultButton->objectName(),0x03);
    comBean->getErrCode().insert(ui->OutputButton->objectName(),0x04);
}
/**
 * @def UI界面初始化函数，主要功能是美化UI
 * @brief AutomationTool::initStyle
 */
void AutomationTool::initStyle()
{
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
    //设置正则表达式对象
    QRegExp rx(comBean->getRExpression().value(ui->IDEdit->objectName()));
    if(rx.indexIn(ui->IDEdit->text())==0){
        ui->IDEdit->setStyleSheet(QString(errFontColor)+QString(comBakColor)+QString(comFont)); //字体相关设置
        //如果存在错误码则移除错误码
        if(!QString(comBean->getErrCode().value(ui->IDEdit->objectName())).isEmpty()){
            comBean->getErrCode().remove(ui->IDEdit->objectName());
        }
        comBean->setID(ui->IDEdit->text());
    }else{
        ui->IDEdit->setStyleSheet(QString(nomFontColor)+QString(comBakColor)+QString(comFont)); //字体相关设置
        comBean->setID("");
        //如果错误码不存在，则添加错误码
        if(QString(comBean->getErrCode().value(ui->IDEdit->objectName())).isEmpty()){
            comBean->getErrCode().insert(ui->IDEdit->objectName(),0x01);
        }
    }
}

/**
 * @def 机种RelyIDEdit文本改变完成触发函数
 * @brief AutomationTool::on_RelyIDEdit_editingFinished
 */
void AutomationTool::on_RelyIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_RelyIDEdit_editingFinished() 函数触发执行!");
    comBean->setRelyID("");
    //如果RelyIDEdit文本输入为空，则说明不依赖任何机种
    if(ui->RelyIDEdit->text().isEmpty()&&!QString(comBean->getErrCode().value(ui->RelyIDEdit->objectName())).isEmpty()){
        comBean->getErrCode().remove(ui->RelyIDEdit->objectName());
        return;
    }
    //RelyIDEdit使用的正则表达式与IDEdit一致，使用IDEdit定义的正则表达式
    QRegExp rx(comBean->getRExpression().value(ui->IDEdit->objectName()));
    if(rx.indexIn(ui->RelyIDEdit->text())==0){
        ui->RelyIDEdit->setStyleSheet(QString(errFontColor)+QString(comBakColor)+QString(comFont)); //字体相关设置
        //如果存在错误码则移除错误码
        if(!QString(comBean->getErrCode().value(ui->RelyIDEdit->objectName())).isEmpty()){
            comBean->getErrCode().remove(ui->RelyIDEdit->objectName());
        }
        comBean->setRelyID(ui->RelyIDEdit->text());
    }else{
        ui->RelyIDEdit->setStyleSheet(QString(nomFontColor)+QString(comBakColor)+QString(comFont)); //字体相关设置
        //如果错误码不存在，则添加错误码
        if(QString(comBean->getErrCode().value(ui->RelyIDEdit->objectName())).isEmpty()){
            comBean->getErrCode().insert(ui->RelyIDEdit->objectName(),0x02);
        }
    }
}
/**
 * @def
 * @brief AutomationTool::on_ResultButton_clicked
 */
void AutomationTool::on_ResultButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_ResultButton_clicked() 函数触发执行!");
    ui->ResultLabel->setText("");
    comBean->setResultDirPath("");
    QString dirName =  QFileDialog::getExistingDirectory(this, tr("Open Directory"),desktopPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirName.isEmpty()) {
        ui->ResultLabel->setStyleSheet(QString(errFontColor));
        QMessageBox::warning(this, "警告", tr("No directory selected!"));
        return;
    }
    ui->ResultLabel->setStyleSheet(QString(nomFontColor));
    ui->ResultLabel->setText(dirName);
    comBean->setResultDirPath(dirName);
}
/**
 * @def
 * @brief AutomationTool::on_MotButton_clicked
 */
void AutomationTool::on_MotButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_MotButton_clicked() 函数触发执行!");
    ui->MotLabel->setText("");
    comBean->setMotDirPath("");
    QString dirName =  QFileDialog::getExistingDirectory(this, tr("Open Directory"),desktopPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirName.isEmpty()) {
        ui->MotLabel->setStyleSheet(QString(errFontColor));
        QMessageBox::warning(this, "警告", tr("No directory selected!"));
        return;
    }
    ui->MotLabel->setStyleSheet(QString(nomFontColor));
    ui->MotLabel->setText(dirName);
    comBean->setMotDirPath(dirName);
}
/**
 * @def
 * @brief AutomationTool::on_OutputButton_clicked
 */
void AutomationTool::on_OutputButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_OutputButton_clicked() 函数触发执行!");
    ui->OutputLabel->setText("");
    comBean->setOutputDirPath("");
    QString dirName =  QFileDialog::getExistingDirectory(this, tr("Open Directory"),desktopPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirName.isEmpty()) {
        ui->OutputLabel->setStyleSheet(QString(errFontColor));
        QMessageBox::warning(this, "警告", tr("No directory selected!"));
        return;
    }
    ui->OutputLabel->setStyleSheet(QString(nomFontColor));
    ui->OutputLabel->setText(dirName);
    comBean->setOutputDirPath(dirName);
}
/**
 * @def
 * @brief AutomationTool::on_CreateButton_clicked
 */
void AutomationTool::on_CreateButton_clicked()
{
    if(comBean->getErrCode().size()==0){

    }else{

    }
}
