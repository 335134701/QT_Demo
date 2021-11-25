#include "FirstForm.h"
#include "ui_FirstForm.h"

FirstForm::FirstForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FirstForm)
{
    ui->setupUi(this);
    this->Init();
    this->ConnectSlot();
}

FirstForm::~FirstForm()
{
    delete ui;
}


/**
 * @brief FirstForm::Init
 * @def 初始化相关设置及参数
 */
void FirstForm::Init()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
}

/**
 * @brief FirstForm::ConnectSlot
 * @def 连接信号槽处理函数
 */
void FirstForm::ConnectSlot()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
}
