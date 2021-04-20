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
    mythread=new MyThread();
    connect(this,&UIMethod::ActiveThreadSignal,this,&UIMethod::EndFindFileThreadSlot);
    //激活线程，以信号槽的方式
    connect(this,&UIMethod::FindFileThreadSignal,mythread,&MyThread::FindFileThreadSlot);
    //线程处理完，返回主函数，以信号槽方式
    connect(mythread,&MyThread::EndFindFileThreadSignal,this,&UIMethod::EndFindFileThreadSlot);
}
/**
 * @def 线程初始化操作
 * @brief UIMethod::StartThread
 * @param thread
 * @param dirPath
 * @param commonMethod
 * @param filters
 * @param flag
 */
void UIMethod::StartThread(QThread *thread, const QString dirPath, CommonMethod *commonMethod, const QStringList filters, unsigned int flag)
{
    if(thread==NULL){
        thread=new QThread();
        mythread->moveToThread(thread);
    }
    thread->start();
    emit FindFileThreadSignal(dirPath,commonMethod,filters,flag);
}
/**
 * @def 结束线程
 * @brief UIMethod::EndThread
 * @param thread
 */
void UIMethod::EndThread(QThread *thread)
{
    if(thread!=NULL&&thread->isRunning())
    {
        thread->quit();
        thread->wait();
    }
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
void UIMethod::ShowIDmessageSlot(QTextEdit *Edit, int flag)
{
    Edit->append(DATETIME+"=======================================");
    switch (flag) {
    case 0:
        Edit->append(DATETIME+"机种番号: "+comBean->getID());
        Edit->append(DATETIME+"机种类型: "+comBean->getIDType());
        break;
    case 1:
        Edit->append(DATETIME+"依赖机种番号: "+comBean->getRelyID());
        Edit->append(DATETIME+"依赖机种类型: "+comBean->getRelyIDType());
        break;
    default:
        break;
    }
    Edit->append(DATETIME+"=======================================");
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
 *      第一步：获取量产管理表相应的路径
 *      第二步：获取ini模板文件相应的路径
 *      第三步：获取P票文件路径
 *      第四步：获取SW确认文件路径
 *      第五步：获取Mot文件路径
 * @brief UIMethod::SelectFileSlot
 * @param filters
 */
void UIMethod::SelectFileSlot(const QString dirPath)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectFileSlot() 函数执行!");
    if(!QFile::exists(dirPath)){return;}
    comBean->setStatusflag(1);
    emit ActiveThreadSignal(QStringList(),dirPath,2);
}
/**
 * @def 此函数功能是子线程查找文件后回调函数
 *      关于flag说明
 *      2       表示查找 量产管理表文件 完成
 *      3       表示查找 成果物路径下SW确认表 完成
 * @brief UIMethod::EndFindFileThreadSignal
 * @param st
 * @param flag
 */
void UIMethod::EndFindFileThreadSlot(QStringList st,QString dirPath, unsigned int flag)
{
    QLogHelper::instance()->LogInfo("UIMethod->EndFindFileThreadSignal() 函数执行!");
    QStringList filters;
    switch (flag) {
    case 2:
        if(st.isEmpty()){
            filters.append(*(comBean->getIDType())+"*ソフトウエア部品番号管理表(量産)_AKM対応用*.xls");
            this->StartThread(dealFileFileThread,dirPath,comBean->getComMethod(),filters,flag);
            return;
        }else{
            comBean->setRelyFilePath(comBean->getComMethod()->AnalyzeRelyFilePath(st));
            comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),RelyFileError,comBean->getRelyFilePath(),true);
            filters.clear();
            filters.append("コンパイルSW確認結果_*"+*(comBean->getID())+".xlsx");
            this->StartThread(dealFileFileThread,dirPath,comBean->getComMethod(),filters,flag+1);
        }
        break;
    case 3:
        QLogHelper::instance()->LogDebug(comBean->getSWFilePath());
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),SWFileError,comBean->getRelyFilePath(),true);
        break;
    case 4:
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),IniFileError,comBean->getRelyFilePath(),true);
        break;
    case 5:
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),APPFileError,comBean->getRelyFilePath(),true);
        break;
    case 6:
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),JoinFileError,comBean->getRelyFilePath(),true);
        break;
    case 7:
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),PFileError,comBean->getRelyFilePath(),true);
        break;
    case 8:
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),CarInfoFileError,comBean->getRelyFilePath(),true);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),CarMapFileError,comBean->getRelyFilePath(),true);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),CarOSDFileError,comBean->getRelyFilePath(),true);
        comBean->setStatusflag(0);
        return;
        break;
    default:
        break;
    }
    //this->StartThread(dealFileFileThread,dirPath,comBean->getComMethod(),filters,flag+1);
    //emit FindFileThreadSignal(dirPath,comBean->getComMethod(),QStringList() << *(comBean->getIDType())+"_"+*(comBean->getID())+"_AKM火災対応_P票.xls",2);
    //this->EndThread(dealFileFileThread);
}

