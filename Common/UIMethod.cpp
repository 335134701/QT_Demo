#include "UIMethod.h"


UIMethod::UIMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("UIMethod() 构造函数执行!");
    this->Init();
}
/**
 * @brief UIMethod::Init
 */
void UIMethod::Init()
{
    fileThread=new FileThread();
    dealFileFileThread=new QThread();
    fileThread->moveToThread(dealFileFileThread);
    excelOperateThread=new ExcelOperateThread();
    excelThread=new QThread();
    excelOperateThread->moveToThread(excelThread);
    //不同线程之间通过信号和槽来传递自定义数据类型QList<SOFTNUMBERTable>的时候
    qRegisterMetaType<QList<SOFTNUMBERTable>>("QList<SOFTNUMBERTable>");//注册SOFTNUMBERTable类型
    //不同线程之间通过信号和槽来传递自定义数据类型QList<CONFIGTable>的时候
    qRegisterMetaType<QList<CONFIGTable>>("QList<CONFIGTable>");//注册CONFIGTable类型

    connect(this,&UIMethod::ActiveThreadSignal,this,&UIMethod::SelectFileSlot);
    //激活线程，以信号槽的方式
    connect(this,&UIMethod::FindFileThreadSignal,fileThread,&FileThread::FindFileThreadSlot);
    //线程处理完，返回主函数，以信号槽方式
    connect(fileThread,&FileThread::EndFindFileThreadSignal,this,&UIMethod::EndFindFileThreadSlot);
    //LogView显示
    connect(this,&UIMethod::ShowIDmessageSignal,this,&UIMethod::ShowIDmessageSlot);
    //连接解析excel线程
    connect(this,&UIMethod::ExcelOperateThreadSignal,excelOperateThread,&ExcelOperateThread::ExcelOperateThreadSlot);
    //解析excel完成后回调函数连接
    connect(excelOperateThread,&ExcelOperateThread::EndExcelOperateThreadSoftSignal,this,&UIMethod::EndExcelOperateThreadSoftSlot);
    connect(excelOperateThread,&ExcelOperateThread::EndExcelOperateThreadConfSignal,this,&UIMethod::EndExcelOperateThreadConfSlot);
}

QTextEdit *UIMethod::getTextEdit() const
{
    return textEdit;
}

void UIMethod::setTextEdit(QTextEdit *value)
{
    textEdit = value;
}

void UIMethod::setComBean(CommonBean *value)
{
    comBean = value;
}

/**
 * @def LogView 界面操作显示
 * @brief UIMethod::ShowIDmessageSlot
 * @param Edit
 * @param flag
 */
void UIMethod::ShowIDmessageSlot(int flag)
{
    this->getTextEdit()->append(DATETIME+" =======================================");
    switch (flag) {
    case IDflag:
        this->getTextEdit()->append(DATETIME+" 机种番号: "+comBean->getID());
        this->getTextEdit()->append(DATETIME+" 机种类型: "+comBean->getIDType());
        comBean->getMessageViewModel()->setItem(0, 1, new QStandardItem(*comBean->getID()));
        comBean->getMessageViewModel()->item(0,1)->setEditable(false);
        comBean->getMessageViewModel()->setItem(1, 1, new QStandardItem(*comBean->getIDType()));
        comBean->getMessageViewModel()->item(1,1)->setEditable(false);
        break;
    case RelyIDflag:
        this->getTextEdit()->append(DATETIME+" 依赖机种番号: "+comBean->getRelyID());
        this->getTextEdit()->append(DATETIME+" 依赖机种类型: "+comBean->getRelyIDType());
        comBean->getMessageViewModel()->setItem(2, 1, new QStandardItem(*comBean->getRelyID()));
        comBean->getMessageViewModel()->item(2,1)->setEditable(false);
        comBean->getMessageViewModel()->setItem(3, 1, new QStandardItem(*comBean->getRelyIDType()));
        comBean->getMessageViewModel()->item(3,1)->setEditable(false);
        break;
    case RelyFileflag:
        this->getTextEdit()->append(DATETIME+" 量产管理表路径: "+comBean->getRelyFilePath());
        comBean->getMessageViewModel()->setItem(4, 1, new QStandardItem(*comBean->getRelyFilePath()));
        break;
    case IniFileflag:
        this->getTextEdit()->append(DATETIME+" ini配置文件路径: "+comBean->getIniFilePath());
        comBean->getMessageViewModel()->setItem(5, 1, new QStandardItem(*comBean->getIniFilePath()));

        break;
    case PFileflag:
        this->getTextEdit()->append(DATETIME+" P票文件路径: "+comBean->getPFilePath());
        comBean->getMessageViewModel()->setItem(6, 1, new QStandardItem(*comBean->getPFilePath()));
        break;
    case SWFileflag:
        this->getTextEdit()->append(DATETIME+" SW確認文件路径: "+comBean->getSWFilePath());
        comBean->getMessageViewModel()->setItem(7, 1, new QStandardItem(*comBean->getSWFilePath()));
        break;
    case CarInfoFileflag:
        this->getTextEdit()->append(DATETIME+" CarInfo文件路径: "+comBean->getCarInfoFilePath());
        comBean->getMessageViewModel()->setItem(8, 1, new QStandardItem(*comBean->getCarInfoFilePath()));
        break;
    case CarMapFileflag:
        this->getTextEdit()->append(DATETIME+" CarMAP文件路径: "+comBean->getCarMapFilePath());
        comBean->getMessageViewModel()->setItem(9, 1, new QStandardItem(*comBean->getCarMapFilePath()));
        break;
    case CarOSDFileflag:
        this->getTextEdit()->append(DATETIME+" OSD文件路径: "+comBean->getCarOSDFilePath());
        comBean->getMessageViewModel()->setItem(10, 1, new QStandardItem(*comBean->getCarOSDFilePath()));
        break;
    case JoinFileflag:
        this->getTextEdit()->append(DATETIME+" join Mot文件路径: "+comBean->getJoinMot());
        comBean->getMessageViewModel()->setItem(11, 1, new QStandardItem(*comBean->getJoinMot()));
        break;
    case APPFileflag:
        this->getTextEdit()->append(DATETIME+" APP Mot文件路径: "+comBean->getAPPMot());
        comBean->getMessageViewModel()->setItem(12, 1, new QStandardItem(*comBean->getAPPMot()));
        break;
    case EEFileflag:
        this->getTextEdit()->append(DATETIME+" EE-A002-1000 DR会議運用手順文件路径: "+comBean->getEEFilePath());
        comBean->getMessageViewModel()->setItem(13, 1, new QStandardItem(*comBean->getEEFilePath()));
        break;
    case ReadyFileflag:
        this->getTextEdit()->append(DATETIME+" 確認シート文件路径: "+comBean->getReadyFilePath());
        comBean->getMessageViewModel()->setItem(14, 1, new QStandardItem(*comBean->getReadyFilePath()));
        break;
    case ConfigFileflag:
        this->getTextEdit()->append(DATETIME+" EntryAVM採用車種コンフィグ詳細文件路径: "+comBean->getConfigFilePath());
        comBean->getMessageViewModel()->setItem(15, 1, new QStandardItem(*comBean->getConfigFilePath()));
        break;
    }
    this->getTextEdit()->append(DATETIME+" =======================================");
}
/**
 * @def 判断机种名称是否符合要求
 *      如果机种符合要求，则记录下来，字体设置为正常，如果存在错误码则移除错误码
 *      如果机种不符合要求，记录为空，字体设置为红色，设置错误码
 * @brief UIMethod::JudgeIDSlot
 * @param Edit
 */
void UIMethod::JudgeIDSlot(QLineEdit *Edit,QString *objectID)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDSlot() 函数执行!");
    *objectID="";
    //设置正则表达式对象
    QRegExp rx(comBean->getRExpression().value(Edit->objectName()));
    if(rx.indexIn(Edit->text())!=0){
        Edit->setStyleSheet(QString(errFontColor)); //字体相关设置;
        //如果错误码不存在，则添加错误码
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),Edit->objectName(),NULL,true);
        return;
    }
    //对指定对象赋值
    *objectID=Edit->text();
    Edit->setStyleSheet(QString(nomFontColor)); //字体相关设置;
    //如果存在错误码,则移除错误码
    comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),Edit->objectName(),NULL,false);
}
/**
 * @def 判断机种类型
 *      以作成机种(ID)为主导地位
 *      以依赖机种(RelyID)为辅导地位
 *      对比依赖机种与作成机种的类型是否一致
 *      如果不一致，则依赖机种显示红色字体
 * @brief UIMethod::JudgeIDTypeSlot
 * @param Edit
 * @param srcobject
 * @param desobject
 */
void UIMethod::JudgeIDTypeSlot(QLineEdit *Edit,QString *srcobject,QString *desobject)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDTypeSlot() 函数执行!");
    QString ret=comBean->getComMethod()->JudgeIDType(Edit->text());
    (*srcobject)=ret;
    //对比机种不为空，当前机种类型和对比机种不一致
    if(!(*desobject).isEmpty()&&ret!=(*desobject))
    {
        if(Edit->objectName()!="IDEdit"){
            Edit->setStyleSheet(QString(errFontColor));
            (*srcobject)="";
        }
        //错误处理，添加错误码
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),IDRelyID,NULL,true);
        return;
    }
    Edit->setStyleSheet(QString(nomFontColor));
    //正确处理，如果存在错误码，则移除
    comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),IDRelyID,NULL,false);
}
/**
 * @def 按钮选择目录并赋值
 * @brief UIMethod::SelectDir
 * @param label
 * @param destDirPath
 * @param dirPath
 */
void UIMethod::SelectDirSlot(QLabel *label,QString *objectID,const QString errName)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectDirSlot() 函数执行!");
    label->setText("");
    QString dirName =  QFileDialog::getExistingDirectory(label, tr("Open Directory"),comBean->desktopDirPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirName.isEmpty()) {
        *objectID="";
        label->setStyleSheet(QString(errFontColor));
        //如果错误码不存在，则添加错误码
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),errName,NULL,true);
        QMessageBox::warning(label, "Warn", tr("No directory selected!"));
        return;
    }
    label->setStyleSheet(QString(nomFontColor));
    //正确处理，如果存在错误码，则移除
    comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),errName,NULL,false);
    label->setText(dirName);
    *objectID=dirName;
}
/**
 * @def 获取相应的文件路径
 * @brief UIMethod::SelectFileSlot
 * @param filters
 */
void UIMethod::SelectFileSlot(QString dirPath,unsigned int flag, bool goOn)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectFileSlot() 函数执行!");
    if(!QDir(dirPath).exists()){
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),*(comBean->getSVNDirPath()),NULL,true);
        return;
    }
    QStringList filters;
    if(!dealFileFileThread->isRunning())
    {
        comBean->setStatusflag(1);
        dealFileFileThread->start();
    }
    switch (flag) {
    case RelyFileflag:
        filters.append(*(comBean->getIDType())+"*ソフトウエア部品番号管理表(量産)_AKM対応用*.xls");
        break;
    case IniFileflag:
        filters.append("LOGZONE_*_2nd.ini");
        break;
    case PFileflag:
        if(!comBean->getRelyID()->isEmpty()){
            filters.append("*_"+*(comBean->getID())+"_AKM火災対応_P票.xls");
            filters.append(*(comBean->getRelyID())+"*と共有する.txt");
        }else{
            filters.append("*_"+*(comBean->getID())+"_AKM火災対応_P票.xls");
        }
        break;
    case SWFileflag:
        if(!comBean->getRelyID()->isEmpty()){
            filters.append("コンパイルSW確認結果_*"+*(comBean->getID())+".xlsx");
            filters.append(*(comBean->getRelyID())+"*と共有する.txt");
        }else{
            filters.append("コンパイルSW確認結果_*"+*(comBean->getID())+".xlsx");
        }
        break;
    case CarInfoFileflag:
        if(!comBean->getErrCode()->value(CarInfoFileError).ID.isEmpty()){
            if(!comBean->getCarMapFilePath()->isEmpty()){
                dirPath=comBean->getCarMapFilePath()->left(comBean->getCarMapFilePath()->lastIndexOf("/"));
            }
            else if(!comBean->getCarOSDFilePath()->isEmpty()){
                dirPath=comBean->getCarMapFilePath()->left(comBean->getCarMapFilePath()->lastIndexOf("/"));
            }
        }
        if(!comBean->getRelyID()->isEmpty()){
            filters.append("*"+*(comBean->getID())+"*_CarInfo.mot");
            filters.append("*"+*(comBean->getRelyID())+"*_CarInfo.mot");
        }else{
            filters.append("*"+*(comBean->getID())+"*_CarInfo.mot");
        }
        break;
    case CarMapFileflag:
        if(!comBean->getErrCode()->value(CarMapFileError).ID.isEmpty()){
            if(!comBean->getIniFilePath()->isEmpty()){
                dirPath=comBean->getIniFilePath()->left(comBean->getIniFilePath()->lastIndexOf("/"));
            }
            else if(!comBean->getCarOSDFilePath()->isEmpty()){
                dirPath=comBean->getCarMapFilePath()->left(comBean->getCarMapFilePath()->lastIndexOf("/"));
            }
        }
        if(!comBean->getRelyID()->isEmpty()){
            filters.append("*"+*(comBean->getID())+"*_CameraMAP.mot");
            filters.append("*"+*(comBean->getRelyID())+"*_CameraMAP.mot");
        }else{
            filters.append("*"+*(comBean->getID())+"*_CameraMAP.mot");
        }
        break;
    case CarOSDFileflag:
        if(!comBean->getErrCode()->value(CarOSDFileError).ID.isEmpty()){
            if(!comBean->getIniFilePath()->isEmpty()){
                dirPath=comBean->getIniFilePath()->left(comBean->getIniFilePath()->lastIndexOf("/"));
            }
            else if(!comBean->getCarMapFilePath()->isEmpty()){
                dirPath=comBean->getCarMapFilePath()->left(comBean->getCarMapFilePath()->lastIndexOf("/"));
            }
        }
        if(!comBean->getRelyID()->isEmpty()){
            filters.append("*"+*(comBean->getID())+"*_OSD.mot");
            filters.append("*"+*(comBean->getRelyID())+"*_OSD.mot");
        }else{
            filters.append("*"+*(comBean->getID())+"*_OSD.mot");
        }
        break;
    case JoinFileflag:
        filters.append("join_*.mot");
        break;
    case APPFileflag:
        filters.append("APP_*.mot");
        filters.append("App.motは*共有する.txt");
        break;
    case EEFileflag:
        //filters.append(*(comBean->getIDType())+"*ソフトウエア部品番号管理表(量産)_AKM対応用*.xls");
        break;
    case ReadyFileflag:
        if(!comBean->getRelyID()->isEmpty()){}else{

        }
        //filters.append(*(comBean->getIDType())+"*ソフトウエア部品番号管理表(量産)_AKM対応用*.xls");
        break;
    case ConfigFileflag:
        filters.append(*(comBean->getIDType())+"*採用車種コンフィグ詳細*.xls");
        break;
    }
    dirPath=comBean->getComMethod()->AnalyzePath(dirPath,*(comBean->getID()),*(comBean->getIDType()),flag);
    QLogHelper::instance()->LogDebug(dirPath);
    //根据需求发送
    emit FindFileThreadSignal(dirPath,comBean->getComMethod(),filters,flag,goOn);
}
/**
 * @def 此函数功能是子线程查找文件后回调函数
 * @brief UIMethod::EndFindFileThreadSignal
 * @param st
 * @param flag
 */
void UIMethod::EndFindFileThreadSlot(QStringList st, unsigned int flag, bool goOn)
{
    QLogHelper::instance()->LogInfo("UIMethod->EndFindFileThreadSignal() 函数执行!");
    switch (flag) {
    case RelyFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getRelyFilePath(),flag);
        if(!comBean->getRelyFilePath()->isEmpty()&&!excelThread->isRunning()){
            excelThread->start();
            emit ExcelOperateThreadSignal(comBean->getExcelOption(),*(comBean->getRelyFilePath()),*(comBean->getID()),*(comBean->getIDType()),flag);
        }
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),RelyFileError,*(comBean->getRelyFilePath()),true);
        break;
    case IniFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getIniFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),IniFileError,*(comBean->getIniFilePath()),true);
        break;
    case PFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getPFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),PFileError,*(comBean->getPFilePath()),true);
        break;
    case SWFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getSWFilePath(),flag);
        QLogHelper::instance()->LogInfo(*comBean->getSWFilePath());
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),SWFileError,*(comBean->getSWFilePath()),true);
        break;
    case CarInfoFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getCarInfoFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),CarInfoFileError,*(comBean->getCarInfoFilePath()),true);
        break;
    case CarMapFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getCarMapFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),CarMapFileError,*(comBean->getCarMapFilePath()),true);
        break;
    case CarOSDFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getCarOSDFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),CarOSDFileError,*(comBean->getCarOSDFilePath()),true);
        //因为周边软件的机种番号可能与目标机种番号不一致，需要做处理
        //处理逻辑是：先获取CarInfo，CarMap mot文件路径
        //没有获取到三个对应的文件，则对路径做处理，再获取一次，如果还是没获取到对应文件的路径，则不进行处理
        /*if(comBean->getCarInfoFilePath()->isEmpty())
        {
            emit ActiveThreadSignal(*(comBean->getSVNDirPath()),flag-2,goOn);
            return;
        }
        if(comBean->getCarInfoFilePath()->isEmpty())
        {
            emit ActiveThreadSignal(*(comBean->getSVNDirPath()),flag-1,goOn);
            return;
        }
        if(comBean->getCarInfoFilePath()->isEmpty())
        {
            emit ActiveThreadSignal(*(comBean->getSVNDirPath()),flag,goOn);
            return;
        }*/
        break;
    case JoinFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getJoinMot(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),JoinFileError,*(comBean->getJoinMot()),true);
        break;
    case APPFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getAPPMot(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),APPFileError,*(comBean->getAPPMot()),true);
        break;
    case EEFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getEEFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),EEFileError,*(comBean->getEEFilePath()),true);
        break;
    case ReadyFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getReadyFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),ReadyFileError,*(comBean->getReadyFilePath()),true);
        break;
    case ConfigFileflag:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getConfigFilePath(),flag);
        if(!comBean->getConfigFilePath()->isEmpty()&&!excelThread->isRunning()){
            excelThread->start();
            emit ExcelOperateThreadSignal(comBean->getExcelOption(),*(comBean->getConfigFilePath()),*(comBean->getID()),*(comBean->getConfigFilePath()),flag);
        }
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),ConfigFileError,*(comBean->getConfigFilePath()),true);
        break;
    }
    emit ShowIDmessageSignal(flag);
    if(goOn){emit ActiveThreadSignal(*(comBean->getSVNDirPath()),flag+1,goOn);}
    else{
        dealFileFileThread->quit();
        dealFileFileThread->wait();
        comBean->setStatusflag(0);
    }
}
/**
 * @def 量产管理表解析完成后回调函数
 * @brief UIMethod::EndExcelOperateThreadSoftSlot
 * @param list
 */
void UIMethod::EndExcelOperateThreadSoftSlot(QList<SOFTNUMBERTable> list)
{
    QLogHelper::instance()->LogInfo("UIMethod->EndExcelOperateThreadSoftSlot() 函数执行!");
    QLogHelper::instance()->LogDebug(QString::number(list.size()));
    comBean->setSoftNumberTable(&list);
    comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),ConfigFileError,QString::number(comBean->getSoftNumberTable()->size()),true);
    excelThread->quit();
    excelThread->wait();
}
/**
 * @def EntryAVM採用車種コンフィグ詳細表解析完成后回调函数
 * @brief UIMethod::EndExcelOperateThreadConfSlot
 * @param list
 */
void UIMethod::EndExcelOperateThreadConfSlot(QList<CONFIGTable> list)
{
    QLogHelper::instance()->LogInfo("UIMethod->EndExcelOperateThreadConfSlot() 函数执行!");
    QLogHelper::instance()->LogDebug(QString::number(list.size()));
    comBean->setConfigTable(&list);
    comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),ConfigFileError,QString::number(comBean->getConfigTable()->size()),true);
    excelThread->quit();
    excelThread->wait();
}

/**
 * @def MessageView单元格数据修改触发函数
 * @brief UIMethod::MessageViewModelEditedSlot
 * @param item
 */
void UIMethod::MessageViewModelEditedSlot(const QStandardItem *item)
{
    QLogHelper::instance()->LogInfo("AutomationTool->MessageViewModelEditedSlot() 函数触发执行!");
    QLogHelper::instance()->LogDebug("column: "+QString::number(item->column())+"    row:"+QString::number(item->row()));
    QLogHelper::instance()->LogDebug(item->text());
}

