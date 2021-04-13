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
    if(flag){
        //如果错误码不存在，则添加错误码
        if(comBean->errCode.value(objectName).ID.isEmpty()){
            comBean->errCode.insert(objectName,comBean->getXmlOperate()->getErrCodeType().value(objectName));
        }
    }else{
        //如果存在错误码则移除错误码
        if(!comBean->errCode.value(objectName).ID.isEmpty()){
            comBean->errCode.remove(objectName);
        }
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
    if(comBean==NULL){return;}
    //设置正则表达式对象
    QRegExp rx(comBean->getRExpression().value(Edit->objectName()));
    if(rx.indexIn(Edit->text())==0){
        *objectID=Edit->text();
        Edit->setStyleSheet(QString(nomFontColor)); //字体相关设置;
        //如果存在错误码,则移除错误码
        ErrorCodeDeal(Edit->objectName(),false);
    }else{
        *objectID="";
        Edit->setStyleSheet(QString(errFontColor)); //字体相关设置;
        //如果错误码不存在，则添加错误码
         ErrorCodeDeal(Edit->objectName(),true);
    }
}
/**
 * @def 选择目录并赋值
 * @brief UIMethod::SelectDir
 * @param label
 * @param destDirPath
 * @param dirPath
 */
void UIMethod::SelectDirSlot(QLabel *label,QString *objectID)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectDirSlot() 函数执行!");
    if(comBean==NULL){return;}
    label->setText("");
    QString dirName =  QFileDialog::getExistingDirectory(label, tr("Open Directory"),comBean->desktopPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
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
/**
 * @def 查找Example文档
 * @brief UIMethod::SelectExample
 * @param dirPath
 * @param condition
 */
void UIMethod::SelectExampleSlot(const QString dirPath,const QString condition)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectExample() 函数执行!");
    if(comBean==NULL||comBean->getIDType().isEmpty()){return;}
    if(QFile::exists(dirPath)){
        //Example目录存在,存在错误码则移除错误码
        ErrorCodeDeal(ExampleDirError,false);
        QDir searchDir= QDir(dirPath);
        QFileInfoList list = searchDir.entryInfoList();
        QFileInfoList::const_iterator cit = list.begin();
        while(cit != list.end())
        {
            QLogHelper::instance()->LogDebug((*cit++).fileName());
        }
    }else{
        //Example目录不存在,异常处理,添加错误码
        ErrorCodeDeal(ExampleDirError,true);
    }
}
/**
 * @def 判断机种类型槽函数
 * @brief AutomationTool::JudgeIDTypeSlot
 */
void UIMethod::JudgeIDTypeSlot(QLineEdit *Edit)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDTypeSlot() 函数执行!");
    if(comBean==NULL){return;}
    QString ret=JudgeIDType(Edit->text());
    if(comBean->getIDType().isEmpty()){
        comBean->setIDType(ret);
    }else if(comBean->getIDType()!=ret){
        Edit->setStyleSheet(QString(errFontColor));
        ErrorCodeDeal(IDRelyID,true);
        //错误处理，添加错误码
        ErrorCodeDeal(IDRelyID,true);
        return ;
    }
    Edit->setStyleSheet(QString(nomFontColor));
    //正确处理，如果存在错误码，则移除
    ErrorCodeDeal(IDRelyID,false);
    return;
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

