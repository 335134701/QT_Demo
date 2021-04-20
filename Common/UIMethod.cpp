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
    connect(this,&UIMethod::ActiveThreadSignal,this,&UIMethod::SelectFileSlot);
    //激活线程，以信号槽的方式
    connect(this,&UIMethod::FindFileThreadSignal,mythread,&MyThread::FindFileThreadSlot);
    //线程处理完，返回主函数，以信号槽方式
    connect(mythread,&MyThread::EndFindFileThreadSignal,this,&UIMethod::EndFindFileThreadSlot);
    //LogView显示
    connect(this,&UIMethod::ShowIDmessageSignal,this,&UIMethod::ShowIDmessageSlot);
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
 *      flag 说明:
 *      0   无任何表示
 *      1   表示机种番号信息获取
 *      2   表示机种类型信息获取
 *      3   表示量产管理表信息获取
 *      4   表示Ini文件信息获取
 *      5   表示P票信息获取
 *      6   表示SW确认文件获取
 *      7   表示CarInfoMot文件获取
 *      8   表示CarMapMot文件获取
 *      9   表示OSDMot文件获取
 *      10   表示 joinMot 文件获取
 *      11   表示 appMot   文件获取
 * @brief UIMethod::ShowIDmessageSlot
 * @param Edit
 * @param flag
 */
void UIMethod::ShowIDmessageSlot(int flag)
{
    this->getTextEdit()->append(DATETIME+"=======================================");
    switch (flag) {
    case 1:
        this->getTextEdit()->append(DATETIME+"机种番号: "+comBean->getID());
        this->getTextEdit()->append(DATETIME+"机种类型: "+comBean->getIDType());
        break;
    case 2:
        this->getTextEdit()->append(DATETIME+"依赖机种番号: "+comBean->getRelyID());
        this->getTextEdit()->append(DATETIME+"依赖机种类型: "+comBean->getRelyIDType());
        break;
    case 3:
        this->getTextEdit()->append(DATETIME+"量产管理表路径: "+comBean->getRelyFilePath());
        break;
    case 4:
        this->getTextEdit()->append(DATETIME+"ini配置文件路径: "+comBean->getIniFilePath());
        break;
    case 5:
        this->getTextEdit()->append(DATETIME+"P票文件路径: "+comBean->getPFilePath());
        break;
    case 6:
        this->getTextEdit()->append(DATETIME+"SW確認文件路径: "+comBean->getRelyIDType());
        break;
    case 7:
        this->getTextEdit()->append(DATETIME+"CarInfo文件路径: "+comBean->getCarInfoFilePath());
        break;
    case 8:
        this->getTextEdit()->append(DATETIME+"CarMAP文件路径: "+comBean->getCarMapFilePath());
        break;
    case 9:
        this->getTextEdit()->append(DATETIME+"OSD文件路径: "+comBean->getCarOSDFilePath());
        break;
    case 10:
        this->getTextEdit()->append(DATETIME+"join Mot文件路径: "+comBean->getJoinMot());
        break;
    case 11:
        this->getTextEdit()->append(DATETIME+"APP Mot文件路径: "+comBean->getAPPMot());
        break;
    default:
        break;
    }
    this->getTextEdit()->append(DATETIME+"=======================================");
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
 *      flag 说明:
 *      0   无任何表示
 *      1   表示机种番号信息获取
 *      2   表示机种类型信息获取
 *      3   表示量产管理表信息获取
 *      4   表示Ini文件信息获取
 *      5   表示P票信息获取
 *      6   表示SW确认文件获取
 *      7   表示CarInfoMot文件获取
 *      8   表示CarMapMot文件获取
 *      9   表示OSDMot文件获取
 *      10   表示 joinMot 文件获取
 *      11   表示 appMot   文件获取
 *      12
 *      13
 * @brief UIMethod::SelectFileSlot
 * @param filters
 */
void UIMethod::SelectFileSlot(QString dirPath,unsigned int flag)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectFileSlot() 函数执行!");
    if(!QFile::exists(dirPath)){return;}
    QStringList filters;
    if(dealFileFileThread==NULL)
    {
        comBean->setStatusflag(1);
        dealFileFileThread=new QThread();
        mythread->moveToThread(dealFileFileThread);
        dealFileFileThread->start();
    }
    switch (flag) {
    case 3:
        filters.append(*(comBean->getIDType())+"*ソフトウエア部品番号管理表(量産)_AKM対応用*.xls");
        break;
    case 4:

        break;
    case 5:

        break;
    case 6:
        filters.append("コンパイルSW確認結果_*"+*(comBean->getID())+".xlsx");
        break;
    case 7:

        break;
    case 8:

        break;
    case 9:

        break;
    case 10:

        break;
    case 11:

        break;
    case 12:

        break;
    case 13:

        break;
    }
    dirPath=comBean->getComMethod()->AnalyzePath(dirPath,flag);
    //根据需求发送
    emit FindFileThreadSignal(dirPath,comBean->getComMethod(),filters,flag);
}
/**
 * @def 此函数功能是子线程查找文件后回调函数
 *      flag 说明:
 *      0   无任何表示
 *      1   表示机种番号信息获取
 *      2   表示机种类型信息获取
 *      3   表示量产管理表信息获取
 *      4   表示Ini文件信息获取
 *      5   表示P票信息获取
 *      6   表示SW确认文件获取
 *      7   表示CarInfoMot文件获取
 *      8   表示CarMapMot文件获取
 *      9   表示OSDMot文件获取
 *      10   表示 joinMot 文件获取
 *      11   表示 appMot   文件获取
 * @brief UIMethod::EndFindFileThreadSignal
 * @param st
 * @param flag
 */
void UIMethod::EndFindFileThreadSlot(QStringList st, unsigned int flag)
{
    QLogHelper::instance()->LogInfo("UIMethod->EndFindFileThreadSignal() 函数执行!");
    switch (flag) {
    case 3:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getRelyFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),RelyFileError,*(comBean->getRelyFilePath()),true);
        break;
    case 4:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getRelyID(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),IniFileError,*(comBean->getRelyFilePath()),true);
        break;
    case 5:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getPFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),PFileError,*(comBean->getRelyFilePath()),true);
        break;
    case 6:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getSWFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),SWFileError,*(comBean->getRelyFilePath()),true);
        break;
    case 7:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getCarInfoFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),CarInfoFileError,*(comBean->getRelyFilePath()),true);
        break;
    case 8:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getCarMapFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),CarMapFileError,*(comBean->getRelyFilePath()),true);
        break;
    case 9:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getCarOSDFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),CarOSDFileError,*(comBean->getRelyFilePath()),true);
        break;
    case 10:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getPFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),JoinFileError,*(comBean->getRelyFilePath()),true);
        break;
    case 11:
        comBean->getComMethod()->AnalyzeFilePath(st,comBean->getPFilePath(),flag);
        comBean->getComMethod()->ErrorCodeDeal(comBean->getErrCode(),comBean->getXmlOperate()->getErrCodeType(),APPFileError,*(comBean->getRelyFilePath()),true);
        //comBean->setStatusflag(0);
        //return;
        break;
    case 12:

        break;
    case 13:

        break;
    }
    emit ShowIDmessageSignal(flag);
    emit ActiveThreadSignal(*(comBean->getSVNDirPath()),flag+1);
}

