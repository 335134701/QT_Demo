#include "Temple.h"
#include "ui_Temple.h"

/**
 * @brief Temple::Temple
 * @param parent
 * @def 构造
 */
Temple::Temple(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Temple)
{
    this->InitStytle();
    ui->setupUi(this);
    this->Init();
    this->ConnectSlot();
}

/**
 * @brief Temple::~Temple
 * @def 析构函数
 */
Temple::~Temple()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
    delete ui;
}

/**
 * @brief Temple::InitStytle
 * @def 调用.css文件美化UI
 */
void Temple::InitStytle()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
    //加载样式表
    QFile file(":/CSS/UI.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        file.close();
    }
}

/**
 * @brief Temple::Init
 * @def 初始化相关设置及参数
 */
void Temple::Init()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
}

/**
 * @brief Temple::ConnectSlot
 * @def 连接信号槽处理函数
 */
void Temple::ConnectSlot()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
}
