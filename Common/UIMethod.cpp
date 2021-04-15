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
    QLogHelper::instance()->LogInfo("UIMethod->ErrorCodeDeal() 函数执行!");
    if(comBean==NULL){return;}
    if(flag&&comBean->getErrCode()->value(objectName).ID.isEmpty()){
        //如果错误码不存在，则添加错误码
        comBean->getErrCode()->insert(objectName,comBean->getXmlOperate()->getErrCodeType().value(objectName));
    }else if(!comBean->getErrCode()->value(objectName).ID.isEmpty()){
        //如果存在错误码则移除错误码
        comBean->getErrCode()->remove(objectName);
    }
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
    //判断comBean 对象是否为空，如果不为空则继续执行
    if(comBean==NULL){return;}
    //设置正则表达式对象
    QRegExp rx(comBean->getRExpression().value(Edit->objectName()));
    if(rx.indexIn(Edit->text())!=0){
        Edit->setStyleSheet(QString(errFontColor)); //字体相关设置;
        //如果错误码不存在，则添加错误码
        ErrorCodeDeal(Edit->objectName(),true);
        return;
    }
    //对指定对象赋值
    *objectID=Edit->text();
    Edit->setStyleSheet(QString(nomFontColor)); //字体相关设置;
    //如果存在错误码,则移除错误码
    ErrorCodeDeal(Edit->objectName(),false);
}
/**
 * @def 判断机种类型
 *      以作成机种(ID)为主导地位
 *      以依赖机种(RelyID)为辅导地位
 *      对比依赖机种与作成机种的类型是否一致
 *      如果不一致，则依赖机种显示红色字体
 * @brief AutomationTool::JudgeIDTypeSlot
 */
void UIMethod::JudgeIDTypeSlot(QLineEdit *Edit)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDTypeSlot() 函数执行!");
    //判断comBean 对象是否为空，如果不为空则继续执行
    if(comBean==NULL){return;}
    QString ret=JudgeIDType(Edit->text());
    if(Edit->objectName()=="IDEdit"){
        comBean->setIDType(ret);
        if((!comBean->getRelyIDType().isEmpty()&&ret!=comBean->getRelyIDType())||(comBean->getID()==comBean->getRelyID())){
            //错误处理，添加错误码
            ErrorCodeDeal(IDRelyID,true);
            return;
        }
    }else{
        if((!comBean->getIDType().isEmpty()&&ret!=comBean->getIDType())||(comBean->getID()==comBean->getRelyID())){
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
 *      根据ID字符串，第三四位字符判断
 * @brief UIMethod::JudgeIDType
 * @param ID
 * @return 返回机种类型对象
 */
QString UIMethod::JudgeIDType(const QString ID)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDType() 函数执行!");
    QString ret;
    if(!ID.contains("EN")){return NULL;}
    //字符转数字
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
 * @def 查找Example目录内文档
 * @brief UIMethod::SelectExample
 * @param dirPath
 * @param condition
 */
void UIMethod::SelectExampleSlot(const QString dirPath, bool flag)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectExample() 函数执行!");
    if(comBean==NULL||comBean->getIDType().isEmpty()){return;}
    QString condition=comBean->getIDType();
    comBean->setRelyFilePath("");
    if(!QFile::exists(dirPath)){
        //Example目录不存在,异常处理,添加错误码
        ErrorCodeDeal(ExampleDirError,true);
        return;
    }
    //Example目录存在,存在错误码则移除错误码
    ErrorCodeDeal(ExampleDirError,false);
    //.ini文件比较特殊，在check过程中获取的文件路径和生成过程中路径不同
    if(!flag){
        if(condition.compare("EntryAVM2")==0){ condition="EntryAVM";}
        if(condition.compare("NextPH3")==0){ condition="NextPhase3";}
        //文件查找并获取返回值
        //comBean->setRelyFilePath(comBean->getComMethod()->OutputFilePath(dirPath,condition,"AKM対応用"));
        if(comBean->getRelyFilePath().isEmpty()){
            ErrorCodeDeal(RelyFileError,true);
        }else{
            ErrorCodeDeal(RelyFileError,false);
        }
    }else{
        //文件查找并获取返回值
        //comBean->setIniFilePath(comBean->getComMethod()->OutputFilePath(dirPath,"LOGZONE_","ini"));
        if(comBean->getIniFilePath().isEmpty()){
            ErrorCodeDeal(IniFileError,true);
        }else{
            ErrorCodeDeal(IniFileError,false);
        }
    }
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
    //判断comBean 对象是否为空，如果不为空则继续执行
    if(comBean==NULL){return;}
    label->setText("");
    QString dirName =  QFileDialog::getExistingDirectory(label, tr("Open Directory"),comBean->desktopDirPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirName.isEmpty()) {
        *objectID="";
        label->setStyleSheet(QString(errFontColor));
        //如果错误码不存在，则添加错误码
        ErrorCodeDeal(label->objectName(),true);
        QMessageBox::warning(label, "Warn", tr("No directory selected!"));
        return;
    }
    label->setStyleSheet(QString(nomFontColor));
    //正确处理，如果存在错误码，则移除
    ErrorCodeDeal(label->objectName(),false);
    label->setText(dirName);
    *objectID=dirName;
}
