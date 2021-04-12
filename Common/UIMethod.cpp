#include "UIMethod.h"

UIMethod::UIMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("UIMethod->UIMethod() 构造函数执行!");
}

void UIMethod::setComBean(CommonBean *value)
{
    comBean = value;
}

/**
 * @def 判断机种名称是否符合要求
 * @brief UIMethod::JudgeIDSlot
 * @param Edit
 */
void UIMethod::JudgeIDSlot(QLineEdit *Edit)
{
    QLogHelper::instance()->LogInfo("UIMethod->JudgeIDSlot() 函数执行!");
    if(comBean==NULL){return;}
    //设置正则表达式对象
    QRegExp rx(comBean->getRExpression().value(Edit->objectName()));
    if(rx.indexIn(Edit->text())==0){
        Edit->setStyleSheet(QString(nomFontColor)); //字体相关设置;
        //如果存在错误码则移除错误码
        if(!comBean->getErrCode().value(Edit->objectName()).ID.isEmpty()){
            comBean->getErrCode().remove(Edit->objectName());
        }
    }else{
        Edit->setStyleSheet(QString(errFontColor)); //字体相关设置;
        //如果错误码不存在，则添加错误码
        if(comBean->getErrCode().value(Edit->objectName()).ID.isEmpty()){
            comBean->getErrCode().insert(Edit->objectName(),comBean->getXmlOperate()->getErrCodeType().value(Edit->objectName()));
        }
    }
}
/**
 * @def 选择目录并赋值
 * @brief UIMethod::SelectDir
 * @param label
 * @param destDirPath
 * @param dirPath
 */
void UIMethod::SelectDirSlot(QLabel *label)
{
    QLogHelper::instance()->LogInfo("UIMethod->SelectDirSlot() 函数执行!");
    if(comBean==NULL){return;}
    label->setText("");
    QString dirName =  QFileDialog::getExistingDirectory(label, tr("Open Directory"),comBean->desktopPath,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dirName.isEmpty()) {
        label->setStyleSheet(QString(errFontColor));
        //错误处理，添加错误码
        if(comBean->getErrCode().value(label->objectName()).ID.isEmpty()){
            comBean->getErrCode().insert(label->objectName(),comBean->getXmlOperate()->getErrCodeType().value(label->objectName()));
        }
        QMessageBox::warning(label, "Warn", tr("No directory selected!"));
        return;
    }
    label->setStyleSheet(QString(nomFontColor));
    //正确处理，如果存在错误码，则移除
    if(!comBean->getErrCode().value(label->objectName()).ID.isEmpty()){
        comBean->getErrCode().remove(label->objectName());
    }
    label->setText(dirName);
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
    //exampleDir=comBean->applicationPath+"/Example";
    if(comBean==NULL||comBean->getIDType().isEmpty()){return;}
    if(QFile::exists(dirPath)){
        /*
        QDir searchDir= QDir(dirPath);
        QFileInfoList list = searchDir.entryInfoList();
        QFileInfoList::const_iterator cit = list.begin();
        while(cit != list.end())
        {
            QLogHelper::instance()->LogDebug((*cit++).fileName());
        }
        */
    }else{
        //Example目录不存在，异常处理
        if(comBean->getErrCode().value("ExampleDirError").ID.isEmpty()){
            comBean->getErrCode().insert("ExampleDirError",comBean->getXmlOperate()->getErrCodeType().value("ExampleDirError"));
        }
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
    if(!Edit->objectName().contains("Rely")){
        comBean->setIDType(ret);
    }else{
        if(comBean->getIDType()!=ret){
            Edit->setStyleSheet(QString(errFontColor));
            //错误处理，添加错误码
            if(comBean->getErrCode().value("RelyIDID").ID.isEmpty()){
                comBean->getErrCode().insert("RelyIDID",comBean->getXmlOperate()->getErrCodeType().value("RelyIDID"));
            }
        }else{
            Edit->setStyleSheet(QString(nomFontColor));
            //正确处理，如果存在错误码，则移除
            if(!comBean->getErrCode().value("RelyIDID").ID.isEmpty()){
                comBean->getErrCode().remove("RelyIDID");
            }
        }
    }
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

