#include "AutomationTool.h"
#include "ui_AutomationTool.h"

AutomationTool::AutomationTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AutomationTool)
{
    QLogHelper::instance()->LogInfo("AutomationTool() 构造函数执行!");
    ui->setupUi(this);
    this->InitStyle();
    this->Init();
    this->InitTableView();
    this->ConnectSlot();
}
/**
 * @brief AutomationTool::~AutomationTool
 */
AutomationTool::~AutomationTool()
{
    QLogHelper::instance()->LogInfo("AutomationTool() 执行结束,删除UI对象!");
    delete ui;
}
/**
 * @def UI初始化过程中需要预处理操作
 * @brief AutomationTool::Init
 */
void AutomationTool::Init()
{
    QLogHelper::instance()->LogInfo("AutomationTool->init() 函数执行!");
    comBean=new CommonBean();
    uiMethod=new UIMethod();
    uiMethod->setComBean(comBean);
    uiMethod->setTextEdit(ui->LogView);
}
/**
 * @def UI连接信号槽函数
 * @brief AutomationTool::ConnectSlot
 */
void AutomationTool::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("AutomationTool->ConnectSlot() 函数执行!");
    //输入ID判断信号槽连接
    connect(this,&AutomationTool::JudgeIDSignal,this->uiMethod,&UIMethod::JudgeIDSlot);
    //输入ID类型判断信号槽连接
    connect(this,&AutomationTool::JudgeIDTypeSignal,this->uiMethod,&UIMethod::JudgeIDTypeSlot);
    //信息显示信号槽连接
    connect(this,&AutomationTool::ShowIDmessageSignal,this->uiMethod,&UIMethod::ShowIDmessageSlot);
    connect(this,&AutomationTool::SelectDirSignal,this->uiMethod,&UIMethod::SelectDirSlot);
    connect(logViewClearAction,&QAction::triggered,this,&AutomationTool::LogViewClearSlot);
    connect(this,&AutomationTool::SelectFileSignal,this->uiMethod,&UIMethod::SelectFileSlot);
    connect(this,&AutomationTool::CreateSignal,this->uiMethod,&UIMethod::CreateSlot);
}

/**
 * @brief AutomationTool::InitTableView
 */
void AutomationTool::InitTableView()
{
    comBean->setMessageViewModel(new QStandardItemModel());
    ui->MessageView->setModel(comBean->getMessageViewModel());
    comBean->getMessageViewModel()->setItem(0, 0, new QStandardItem("机种番号:"));
    comBean->getMessageViewModel()->item(0,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(0,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(0, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->item(0,1)->setEditable(false);
    comBean->getMessageViewModel()->setItem(1, 0, new QStandardItem("机种类型:"));
    comBean->getMessageViewModel()->item(1,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(1,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(1, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->item(1,1)->setEditable(false);
    comBean->getMessageViewModel()->setItem(2, 0, new QStandardItem("依赖机种番号:"));
    comBean->getMessageViewModel()->item(2,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(2,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(2, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->item(2,1)->setEditable(false);
    comBean->getMessageViewModel()->setItem(3, 0, new QStandardItem("依赖机种类型:"));
    comBean->getMessageViewModel()->item(3,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(3,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(3, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->item(3,1)->setEditable(false);
    comBean->getMessageViewModel()->setItem(4, 0, new QStandardItem("量产管理表路径:"));
    comBean->getMessageViewModel()->item(4,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(4,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(4, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(5, 0, new QStandardItem("INI模板文件路径:"));
    comBean->getMessageViewModel()->item(5,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(5,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(5, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(6, 0, new QStandardItem("P票文件路径"));
    comBean->getMessageViewModel()->item(6,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(6,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(6, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(7, 0, new QStandardItem("SW确认文件路径:"));
    comBean->getMessageViewModel()->item(7,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(7,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(7, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(8, 0, new QStandardItem("CarInfoMot文件路径:"));
    comBean->getMessageViewModel()->item(8,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(8,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(8, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(9, 0, new QStandardItem("CarMapMot文件路径:"));
    comBean->getMessageViewModel()->item(9,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(9,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(9, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(10, 0, new QStandardItem("OSDMot文件路径:"));
    comBean->getMessageViewModel()->item(10,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(10,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(10, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(11, 0, new QStandardItem("appMot文件路径:"));
    comBean->getMessageViewModel()->item(11,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(11,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(11, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(12, 0, new QStandardItem("joinMot文件路径:"));
    comBean->getMessageViewModel()->item(12,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(12,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(12, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(13, 0, new QStandardItem("DR会議運用手順文件路径:"));
    comBean->getMessageViewModel()->item(13,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(13,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(13, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(14, 0, new QStandardItem("確認シート文件路径:"));
    comBean->getMessageViewModel()->item(14,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(14,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(14, 1, new QStandardItem(""));
    comBean->getMessageViewModel()->setItem(15, 0, new QStandardItem("採用車種文件路径:"));
    comBean->getMessageViewModel()->item(15,0)->setTextAlignment(Qt::AlignRight);
    comBean->getMessageViewModel()->item(15,0)->setEditable(false);
    comBean->getMessageViewModel()->setItem(15, 1, new QStandardItem(""));
    ui->MessageView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->MessageView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);//对第0列单独设置固定宽度
    ui->MessageView->setColumnWidth(0,220);
    connect(comBean->getMessageViewModel(),&QStandardItemModel::itemChanged,uiMethod,&UIMethod::MessageViewModelEditedSlot);
}
/**
 * @def UI界面初始化函数，主要功能是美化UI
 * @brief AutomationTool::InitStyle
 */
void AutomationTool::InitStyle()
{
    QLogHelper::instance()->LogInfo("AutomationTool->initStyle() 函数执行!");
    //加载样式表
    QFile file(":/qss.css");
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        file.close();
    }
    logViewClearAction=new QAction("清除显示");
    //为Logview添加清除操作
    ui->LogView->addAction(logViewClearAction);
    //设置TableView自适应
    ui->MessageView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->MessageView->verticalHeader()->setStretchLastSection(false);
    //ui->MessageView->setSelectionBehavior(QAbstractItemView::SelectRows);
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
    if(comBean==NULL||MessageWarn()){return;}
    //如果机种番号变更，则需要重新初始化
    comBean->ResetParameter();
    //初始化TableView显示
    InitTableView();
    //判断机种名称是否符合要求
    emit JudgeIDSignal(ui->IDEdit,comBean->getID());
    if(comBean->getID()->isEmpty()){return;}
    //分析机种类型
    emit JudgeIDTypeSignal(ui->IDEdit,comBean->getIDType(),comBean->getRelyIDType());
    //如果ID和依赖ID类型不一致，则依赖ID会显示红色
    if(!comBean->getIDRelyIDflag()){
        ui->RelyIDEdit->setStyleSheet(QString(errFontColor));
        comBean->setIDRelyIDflag(true);
    }else{
        ui->RelyIDEdit->setStyleSheet(QString(nomFontColor));
    }
    //LogView界面显示信息
    emit ShowIDmessageSignal(IDflag);
}
/**
 * @def 机种RelyIDEdit文本改变完成触发函数
 * @brief AutomationTool::on_RelyIDEdit_editingFinished
 */
void AutomationTool::on_RelyIDEdit_editingFinished()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_RelyIDEdit_editingFinished() 函数触发执行!");
    if(comBean==NULL||MessageWarn()){return;}
    //如果RelyIDEdit文本输入为空或者ID与依赖ID一致，则说明不依赖任何机种
    if(ui->RelyIDEdit->text().isEmpty()||ui->RelyIDEdit->text()==(*comBean->getID())){
        ui->RelyIDEdit->setText("");
        return;
    }
    //判断机种名称是否符合要求
    emit JudgeIDSignal(ui->RelyIDEdit,comBean->getRelyID());
    if(comBean->getRelyID()->isEmpty()){return;}
    //判断依赖机种是否和作成机种同一种类型
    emit JudgeIDTypeSignal(ui->RelyIDEdit,comBean->getRelyIDType(),comBean->getIDType());
    //如果当前机种与依赖机种类型不一致，则设置依赖机种字体为红色字体
    if(!comBean->getIDRelyIDflag()){
        ui->RelyIDEdit->setStyleSheet(QString(errFontColor));
        comBean->setIDRelyIDflag(true);
    }else{
        ui->RelyIDEdit->setStyleSheet(QString(nomFontColor));
    }
    emit ShowIDmessageSignal(RelyIDflag);
}
/**
 * @def
 * @brief AutomationTool::on_SVNButton_clicked
 */
void AutomationTool::on_SVNButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_SVNButton_clicked() 函数触发执行!");
    if(comBean==NULL||MessageWarn()){return;}
    comBean->setSVNDirPath(new QString());
    //获取相应文件路径
    emit SelectDirSignal(ui->SVNLabel,comBean->getSVNDirPath());
}

/**
 * @def
 * @brief AutomationTool::on_OutputButton_clicked
 */
void AutomationTool::on_OutputButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_OutputButton_clicked() 函数触发执行!");
    if(comBean==NULL||MessageWarn()){return;}
    comBean->setOutputDirPath(new QString());
    //生成路径获取
    emit SelectDirSignal(ui->OutputLabel,comBean->getOutputDirPath());
}
/**
 * @def
 * @brief AutomationTool::on_CreateButton_clicked
 */
void AutomationTool::on_CreateButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_CreateButton_clicked() 函数触发执行!");
    if(comBean==NULL||comBean->getID()->isEmpty()||MessageWarn()){return;}
    emit CreateSignal();
}
/**
 * @brief AutomationTool::LogViewClearSlot
 */
void AutomationTool::LogViewClearSlot()
{
    QLogHelper::instance()->LogInfo("AutomationTool->LogViewClearSlot() 函数触发执行!");
    ui->LogView->clear();
}
/**
 * @def 执行某项操作时,其他操作不可执行提示
 *      关于 Statusflag 表示说明：
 *          0 表示无任何操作
 *          1 表示正在查找文件
 *          2 表示正在解析文件
 *          3 表示正在生成相应的文件目录结构
 * @brief AutomationTool::MessageWarn
 * @return
 */
bool AutomationTool::MessageWarn()
{
    QLogHelper::instance()->LogInfo("AutomationTool->MessageWarn() 函数执行!");
    if(comBean->getStatusflag()!=0)
    {
        switch (comBean->getStatusflag()) {
        case 1:
            QMessageBox::warning(this,"Warn","正在执行文件查找任务，其他任务暂时无法执行!");
            break;
        case 2:
            QMessageBox::warning(this,"Warn","正在执行文件解析任务，其他任务暂时无法执行!");
            break;
        case 3:
            QMessageBox::warning(this,"Warn","正在执行生成任务，其他任务暂时无法执行!");
            break;
        default:
            break;
        }
        return true;
    }
    return false;
}
/**
 * @brief AutomationTool::on_MessageView_doubleClicked
 * @param index
 */
void AutomationTool::on_MessageView_doubleClicked(const QModelIndex &index)
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_MessageView_doubleClicked() 函数触发执行!");
    if(comBean==NULL||MessageWarn()){return;}
    if(!comBean->getTableViewEditflag()&&(index.row()>3&&index.column()>=1)&&index.row()<=(DefineTableSize+comBean->getSoftNumberTable()->size())){
        comBean->setTableViewEditflag(true);
    }
}
/**
 * @def 文件检索触发槽函数
 * @brief AutomationTool::on_FileSearchButton_clicked
 */
void AutomationTool::on_FileSearchButton_clicked()
{
    QLogHelper::instance()->LogInfo("AutomationTool->on_FileSearchButton_clicked() 函数触发执行!");
    if(comBean==NULL||MessageWarn()){return;}
    if(comBean->getID()->isEmpty()||comBean->getSVNDirPath()->isEmpty()){
        QMessageBox::warning(this,"Title","机种番号为空或SVN路径为空!");
        return;
    }
    //发送信号，执行文件检索任务
    emit SelectFileSignal(*(comBean->getSVNDirPath()),RelyFileflag,true);
}
