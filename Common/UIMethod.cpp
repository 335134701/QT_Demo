#include "UIMethod.h"

UIMethod::UIMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("UIMethod() 构造函数执行!");
}

void UIMethod::setComBean(CommonBean *value)
{
    comBean = value;
}

/**
 * @def 处理错误码的添加或者移除
 *      flag  true  表示添加错误码
 *      flag  false 表示移除错误码
 * @brief UIMethod::ErrorCodeDeal
 * @param ID
 * @param flag
 */
void UIMethod::ErrorCodeDeal(const QString objectName, bool flag)
{
    if(comBean==NULL){return;}
    if(flag&&comBean->errCode.value(objectName).ID.isEmpty()){
        //如果错误码不存在，则添加错误码
        comBean->errCode.insert(objectName,comBean->getXmlOperate()->getErrCodeType().value(objectName));
    }else if(!comBean->errCode.value(objectName).ID.isEmpty()){
        //如果存在错误码则移除错误码
            comBean->errCode.remove(objectName);
    }
}

/**
 * @def 判断机种名称是否符合要求
 * @brief UIMethod::JudgeIDSlot
 * @param Edit
 */
void UIMethod::JudgeIDSlot(QLineEdit *Edit,QString *objectID)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDSlot() 函数执行!");
    if(comBean==NULL){
        QLogHelper::instance()->LogInfo("UIMethod->JudgeIDSlot() comBean = NULL");
        return;
    }
    //设置正则表达式对象
    QRegExp rx(comBean->getRExpression().value(Edit->objectName()));
    if(rx.indexIn(Edit->text())==0){
        QLogHelper::instance()->LogInfo("UIMethod->JudgeIDSlot() "+Edit->text()+" 符合正则表达式要求!");
        *objectID=Edit->text();
        Edit->setStyleSheet(QString(nomFontColor)); //字体相关设置;
        //如果存在错误码,则移除错误码
        ErrorCodeDeal(Edit->objectName(),false);
    }else{
        QLogHelper::instance()->LogInfo("UIMethod->JudgeIDSlot() "+Edit->text()+" 不符合正则表达式要求!");
        *objectID="";
        Edit->setStyleSheet(QString(errFontColor)); //字体相关设置;
        //如果错误码不存在，则添加错误码
         ErrorCodeDeal(Edit->objectName(),true);
    }
}
/**
 * @def 判断机种类型
 * @brief AutomationTool::JudgeIDTypeSlot
 */
void UIMethod::JudgeIDTypeSlot(QLineEdit *Edit)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDTypeSlot() 函数执行!");
    if(comBean==NULL){
        QLogHelper::instance()->LogInfo("UIMethod->JudgeIDTypeSlot() comBean = NULL");
        return;
    }
    QString ret=JudgeIDType(Edit->text());
    if(Edit->objectName()=="IDEdit"){
        QLogHelper::instance()->LogInfo("UIMethod->JudgeIDTypeSlot() "+Edit->objectName()+" 类型判断!");
        comBean->setIDType(ret);
        if((!comBean->getRelyIDType().isEmpty()&&ret!=comBean->getRelyIDType())||(comBean->ID==comBean->RelyID)){
            //错误处理，添加错误码
            ErrorCodeDeal(IDRelyID,true);
            return;
        }
    }
    if(Edit->objectName()=="RelyIDEdit"){
        QLogHelper::instance()->LogInfo("UIMethod->JudgeIDTypeSlot() "+Edit->objectName()+" 类型判断!");
        if((!comBean->getIDType().isEmpty()&&ret!=comBean->getIDType())||(comBean->ID==comBean->RelyID)){
            Edit->setStyleSheet(QString(errFontColor)); //字体相关设置;
            //错误处理，添加错误码
            ErrorCodeDeal(IDRelyID,true);
            return;
        }
        comBean->setRelyIDType(ret);
    }
    Edit->setStyleSheet(QString(nomFontColor));
    //正确处理，如果存在错误码，则移除
    ErrorCodeDeal(IDRelyID,false);
}
/**
 * @def 判断机种类型
 * @brief UIMethod::JudgeIDType
 * @param ID
 * @return 返回机种类型对象
 */
QString UIMethod::JudgeIDType(const QString ID)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDType() 函数执行!");
    QString ret;
    if(!ID.contains("EN")){return NULL;}
    switch (ID.mid(2,2).toInt()) {
    case 33 :
    case 40 :
    case 42 :
        ret="EntryAVM";
        break;
    case 34:
        ret="EntryAVM2";
        break;
    case 35:
        ret="EntryIPA";
        break;
    case 36:
        ret="FAP";
        break;
    case 37:
        ret="NextPH3";
        break;
    default:
        ret="";
        break;
    }
    return ret;
}
/**
 * @def 按钮选择目录并赋值
 * @brief UIMethod::SelectDir
 * @param label
 * @param destDirPath
 * @param dirPath
 */
void UIMethod::SelectDirSlot(QLabel *label,QString *objectID)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectDirSlot() 函数执行!");
    if(comBean==NULL){
        QLogHelper::instance()->LogInfo("UIMethod->SelectDirSlot() comBean = NULL");
        return;
    }
    label->setText("");
    QString dirName =  QFileDialog::getExistingDirectory(label, tr("Open Directory"),comBean->desktopDirPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirName.isEmpty()) {
        QLogHelper::instance()->LogInfo("UIMethod->SelectDirSlot() "+dirName+" 未选择目录!");
        *objectID="";
        label->setStyleSheet(QString(errFontColor));
        //如果错误码不存在，则添加错误码
        ErrorCodeDeal(label->objectName(),true);
        QMessageBox::warning(label, "Warn", tr("No directory selected!"));
        return;
    }
    QLogHelper::instance()->LogInfo("UIMethod->SelectDirSlot() 选择目录: "+dirName);
    label->setStyleSheet(QString(nomFontColor));
    //正确处理，如果存在错误码，则移除
    ErrorCodeDeal(label->objectName(),false);
    label->setText(dirName);
    *objectID=dirName;
}
/**
 * @def 查找Example目录内文档
 * @brief UIMethod::SelectExample
 * @param dirPath
 * @param condition
 */
void UIMethod::SelectExampleSlot(const QString dirPath, QString condition)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectExample() 函数执行!");
    if(comBean==NULL||comBean->getIDType().isEmpty()){
        QLogHelper::instance()->LogInfo("UIMethod->SelectExampleSlot() comBean==NULL||comBean->getIDType().isEmpty() 满足条件!" );
        return;
    }
    if(condition.compare("EntryAVM2")==0){ condition="EntryAVM";}
    if(condition.compare("NextPH3")==0){ condition="NextPhase3";}
    comBean->setRelyFilePath("");
    if(QFile::exists(dirPath)){
        QLogHelper::instance()->LogInfo("UIMethod->SelectExampleSlot() "+dirPath+" 目录存在!" );
        //Example目录存在,存在错误码则移除错误码
        ErrorCodeDeal(ExampleDirError,false);
        QDir searchDir(dirPath);
        //列出searchDir(path)目录文件下所有文件和目录信息，存储到file_list容器
        QFileInfoList file_list = searchDir.entryInfoList();
        //进行子文件夹folder_list递归遍历，将内容存入file_list容器
        foreach(QFileInfo fileinfo, file_list)
        {
            if(fileinfo.fileName().contains(condition)&&fileinfo.fileName().contains("AKM対応用")){
                QLogHelper::instance()->LogInfo("UIMethod->SelectExampleSlot() "+fileinfo.fileName()+" 文件存在!" );
                comBean->setRelyFilePath(fileinfo.absoluteFilePath());
            }
            if(fileinfo.fileName().contains("LOGZONE_")&&fileinfo.completeSuffix()=="ini"){
                QLogHelper::instance()->LogInfo("UIMethod->SelectExampleSlot() "+fileinfo.fileName()+" 文件存在!" );
                comBean->setIniFilePath(fileinfo.absoluteFilePath());
            }
        }
        if(comBean->getRelyFilePath().isEmpty()){
            ErrorCodeDeal(RelyFileError,true);
        }else{
            ErrorCodeDeal(RelyFileError,false);
        }
        if(comBean->getIniFilePath().isEmpty()){
            ErrorCodeDeal(IniFileError,true);
        }else{
            ErrorCodeDeal(IniFileError,false);
        }
    }else{
        //Example目录不存在,异常处理,添加错误码
        ErrorCodeDeal(ExampleDirError,true);
    }
}
/**
 * @def 获取成果物下面相关文件路径
 * @brief UIMethod::SelectResultFileSlot
 * @param dirPath
 */
void UIMethod::SelectResultFileSlot(const QString dirPath)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectResultFileSlot() 函数执行!");
    if(comBean==NULL||comBean->getIDType().isEmpty()){
        QLogHelper::instance()->LogInfo("UIMethod->SelectResultFileSlot() comBean==NULL !" );
        return;
    }
}


