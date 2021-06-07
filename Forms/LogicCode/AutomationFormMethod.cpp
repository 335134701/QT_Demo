#include "AutomationFormMethod.h"

/**
 * @brief AutomationFormMethod::AutomationFormMethod
 * @param parent
 */
AutomationFormMethod::AutomationFormMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod 构造函数执行!");
    this->Init();
    this->ConnectSlot();
    this->InitTableView();
}

/**
 * @brief AutomationFormMethod::getTableView
 * @return
 */
QTableView *AutomationFormMethod::getTableView() const
{
    return tableView;
}

/**
 * @brief AutomationFormMethod::setTableView
 * @param value
 */
void AutomationFormMethod::setTableView(QTableView *value)
{
    tableView = value;
}

/**
 * @brief AutomationFormMethod::getAutomationFormBean
 * @return
 */
AutomationFormBean *AutomationFormMethod::getAutomationFormBean() const
{
    return automationFormBean;
}

/**
 * @brief AutomationFormMethod::setAutomationFormBean
 * @param value
 */
void AutomationFormMethod::setAutomationFormBean(AutomationFormBean *value)
{
    automationFormBean = value;
}

/**
 * @brief AutomationFormMethod::getLogFormMethod
 * @return
 */
LogFormMetod *AutomationFormMethod::getLogFormMethod() const
{
    return logFormMethod;
}

/**
 * @brief AutomationFormMethod::setLogFormMethod
 * @param value
 */
void AutomationFormMethod::setLogFormMethod(LogFormMetod *value)
{
    logFormMethod = value;
}

/**
 * @brief AutomationFormMethod::Init
 */
void AutomationFormMethod::Init()
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod->Init() 函数执行!");
    fileThread=new QThread();
    excelThread=new QThread();
    auCommonMethod=new AuCommonMethod();
    excelOperateThread=new AuExcelOperateThread();
    fileOperateThread=new AuFileOperateThread();
    fileOperateThread->moveToThread(fileThread);
    excelOperateThread->moveToThread(excelThread);
}

/**
 * @brief AutomationFormMethod::ConnectSlot
 */
void AutomationFormMethod::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod->ConnectSlot() 函数执行!");
    connect(this,&AutomationFormMethod::ShowMessageProcessSignal,this,&AutomationFormMethod::ShowMessageProcessSlot);
    //文件检索及文件检索完成后回调处理函数
    connect(this,&AutomationFormMethod::SearchFileSignal,this,&AutomationFormMethod::SearchFileSlot);
    connect(this,&AutomationFormMethod::FileSearchSignal,this->fileOperateThread,&AuFileOperateThread::FileSearchSlot);
    connect(this->fileOperateThread,&AuFileOperateThread::EndFileSearcSignal,this,&AutomationFormMethod::EndFileSearcSlot);
}

/**
 * @brief AutomationFormMethod::ConnectOtherUISlot
 */
void AutomationFormMethod::ConnectOtherUISlot()
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod->ConnectOtherUISlot() 函数执行!");
    connect(this,&AutomationFormMethod::ShowLogMessageSignal,this->logFormMethod,&LogFormMetod::ShowLogMessageSlot);
}


/**
 * @brief AutomationFormMethod::InitTableView
 */
void AutomationFormMethod::InitTableView()
{
    QLogHelper::instance()->LogInfo("AutomationFormMethod->InitTableView() 函数执行!");
}

/**
 * @def 当前界面TableView显示
 * @brief AutomationFormMethod::ShowTableView
 * @param tableView
 * @param message
 * @param flag
 */
void AutomationFormMethod::ShowTableView(const QStringList message, const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->ShowTableView() 函数执行!");
}



/**
 * @def 判断机种名称是否符合要求
 *      如果机种符合要求，则记录下来，字体设置为正常，如果存在错误码则移除错误码
 *      如果机种不符合要求，记录为空，字体设置为红色，设置错误码
 * @brief SIFormMethod::JudgeIDSlot
 * @param Edit
 * @param ID
 */
void AutomationFormMethod::JudgeIDSlot(QLineEdit *Edit, QString *ID)
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
 * @brief AutomationFormMethod::JudgeIDTypeSlot
 * @param Edit
 * @param srcobject
 * @param desobject
 */
void AutomationFormMethod::JudgeIDTypeSlot(QLineEdit *Edit, QString *srcobject, QString *desobject)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->JudgeIDTypeSlot() 函数执行!");
    QString ret=auCommonMethod->JudgeIDType(Edit->text());
    (*srcobject)=ret;
    //对比机种不为空，当前机种类型和对比机种不一致
    if(!(*desobject).isEmpty()&&ret!=(*desobject))
    {
        //错误处理，设置标记位false
        automationFormBean->setIDRelyIDflag(false);
        return;
    }
    Edit->setStyleSheet(QString(nomFontColor));
    //正确处理，设置标记位true
    automationFormBean->setIDRelyIDflag(true);
}

/**
 * @def 关于Log信息打包出来
 * @brief AutomationFormMethod::ShowMessageProcessSlot
 * @param flag
 * @param Log_Flag
 */
void AutomationFormMethod::ShowMessageProcessSlot(const unsigned int flag, const unsigned int Log_Flag)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->ShowLogMessageProcess() 函数执行!");
    unsigned int level;
    QStringList message;
    //对数据包进行处理
    switch (flag) {
    case AuIDflag:
        level=LOG_INFO;
        message.append("机种番号: "+*automationFormBean->getID());
        message.append("机种类型: "+*automationFormBean->getIDType());
        break;
    case AuRelyIDflag:
        level=LOG_INFO;
        message.append("依赖机种番号: "+*automationFormBean->getID());
        message.append("依赖机种类型: "+*automationFormBean->getIDType());
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
 * @def 处理路径选择相关操作
 * @brief AutomationFormMethod::SelectDirSlot
 * @param label
 * @param objectDir
 */
void AutomationFormMethod::SelectDirSlot(QLabel *label, QString *objectDir)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->SelectDirSlot() 函数执行!");
    QString dirName =  QFileDialog::getExistingDirectory(label, tr("Open Directory"),automationFormBean->getCommonMethod()->desktopDirPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
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
 * @brief AutomationFormMethod::SearchFileSlot
 * @param flag
 * @param isGoON
 */
void AutomationFormMethod::SearchFileSlot(unsigned int flag, bool isGoON)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->SearchFileSlot() 函数执行!");
    /*
    //校验SVN路径是否存在
    if(!QDir(*siFormBean->getSVNDirPath()).exists()){return;}
    //文件检索过滤器
    QStringList filters;
    */

}

/**
 * @brief AutomationFormMethod::EndFileSearcSlot
 * @param filePath
 * @param flag
 * @param isGoON
 */
void AutomationFormMethod::EndFileSearcSlot(const QString filePath, unsigned int flag, bool isGoON)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->EndFileSearcSlot() 函数执行!");
    unsigned int log_Flag=0;
    switch (flag) {
    case AuRelyFileflag:
        log_Flag=LOG_ALL;
        (*automationFormBean->getRelyFilePath())=filePath;
        break;
    }
    emit ShowMessageProcessSignal(flag,log_Flag);
    if(isGoON){
        emit SearchFileSignal(flag,isGoON);
    }
    else{
        fileThread->quit();
        fileThread->wait();
        automationFormBean->setAumationStatus(AU_READY);
    }
}

