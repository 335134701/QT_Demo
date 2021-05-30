#include "SIFormMethod.h"

/**
 * @brief SIFormMethod::SIFormMethod
 * @param parent
 */
SIFormMethod::SIFormMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("SIFormMethod 构造函数执行!");
    this->Init();
    this->ConnectSlot();
    this->InitTableView();
}

/**
 * @brief SIFormMethod::getTableView
 * @return
 */
QTableView *SIFormMethod::getTableView() const
{
    return tableView;
}

/**
 * @brief SIFormMethod::setTableView
 * @param value
 */
void SIFormMethod::setTableView(QTableView *value)
{
    tableView = value;
}
/**
 * @brief SIFormMethod::getSiFormBean
 * @return
 */
SIFormBean *SIFormMethod::getSiFormBean() const
{
    return siFormBean;
}

/**
 * @brief SIFormMethod::setSiFormBean
 * @param value
 */
void SIFormMethod::setSiFormBean(SIFormBean *value)
{
    siFormBean = value;
}

/**
 * @brief SIFormMethod::getLogFormMethod
 * @return
 */
LogFormMetod *SIFormMethod::getLogFormMethod() const
{
    return logFormMethod;
}

/**
 * @brief SIFormMethod::setLogFormMethod
 * @param value
 */
void SIFormMethod::setLogFormMethod(LogFormMetod *value)
{
    logFormMethod = value;
}

/**
 * @def 初始化函数
 * @brief SIFormMethod::Init
 */
void SIFormMethod::Init()
{
    QLogHelper::instance()->LogInfo("SIFormMethod->Init() 函数执行!");
    excelOperateThread=new SIExcelOperateThread();
    fileOperateThread=new SIFileOperateThread();
}

/**
 * @def 处理连接信号槽函数
 * @brief SIFormMethod::ConnectSlot
 */
void SIFormMethod::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("SIFormMethod->ConnectSlot() 函数执行!");
    connect(this,&SIFormMethod::ShowMessageProcessSignal,this,&SIFormMethod::ShowMessageProcessSlot);
}

/**
 * @连接其他UI 信号槽函数在初始化过程中需要做特殊处理
 * @brief SIFormMethod::ConnectOtherUISlot
 */
void SIFormMethod::ConnectOtherUISlot()
{
    QLogHelper::instance()->LogInfo("SIFormMethod->ConnectOtherUISlot() 函数执行!");
    connect(this,&SIFormMethod::ShowLogMessageSignal,this->logFormMethod,&LogFormMetod::ShowLogMessageSlot);
}

/**
 * @def Table界面初始化显示
 * @brief SIFormMethod::InitTableView
 */
void SIFormMethod::InitTableView()
{
    QLogHelper::instance()->LogInfo("SIFormMethod->InitTableView() 函数执行!");
}

/**
 * @def 判断机种名称是否符合要求
 *      如果机种符合要求，则记录下来，字体设置为正常，如果存在错误码则移除错误码
 *      如果机种不符合要求，记录为空，字体设置为红色，设置错误码
 * @brief SIFormMethod::JudgeIDSlot
 * @param Edit
 * @param ID
 */
void SIFormMethod::JudgeIDSlot(QLineEdit *Edit, QString *ID)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->JudgeIDSlot() 函数执行!");
    QRegExp rx("^EN(3[3-7]|42)\\d\\dP[A-Z]");
    if(rx.indexIn(Edit->text())!=0){
        Edit->setStyleSheet(QString(errFontColor)); //字体相关设置;
        return;
    }
    //对指定对象赋值
    (*ID)=Edit->text();
    Edit->setStyleSheet(QString(nomFontColor)); //字体相关设置;
}
/**
 * @def 判断机种类型
 *      以作成机种(ID)为主导地位
 *      对比依赖机种与作成机种的类型是否一致
 *      如果不一致，则依赖机种显示红色字体
 * @brief SIFormMethod::JudgeIDTypeSlot
 * @param Edit
 * @param srcobject
 * @param desobject
 */
void SIFormMethod::JudgeIDTypeSlot(QLineEdit *Edit, QString *srcobject, QString *desobject)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->JudgeIDTypeSlot() 函数执行!");
    QString ret=siFormBean->getCommonMethod()->JudgeIDType(Edit->text());
    (*srcobject)=ret;
    //对比机种不为空，当前机种类型和对比机种不一致
    if(!(*desobject).isEmpty()&&ret!=(*desobject))
    {
        //错误处理，设置标记位false
        siFormBean->setIDRelyIDflag(false);
        return;
    }
    Edit->setStyleSheet(QString(nomFontColor));
    //正确处理，设置标记位true
    siFormBean->setIDRelyIDflag(true);
}

/**
 * @def 关于Log信息打包出来
 * @brief SIFormMethod::ShowMessageProcessSlot
 * @param flag
 * @param Log_Flag
 */
void SIFormMethod::ShowMessageProcessSlot(const unsigned int flag, const unsigned int Log_Flag)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->ShowLogMessageProcess() 函数执行!");
    unsigned int level;
    QStringList message;
    //对数据包进行处理
    switch (flag) {
    case IDflag:
        level=LOG_INFO;
        message.append("机种番号: "+*siFormBean->getID());
        message.append("机种类型: "+*siFormBean->getIDType());
        break;
    case RelyIDflag:
        level=LOG_INFO;
        message.append("依赖机种番号: "+*siFormBean->getID());
        message.append("依赖机种类型: "+*siFormBean->getIDType());
        break;
    default:
        break;
    }
    switch (Log_Flag) {
    case LOG_ALL:
        //判断显示处理
        this->ShowTableView(message,flag);
        emit ShowLogMessageSignal(message,level);
        break;
    case LOG_LOG:
        //判断显示处理
        emit ShowLogMessageSignal(message,level);
        break;
    case LOG_TABLE:
        this->ShowTableView(message,flag);
        break;
    }
}

/**
 * @def 当前界面TableView显示
 * @brief SIFormMethod::ShowTableView
 * @param tableView
 * @param message
 * @param flag
 */
void SIFormMethod::ShowTableView(const QStringList message, const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->ShowTableView() 函数执行!");
}

/**
 * @def 处理路径选择相关操作
 * @brief SIFormMethod::SelectDirSlot
 * @param label
 * @param objectDir
 */
void SIFormMethod::SelectDirSlot(QLabel *label, QString *objectDir)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->SelectDirSlot() 函数执行!");
    QString dirName =  QFileDialog::getExistingDirectory(label, tr("Open Directory"),siFormBean->getCommonMethod()->desktopDirPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirName.isEmpty()) {
        label->setStyleSheet(QString(errFontColor));
        QMessageBox::warning(label, "Warn", tr("No directory selected!"));
        return;
    }
    label->setStyleSheet(QString(nomFontColor));
    label->setText(dirName);
    *objectDir=dirName;
}

/**
 * @def 处理文件搜索相关操作
 * @brief SIFormMethod::SearchFileSlot
 * @param flag
 * @param isGoON
 */
void SIFormMethod::SearchFileSlot(unsigned int flag, bool isGoON)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->SearchFileSlot() 函数执行!");
    //校验SVN路径是否存在
    if(!QDir(*siFormBean->getSVNDirPath()).exists()){return;}
    //文件检索过滤器
    QStringList filters;

}



