#include "GTool.h"
#include "ui_GTool.h"

/**
 * @brief GTool::GTool
 * @param parent
 * @def 构造
 */
GTool::GTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GTool)
{
    this->InitStytle();
    ui->setupUi(this);
    this->Init();
    this->ConnectSlot();
}

/**
 * @brief GTool::~GTool
 * @def 析构函数
 */
GTool::~GTool()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
    delete ui;
}


/**
 * @brief GTool::InitStytle
 * @def 调用.css文件美化UI
 */
void GTool::InitStytle()
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
 * @brief GTool::Init
 * @def 初始化相关设置及参数
 */
void GTool::Init()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
    this->ui->Log->setVisible(false);
}

/**
 * @brief GTool::ConnectSlot
 * @def 连接信号槽处理函数
 */
void GTool::ConnectSlot()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
    connect(this->ui->actionInfo,&QAction::triggered,this,&GTool::on_Log_triggred);
    connect(this->ui->actionDebug,&QAction::triggered,this,&GTool::on_Log_triggred);
    connect(this->ui->actionWarn,&QAction::triggered,this,&GTool::on_Log_triggred);
    connect(this->ui->actionError,&QAction::triggered,this,&GTool::on_Log_triggred);
    connect(this->ui->actionOtherMessage,&QAction::triggered,this,&GTool::on_Log_triggred);
}

/**
 * @brief GTool::on_Log_triggred
 */
void GTool::on_Log_triggred()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
}
