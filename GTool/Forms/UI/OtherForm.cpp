#include "OtherForm.h"
#include "ui_OtherForm.h"

OtherForm::OtherForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OtherForm)
{
    ui->setupUi(this);
    this->Init();
    this->ConnectSlot();
}

OtherForm::~OtherForm()
{
    delete ui;
}


/**
 * @brief OtherForm::Init
 * @def 初始化相关设置及参数
 */
void OtherForm::Init()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
}

/**
 * @brief OtherForm::ConnectSlot
 * @def 连接信号槽处理函数
 */
void OtherForm::ConnectSlot()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
}

void OtherForm::on_pushButton_clicked()
{

}
