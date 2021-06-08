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
    TmpProcessStatus=0;
    fileThread=new QThread();
    excelThread=new QThread();
    siCommonMethod=new SICommonMethod();
    excelOperateThread=new SIExcelOperateThread();
    fileOperateThread=new SIFileOperateThread();
    fileOperateThread->moveToThread(fileThread);
    excelOperateThread->moveToThread(excelThread);
    siConfig=new SIConfig();
}

/**
 * @def 处理连接信号槽函数
 * @brief SIFormMethod::ConnectSlot
 */
void SIFormMethod::ConnectSlot()
{
    QLogHelper::instance()->LogInfo("SIFormMethod->ConnectSlot() 函数执行!");
    //不同线程之间传递自定义类型参数需要注册
    qRegisterMetaType<QList<SI_SOFTNUMBERTable>> ("QList<SI_SOFTNUMBERTable>");
    qRegisterMetaType<QList<SI_ERRORTable>> ("QList<SI_ERRORTable>");
    qRegisterMetaType<QList<SI_DEFINEMESSAGE>> ("QList<SI_DEFINEMESSAGE>");
    //连接Log信息输出处理信号槽函数
    connect(this,&SIFormMethod::ShowMessageProcessSignal,this,&SIFormMethod::ShowMessageProcessSlot);
    //连接外部程序处理信号槽函数
    connect(this,&SIFormMethod::RunOrderSignal,this,&SIFormMethod::RunOrderSlot);
    connect(this->fileOperateThread,&SIFileOperateThread::EndRunOrderSignal,this,&SIFormMethod::EndRunOrderSlot);
    //SVN更新任务信号槽函数处理
    connect(this,&SIFormMethod::UpdateSVNSignal,this->fileOperateThread,&SIFileOperateThread::UpdateSVNSlot);
    //连接文件解压信号槽函数
    connect(this,&SIFormMethod::UNZipCodeFileSignal,this->fileOperateThread,&SIFileOperateThread::UNZipCodeFileSlot);
    //文件检索及文件检索完成后回调处理函数
    connect(this,&SIFormMethod::SearchFileSignal,this,&SIFormMethod::SearchFileSlot);
    connect(this,&SIFormMethod::FileSearchSignal,this->fileOperateThread,&SIFileOperateThread::FileSearchSlot);
    connect(this->fileOperateThread,&SIFileOperateThread::EndFileSearcSignal,this,&SIFormMethod::EndFileSearcSlot);
    //Excel文件解析信号槽函数连接
    connect(this,&SIFormMethod::ReadExcelThreadSignal,this->excelOperateThread,&SIExcelOperateThread::ReadExcelThreadSlot);
    //量产管理表解析完成后信号槽函数连接
    connect(this->excelOperateThread,&SIExcelOperateThread::EndReadSoftExcelSignal,this,&SIFormMethod::EndReadSoftExcelSlot);
    //SW一览表解析完成后回调函数连接
    connect(this->excelOperateThread,&SIExcelOperateThread::EndReadDefineFileExcelSignal,this,&SIFormMethod::EndReadDefineFileExcelSlot);
    //文件检查信号槽函数
    connect(this,&SIFormMethod::CheckBAFileSignal,this->fileOperateThread,&SIFileOperateThread::CheckBAFileSlot);
    connect(this->fileOperateThread,&SIFileOperateThread::EndCheckBAFileSignal,this,&SIFormMethod::EndCheckBAFileSlot);
    connect(this,&SIFormMethod::CheckCLFileSignal,this->fileOperateThread,&SIFileOperateThread::CheckCLFileSlot);
    connect(this->fileOperateThread,&SIFileOperateThread::EndCheckCLFileSignal,this,&SIFormMethod::EndCheckCLFileSlot);
    //文件预处理完成后继续执行预处理操作
    connect(this,&SIFormMethod::PretreatmentSignal,this,&SIFormMethod::PretreatmentSlot);
    //可执行文件生成后后处理操作
    connect(this,&SIFormMethod::FileCompressionSignal,this,&SIFormMethod::FileCompressionSlot);
    //文件复制信号槽函数
    connect(this,&SIFormMethod::CopyCodeFileSignal,this->fileOperateThread,&SIFileOperateThread::CopyCodeFileSlot);
    connect(this->fileOperateThread,&SIFileOperateThread::EndCopyCodeFileSignal,this,&SIFormMethod::EndCopyCodeFileSlot);
    //项目源码后续压缩信号槽函数
    connect(this,&SIFormMethod::ZipCodeFileSignal,this->fileOperateThread,&SIFileOperateThread::ZipCodeFileSlot);
    //推算依赖ID连接信号槽函数
    connect(this,&SIFormMethod::InferRelyIDSignal,this,&SIFormMethod::InferRelyIDSlot);
    connect(this,&SIFormMethod::InferRelyIDProcessSignal,this->excelOperateThread,&SIExcelOperateThread::InferRelyIDProcessSlot);
    connect(this->excelOperateThread,&SIExcelOperateThread::EndInferRelyIDProcessSignal,this,&SIFormMethod::EndInferRelyIDProcessSlot);
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
    QString ret=siCommonMethod->JudgeIDType(Edit->text());
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
    unsigned int level=LOG_INFO;
    QStringList message;
    //对数据包进行处理
    switch (flag) {
    case SIIDflag:
        level=LOG_INFO;
        message.append("机种番号: "+*siFormBean->getID());
        message.append("机种类型: "+*siFormBean->getIDType());
        break;
    case SIRelyIDflag:
        level=LOG_INFO;
        message.append("依赖机种番号: "+*siFormBean->getID());
        message.append("依赖机种类型: "+*siFormBean->getIDType());
        break;
    case SISVNUpdateflag:
        if(siFormBean->getSIStatus()!=0){
            level=LOG_INFO;
            message.append("SVN开始更新，更新路径: "+*siFormBean->getSVNDirPath());
        }else{
            if(siFormBean->getSVNUpdateStatus()){
                level=LOG_INFO;
                message.append("SVN更新成功，更新路径: "+*siFormBean->getSVNDirPath());
            }else{
                level=LOG_WARN;
                message.append("SVN更新失败！");
            }
        }
        break;
    case SIUnzipFileflag:
        if(siFormBean->getSIStatus()==SI_PRETREAMENT){
            level=LOG_INFO;
            message.append("项目源码开始解压，文件路径: "+*siFormBean->getCodeFilePath());
        }else {
            if(siFormBean->getUnzipflag()){
                level=LOG_INFO;
                message.append("项目源码解压完成，目录路径: "+siFormBean->getCodeFilePath()->left(siFormBean->getCodeFilePath()->lastIndexOf("/"))+"/"+ProjectName);
            }else{
                level=LOG_ERROR;
                message.append("项目源码解压失败!");
            }
        }
        break;
    case SIRelyFileflag:
        if(!siFormBean->getRelyFilePath()->isEmpty()){
            level=LOG_INFO;
            message.append("量产管理表路径: "+*siFormBean->getRelyFilePath());
        }else{
            level=LOG_ERROR;
            message.append("量产管理表路径获取失败!");
        }
        break;
    case SIPFileflag:
        if(!siFormBean->getPFilePath()->isEmpty()){
            level=LOG_INFO;
            message.append("P票模板路径: "+*siFormBean->getPFilePath());
        }else{
            level=LOG_ERROR;
            message.append("P票模板路径获取失败!");
        }
        break;
    case SISWFileflag:
        if(!siFormBean->getSWFilePath()->isEmpty()){
            level=LOG_INFO;
            message.append("SW確認文件模板路径: "+*siFormBean->getSWFilePath());
        }else{
            level=LOG_ERROR;
            message.append("SW確認文件模板路径获取失败!");
        }
        break;
    case SICarInfoFileflag:
        if(!siFormBean->getCarInfoFilePath()->isEmpty()){
            level=LOG_INFO;
            message.append("CarInfo文件路径: "+*siFormBean->getCarInfoFilePath());
        }else{
            level=LOG_ERROR;
            message.append("CarInfo文件模板路径获取失败!");
        }
        break;
    case SIBuildFileflag:
        if(!siFormBean->getBuildFilePath()->isEmpty()){
            level=LOG_INFO;
            message.append("Build文件模板路径: "+*siFormBean->getBuildFilePath());
        }else{
            level=LOG_ERROR;
            message.append("Build文件模板路径获取失败!");
        }
        break;
    case SICodeFileflag:
        if(!siFormBean->getCodeFilePath()->isEmpty()){
            level=LOG_INFO;
            message.append("项目源码路径: "+*siFormBean->getCodeFilePath());
        }else{
            level=LOG_ERROR;
            message.append("项目源码路径获取失败!");
        }
        break;
    case SICopyCodeflag:
        if(siFormBean->getCopyCodeflag()){
            level=LOG_INFO;
            message.append("项目源码复制成功,文件新路径: "+*siFormBean->getCodeFilePath());
        }else{
            level=LOG_ERROR;
            message.append("项目源码复制失败!");
        }
        break;
    case SISHDefineFileflag:
        if(!siFormBean->getSHDefineFilePath()->isEmpty()){
            level=LOG_INFO;
            message.append("SW一覧表路径: "+*siFormBean->getSHDefineFilePath());
        }else{
            level=LOG_ERROR;
            message.append("SW一覧表路径获取失败!");
        }
        break;
    case SIRelyMessageflag:
        if(siFormBean->getSoftList()->size()>0){
            level=LOG_INFO;
            foreach (SI_SOFTNUMBERTable table, *(siFormBean->getSoftList())) {
                message.append("クラリオン機種番号: "+table.ModelNumber);
                message.append("車種仕向け: "+table.CarModels);
                message.append("CAN世代: "+table.CANGen);
                message.append("生産段階: "+table.Productionstage);
                message.append("Application PartsNo: "+table.ApplicationPartNo);
                message.append("Application Ver: "+table.ApplicationVer);
                message.append("Car Info PartsNo: "+table.CarInfoPartNo);
                message.append("Car Info Ver: "+table.CarInfoVer);
                message.append("\\n");
            }
        }else{
            level=LOG_WARN;
            message.append("文件解析失败 : "+siFormBean->getRelyFilePath()->mid(siFormBean->getRelyFilePath()->lastIndexOf("/")+1));
        }
        break;
    case SISHDefineflag:
        if(siFormBean->getDefineList()->size()>0){
            level=LOG_INFO;
            foreach (SI_DEFINEMESSAGE table, *(siFormBean->getDefineList())) {
                message.append("番号: "+*siFormBean->getID());
                message.append("机种类型: "+*siFormBean->getIDType());
                message.append("HEWプロジェクト: "+table.stageName);
                message.append("マクロ定義: "+table.defineName);
                if(table.isUse){
                    message.append("使用状态: 使用");
                }else{
                    message.append("使用状态: 未使用");
                }
                message.append("\\n");
            }
        }else{
            level=LOG_WARN;
            message.append("文件解析失败 : "+siFormBean->getSHDefineFilePath()->mid(siFormBean->getSHDefineFilePath()->lastIndexOf("/")+1));
        }
        break;
    case SIERRorMessageflag:
        if(siFormBean->getErrList()->size()>0){
            level=LOG_ERROR;
            foreach (SI_ERRORTable err, *(siFormBean->getErrList())) {
                message.append("文件(目录)名称: "+err.fileName);
                if(!err.sheetName.isEmpty()){
                    message.append("sheet名称: "+err.fileName);
                }
                if(err.row!=0||err.col!=0)
                {
                    message.append("第: "+QString::number(err.row)+ " 行, 第: "+QString::number(err.col)+" 列");
                }
                message.append("错误描述: "+err.errMessage);
            }
        }
        break;
    case SIBADirflag:
        if(siFormBean->getBAflag()){
            level=LOG_INFO;
            message.append("Before&After 文件夹校验成功!");
        }else{
            level=LOG_ERROR;
            message.append("Before&After 文件夹校验失败!");
        }
        break;
    case SICLVERCheckflag:
        if(siFormBean->getNEWCLflag()){
            level=LOG_WARN;
            message.append("ID : "+*siFormBean->getID()+" 需要作成新规!");
        }
        break;
    case SIZIPFileflag:
        if(siFormBean->getSIStatus()==SI_FILEZIP){
            level=LOG_INFO;
            message.append("项目源码开始压缩，文件夹路径: "+*siFormBean->getOutputDirPath()+"/"+*siFormBean->getID()+"/"+ProjectName);
        }else {
            if(siFormBean->getZIPflag()){
                level=LOG_INFO;
                message.append("项目源码压缩成功!");
            }else{
                level=LOG_ERROR;
                message.append("项目源码解压失败!");
            }
        }
        break;
    case SIRelyIDDealflag:
        level=LOG_INFO;
        message.append(*siFormBean->getID()+ " 宏定义推算结束!");
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
 * @def 执行调用外部程序处理函数
 * @brief SIFormMethod::RunOrderSlot
 * @param flag
 */
void SIFormMethod::RunOrderSlot(const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->RunOrderSlot() 函数执行!");
    QFile *file;
    QString exeFilePath;
    emit ShowMessageProcessSignal(flag,LOG_LOG);
    switch (flag) {
    case SISVNUpdateflag:
        //SVN状态更新
        exeFilePath=siFormBean->getCommonMethod()->GetSVNInstallPath();
        if(file->exists((*siFormBean->getSVNDirPath())+"/.svn")&&file->exists(exeFilePath))
        {
            //如果子线程未启动，则开启子线程
            if(!fileThread->isRunning()){fileThread->start();}
            emit UpdateSVNSignal(exeFilePath,*siFormBean->getSVNDirPath(),flag);
        }
        break;
    case SIUnzipFileflag:
        //解压状态更新
        siFormBean->setSIStatus(SI_FILEUNZIP);
        exeFilePath=siFormBean->getCommonMethod()->Get7zInstallPath();
        if(file->exists(*siFormBean->getCodeFilePath())&&file->exists(exeFilePath))
        {
            //如果子线程未启动，则开启子线程
            if(!fileThread->isRunning()){fileThread->start();}
            emit UNZipCodeFileSignal(exeFilePath,*siFormBean->getCodeFilePath(),*siFormBean->getOutputDirPath()+"/"+*siFormBean->getID(),flag);
        }
    case SIZIPFileflag:
        QString txt=siFormBean->getSoftList()->value(siFormBean->getSoftList()->size()-1).CarModels;
        if(txt.contains("-")){txt.replace("-","_");}
        //压缩状态更新
        exeFilePath=siFormBean->getCommonMethod()->Get7zInstallPath();
        if(QDir(*siFormBean->getOutputDirPath()+"/"+*siFormBean->getID()+"/"+ProjectName).exists()&&file->exists(exeFilePath))
        {
            //如果子线程未启动，则开启子线程
            if(!fileThread->isRunning()){fileThread->start();}
            emit ZipCodeFileSignal(exeFilePath,*siFormBean->getOutputDirPath()+"/"+*siFormBean->getID()+"/"+ProjectName, \
                                   *siFormBean->getIDType(),txt,siFormBean->getSoftList()->value(siFormBean->getSoftList()->size()-1).ApplicationVer,flag);
        }
        break;
    }
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
    QString dirPath=*siFormBean->getSVNDirPath();
    //校验SVN路径是否存在
    if(!QDir(*siFormBean->getSVNDirPath()).exists()){return;}
    //文件检索过滤器
    QStringList filters;
    siFormBean->setSIStatus(SI_FILESEARCH);
    //如果文件搜索子线程未开启，则开启子线程
    if(!fileThread->isRunning()){
        fileThread->start();
    }
    switch (flag) {
    case SIRelyFileflag:
        if(*siFormBean->getIDType()=="EntryAVM2"){
            filters.append("*EntryAVM*ソフトウエア部品番号管理表(量産)_AKM対応用*.xls");
        }else{
            filters.append("*"+*(siFormBean->getIDType())+"*ソフトウエア部品番号管理表(量産)_AKM対応用*.xls");
        }
        break;
    case SIPFileflag:
        filters.append("*AKM火災対応_P票.xls");
        break;
    case SISWFileflag:
        filters.append("コンパイルSW確認結果*.xls");
        break;
    case SICarInfoFileflag:
        filters.append("*"+*(siFormBean->getID())+"*_CarInfo.mot");
        break;
    case SIBuildFileflag:
        filters.append("Buildパラメタ(option)_*.xlsx");
        break;
    case SICodeFileflag:
        filters.append("*.7z");
        filters.append("*.zip");
        break;
    case SISHDefineFileflag:
        filters.append("SH7766*コンパイルSW一覧*.xlsx");
        break;
    }
    emit FileSearchSignal(dirPath,filters,*siFormBean->getID(),*siFormBean->getIDType(),flag,isGoON);
}

/**
 * @def 预处理
 *      包括 创建对应机种文件夹路径
 *          将项目源码复制到创建的路径下
 *          对项目源码解压到指定文件夹下
 *          根据依赖机种番号的设定分成两步处理：依赖机种番号未设定，依赖机种已设定
 *          依赖机种已设定:
 *
 *          依赖机种未设定：
 *              1.经过算法获取机种番号留用关系，
 *                  如果在量产管理表中能获取到留用关系，则不需要再在宏定义表格中做宏定义
 *                  如果在量产管理表中无法获取到留用关系，则需要在对应的宏定义表中添加宏定义
 *              2.检查Before&After文件及其文件夹是否正确
 *              3.合并After文件夹到指定项目路径下
 *              4.根据项目路径下相关文件夹信息判断是否需要做新规
 *              5.将宏定义写入对应的文件中
 *
 * @brief SIFormMethod::PretreatmentSlot
 */
void SIFormMethod::PretreatmentSlot()
{
    QLogHelper::instance()->LogInfo("SIFormMethod->PretreatmentSlot() 函数执行!");
    QString tmpPath;
    bool flag=false;
    QDir *folder=new QDir();
    if(siFormBean->getSIStatus()==SI_READY){
        siFormBean->setSIStatus(SI_PRETREAMENT);
        tmpPath=*siFormBean->getOutputDirPath()+"/"+*siFormBean->getID();
        //如果输出路径不存在，则默认为桌面路径
        if(siFormBean->getOutputDirPath()->isEmpty()){
            (*siFormBean->getOutputDirPath())=siFormBean->getCommonMethod()->desktopDirPath;
            tmpPath=*siFormBean->getOutputDirPath()+"/"+*siFormBean->getID();
        }
        //第一步创建目录
        folder->setPath(tmpPath);
        //如果路径下文件夹存在，则删除文件夹
        if(folder->exists()){
            folder->removeRecursively();
            this->SendConMessageLog(tmpPath+" 文件夹删除成功!",LOG_INFO);
        }
        flag=folder->mkpath(tmpPath);
        if(!flag){RetSIStatus(tmpPath+" 文件夹创建失败!",LOG_ERROR);return;}
        emit this->SendConMessageLog(tmpPath+" 文件夹创建成功!",LOG_INFO);
        //宏定义文件检查，如果没有则需要写入表格中
        /* siFormBean->setSIStatus(SI_FILEDEFINE);
        emit InferRelyIDSignal();
    }else if(siFormBean->getSIStatus()==SI_FILEDEFINE){

        QLogHelper::instance()->LogDebug("---------------------------------------------- 推算完成!");
        siFormBean->setSIStatus(SI_READY);
    }*/
        //项目源码文件复制
        siFormBean->setSIStatus(SI_FILECODECOPY);
        if(!fileThread->isRunning()){fileThread->start();}
        emit CopyCodeFileSignal(*siFormBean->getCodeFilePath(),tmpPath+"/"+siFormBean->getCodeFilePath()->mid(siFormBean->getCodeFilePath()->lastIndexOf("/")+1));
    }else if(siFormBean->getSIStatus()==SI_FILECODECOPY){
        if(!siFormBean->getCopyCodeflag()){siFormBean->setSIStatus(SI_READY);return;}
        siFormBean->setSIStatus(SI_PRETREAMENT);
        //解压项目源码
        emit RunOrderSignal(SIUnzipFileflag);
    }
    else if(siFormBean->getSIStatus()==SI_FILEUNZIP){
        if(!siFormBean->getUnzipflag()){siFormBean->setSIStatus(SI_READY);return;}
        //BeforeAfter文件检查
        siFormBean->setSIStatus(SI_FILCHECKBA);
        if(!fileThread->isRunning()){fileThread->start();}
        emit CheckBAFileSignal(*siFormBean->getSVNDirPath(),*siFormBean->getID(),*siFormBean->getIDType(),*siFormBean->getSoftList(),SIBADirflag);
    }else if(siFormBean->getSIStatus()==SI_FILCHECKBA){

        //合并文件
        if(!siFormBean->getBAflag()){siFormBean->setSIStatus(SI_READY);return;}
        QStringList dirPathList=siCommonMethod->GetBeforeAfterDirPath(fileOperateThread->getSiFileOperateMethod()->AnalyzePath(*siFormBean->getSVNDirPath(),*siFormBean->getID(),*siFormBean->getIDType(),SIBADirflag),*siFormBean->getSoftList());
        if(dirPathList.size()>0&&(siFormBean->getCommonMethod()->CopyDir(dirPathList[1],siFormBean->getCodeFilePath()->left(siFormBean->getCodeFilePath()->lastIndexOf("/"))+"/"+ProjectName))){
            this->SendConMessageLog("After文件夹与源码程序文件夹合并成功!",LOG_INFO);
        }else{
            this->RetSIStatus("After文件夹与源码程序文件夹合并失败，任务无法继续执行!",LOG_ERROR);
        }
        //新规文件检查
        siFormBean->setSIStatus(SI_FILCHECKCL);
        if(!fileThread->isRunning()){fileThread->start();}
        tmpPath=*siFormBean->getOutputDirPath()+"/"+*siFormBean->getID();
        if(!siFormBean->getRelyID()->isEmpty()&&siFormBean->getRelyIDSoftList()->size()>0){
            emit CheckCLFileSignal(tmpPath+"/"+ProjectName,*siFormBean->getRelyID(),*siFormBean->getRelyIDSoftList());
        }else if(siFormBean->getRelyID()->isEmpty()&&siFormBean->getSoftList()->size()>0){
            emit CheckCLFileSignal(tmpPath+"/"+ProjectName,*siFormBean->getID(),*siFormBean->getSoftList());
        }else{
            fileThread->quit();
            fileThread->wait();
            this->RetSIStatus("机种番号信息获取失败,CL_Ver_Parts_ex.h文件检查任务无法继续执行!",LOG_ERROR);
            return;
        }
    }else if(siFormBean->getSIStatus()==SI_FILCHECKCL){

        //宏定义文件写入
    }
}

/**
 * @def 文件压缩处理，前提条件是：量产管理表中信息解析成功，CarInfo文件获取成功
 *      1.将生成的ALL.mot,APP.mot文件与当前时间做对比，超过三十分钟则认为生成失败
 *      2.将Carinfo.mot,ALL.mot文件复制到tools路径下
 *      3.更改SumAddCP.bat,joinmot.bat中对应CarInfo.mot名称
 *      4.执行SumAddCP.bat生成CarInfo.mot.org文件
 *      5.执行joinmot.bat,生成join.mot文件
 *      6.将SH7766_PF\ALL\ALL目录下多余文件夹删除
 *      7.将SH7766_PF\ALL\ALL\机种番号目录下多余文件删除
 *      8.将join.mot,app.mot文件复制到指定路径下
 *      9.压缩项目文件，生成两个不同文件压缩包到指定路径下
 *
 * @brief SIFormMethod::FileCompressionSlot
 */
void SIFormMethod::FileCompressionSlot()
{
    QLogHelper::instance()->LogInfo("SIFormMethod->FileCompressionSlot() 函数执行!");
    QString carModelsName=siFormBean->getSoftList()->value(siFormBean->getSoftList()->size()-1).CarModels;
    siFormBean->setSIStatus(SI_FILCOMPRESSION);
    if(carModelsName.contains("-")){carModelsName.replace("-","_");}
    carModelsName=carModelsName+"_"+siFormBean->getID();
    (*siFormBean->getOutputDirPath())=siFormBean->getCommonMethod()->desktopDirPath;
    QString tmpPath=*siFormBean->getOutputDirPath()+"/"+*siFormBean->getID()+"/"+ProjectName;
    //第一步：对比生成的ALL.mot,APP.mot文件
    //if(!siCommonMethod->CompressionCompareMotFile(tmpPath+"/ALL/ALL/"+carModelsName)){this->RetSIStatus("Mot文件生成失败，任务无法继续执行!",LOG_ERROR);return;}
    //第二步：文件复制
    if(!siFormBean->getCommonMethod()->CopyFile(*siFormBean->getCarInfoFilePath(),tmpPath+"/tools/"+siFormBean->getCarInfoFilePath()->mid(siFormBean->getCarInfoFilePath()->lastIndexOf("/")+1))){
        this->RetSIStatus("CarInfo.mot文件复制失败，任务无法继续执行!",LOG_ERROR);return;
    }
    if(!siFormBean->getCommonMethod()->CopyFile(tmpPath+"/ALL/ALL/"+carModelsName+"/ALL.mot",tmpPath+"/tools/ALL.mot")){
        this->RetSIStatus("CarInfo.mot文件复制失败，任务无法继续执行!",LOG_ERROR);return;
    }
    //第三步:文件更改
    if(!siCommonMethod->CompressionChangeBatFile(tmpPath+"/tools",siFormBean->getCarInfoFilePath()->mid(siFormBean->getCarInfoFilePath()->lastIndexOf("/")+1))){
        this->RetSIStatus(QString(SUMBAT)+"Bat文件修改失败，任务无法继续执行!",LOG_ERROR);return;
    }
    //第四,五步:执行bat文件
    if(!siCommonMethod->CompressionRunCmd(tmpPath+"/tools/"+SUMBAT)){
        this->RetSIStatus(QString(SUMBAT)+" 执行失败，任务无法继续执行!",LOG_ERROR);return;
    }
    if(!siCommonMethod->CompressionRunCmd(tmpPath+"/tools/"+JOINBAT)){
        this->RetSIStatus(QString(JOINBAT)+" 执行失败，任务无法继续执行!",LOG_ERROR);return;
    }
    if(QFile::exists(tmpPath+"/tools/join.mot")){this->SendConMessageLog("join.mot文件生成成功!",LOG_INFO);}
    else{
        this->RetSIStatus("join.mot文件生成失败!",LOG_ERROR);return;
    }
    //第六步：删除多余的文件夹
    if(!siCommonMethod->CompressionRemoveDir(tmpPath+"/ALL/ALL",carModelsName)){
        this->RetSIStatus("ALL路径下多余目录删除失败，任务无法继续执行!",LOG_ERROR);return;
    }
    //第七步：删除多余的文件
    if(!siCommonMethod->CompressionRemoveFile(tmpPath+"/ALL/ALL/"+carModelsName)){
        this->RetSIStatus("ALL/"+carModelsName+"路径下多余文件删除失败，任务无法继续执行!",LOG_ERROR);return;
    }
    //第八步:复制join.mot,APP.mot文件到指定目录
    if(!siCommonMethod->CompressionCopyMot(tmpPath,carModelsName,*siFormBean->getIDType(),    \
                                           siFormBean->getSoftList()->value(siFormBean->getSoftList()->size()-1).ApplicationVer,    \
                                           siFormBean->getSoftList()->value(siFormBean->getSoftList()->size()-1).CarInfoVer)){
        this->RetSIStatus(tmpPath+"路径下Mot文件复制到指定路径失败，任务无法继续执行!",LOG_ERROR);return;
    }else{this->SendConMessageLog("APP.mot&Join.mot成功复制到 "+ tmpPath.left(tmpPath.lastIndexOf("/"))+" 路径下!",LOG_INFO);}
    //第九步:压缩文件
    siFormBean->setSIStatus(SI_FILEZIP);
    emit RunOrderSignal(SIZIPFileflag);
}

/**
 * @def 推算RelyID
 *      flag = 1 表示手动输入依赖ID:原宏定义解析不为空
 *      flag = 2 表示手动输入依赖ID:原宏定义解析为空
 *      flag = 11 表示推算依赖ID
 * @brief SIFormMethod::InferRelyIDSlot
 */
void SIFormMethod::InferRelyIDSlot()
{
    QLogHelper::instance()->LogInfo("SIFormMethod->InferRelyIDSlot() 函数执行!");
    unsigned int flag=0;
    //开启子线程
    if(!excelThread->isRunning()){
        siFormBean->setSIStatus(SI_FILEDEFINE);
        excelThread->start();
    }
    //如果依赖ID不为空,则需要从量产管理表中获取依赖ID信息，从宏定义表中获取宏定义信息
    if(!siFormBean->getRelyID()->isEmpty()){
        flag=1;
        if(siFormBean->getDefineList()->size()==0&&!siFormBean->getSHDefineFilePath()->isEmpty()){
            flag=2;
        }
        emit InferRelyIDProcessSignal(*siFormBean->getRelyFilePath(),*siFormBean->getSHDefineFilePath(),*siFormBean->getRelyID(),*siFormBean->getRelyIDType(),"",flag);
    }else if(siFormBean->getDefineList()->size()==0){
        //推算依赖ID
        flag=11;
        emit InferRelyIDProcessSignal(*siFormBean->getRelyFilePath(),*siFormBean->getSHDefineFilePath(),*siFormBean->getID(),*siFormBean->getIDType(),siFormBean->getSoftList()->value(siFormBean->getSoftList()->size()-1).ApplicationPartNo,flag);
    }else if(siFormBean->getSoftList()>0){
        //如果宏定义获取成功则不需要推算依赖ID
        excelThread->quit();
        excelThread->wait();
        emit PretreatmentSignal();
    }
}

/**
 * @def 执行调用外部程序处理结束回调函数
 * @brief SIFormMethod::EndRunOrderSlot
 * @param result
 * @param flag
 */
void SIFormMethod::EndRunOrderSlot(const bool result,const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->EndRunOrderSlot() 函数执行!");
    //子线程阻塞
    fileThread->quit();
    fileThread->wait();
    switch (flag) {
    case SISVNUpdateflag:
        siFormBean->setSVNUpdateStatus(result);
        if(siFormBean->getSIStatus()==SI_SVNUPDATE){siFormBean->setSIStatus(SI_READY);}
        break;
    case SIUnzipFileflag:
        siFormBean->setUnzipflag(result);
        break;
    case SIZIPFileflag:
        siFormBean->setZIPflag(result);
        siFormBean->setSIStatus(SI_READY);
    default:
        break;
    }
    emit ShowMessageProcessSlot(flag,LOG_LOG);
    if(flag==SIUnzipFileflag){emit PretreatmentSignal();}
    if(flag==SIZIPFileflag){this->SendConMessageLog("SI 任务结束!",LOG_INFO);}
}


/**
 * @def 文件搜索结束回调函数
 * @brief SIFormMethod::EndFileSearcSlot
 * @param filePath
 * @param flag
 * @param isGoON
 */
void SIFormMethod::EndFileSearcSlot(const QString filePath, unsigned int flag, bool isGoON)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->EndFileSearcSlot() 函数执行!");
    unsigned int log_Flag=0;
    switch (flag) {
    case SIRelyFileflag:
        log_Flag=LOG_ALL;
        (*siFormBean->getRelyFilePath())=filePath;
        if(!filePath.isEmpty()){
            if(!excelThread->isRunning()){
                siFormBean->setSIStatus(SI_FILEREAD);
                excelThread->start();
            }
            emit ReadExcelThreadSignal(filePath,*siFormBean->getID(),*siFormBean->getIDType(),flag);
        };
        break;
        /*
    case SIPFileflag:
        log_Flag=LOG_ALL;
        (*siFormBean->getPFilePath())=filePath;
        break;
    case SISWFileflag:
        log_Flag=LOG_ALL;
        (*siFormBean->getSWFilePath())=filePath;
        break;
        */
    case SICarInfoFileflag:
        log_Flag=LOG_ALL;
        (*siFormBean->getCarInfoFilePath())=filePath;
        break;
    case SICodeFileflag:
        log_Flag=LOG_ALL;
        (*siFormBean->getCodeFilePath())=filePath;
        break;
    case SISHDefineFileflag:
        log_Flag=LOG_ALL;
        (*siFormBean->getSHDefineFilePath())=filePath;
        if(!filePath.isEmpty()){
            if(!excelThread->isRunning()){
                siFormBean->setSIStatus(SI_FILEREAD);
                excelThread->start();
            }
            emit ReadExcelThreadSignal(filePath,*siFormBean->getID(),*siFormBean->getIDType(),flag);
        }
        isGoON=false;
        break;
    }
    emit ShowMessageProcessSignal(flag,log_Flag);
    if(isGoON){
        emit SearchFileSignal(flag+1,isGoON);
    }
    else{
        fileThread->quit();
        fileThread->wait();
        if(!fileThread->isRunning()&&!excelThread->isRunning()){siFormBean->setSIStatus(SI_READY);}
    }
}

/**
 * @brief SIFormMethod::EndCopyCodeFileSlot
 * @param filePath
 * @param flag
 * @param result
 */
void SIFormMethod::EndCopyCodeFileSlot(const QString filePath,const bool result)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->EndCopyCodeFileSlot() 函数执行!");
    fileThread->quit();
    fileThread->wait();
    siFormBean->setCopyCodeflag(result);
    (*siFormBean->getCodeFilePath())=filePath;
    emit ShowMessageProcessSlot(SICopyCodeflag,LOG_LOG);
    emit PretreatmentSignal();
}

/**
 * @def 量产管理表信息读取完成后回调函数
 * @brief SIFormMethod::EndReadSoftExcelSlot
 * @param softList
 * @param errList
 */
void SIFormMethod::EndReadSoftExcelSlot(const QList<SI_SOFTNUMBERTable> softList,const QList<SI_ERRORTable> errList)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->EndReadSoftExcelSlot() 函数执行!");
    (*siFormBean->getSoftList())=softList;
    (*siFormBean->getErrList())=errList;
    if(!excelThread->isRunning()){
        excelThread->quit();
        excelThread->wait();
    }
    if(!fileThread->isRunning()&&!excelThread->isRunning()){siFormBean->setSIStatus(SI_READY);}
    emit ShowMessageProcessSignal(SIRelyMessageflag,LOG_ALL);
    emit ShowMessageProcessSignal(SIERRorMessageflag,LOG_LOG);
}
/**
 * @def 校验文件是否存在
 * @brief SIFormMethod::EndCheckFileSlot
 * @param result
 * @param errList
 */
void SIFormMethod::EndCheckBAFileSlot( const bool result, const QList<SI_ERRORTable> errList)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->EndCheckBAFileSlot() 函数执行!");
    fileThread->quit();
    fileThread->wait();
    siFormBean->setBAflag(result);
    (*siFormBean->getErrList())=errList;
    emit ShowMessageProcessSlot(SIBADirflag,LOG_LOG);
    emit ShowMessageProcessSlot(SIERRorMessageflag,LOG_LOG);
    emit PretreatmentSignal();
}

/**
 * @brief SIFormMethod::EndCheckCLFileSlot
 * @param result
 * @param errList
 */
void SIFormMethod::EndCheckCLFileSlot(const bool result, const QList<SI_ERRORTable> errList)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->EndCheckCLFileSlot() 函数执行!");
    fileThread->quit();
    fileThread->wait();
    siFormBean->setNEWCLflag(result);
    (*siFormBean->getErrList())=errList;
    emit ShowMessageProcessSlot(SICLVERCheckflag,LOG_LOG);
    emit ShowMessageProcessSlot(SIERRorMessageflag,LOG_LOG);
    emit PretreatmentSignal();
}


/**
 * @def SW一览表解析完成后回调函数
 * @brief SIFormMethod::EndReadDefineFileExcelSlot
 * @param defineList
 * @param errList
 */
void SIFormMethod::EndReadDefineFileExcelSlot(const QList<SI_DEFINEMESSAGE> defineList, const QList<SI_ERRORTable> errList)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->EndReadDefineFileExcelSlot() 函数执行!");
    (*siFormBean->getDefineList())=defineList;
    (*siFormBean->getErrList())=errList;
    excelThread->quit();
    excelThread->wait();
    emit ShowMessageProcessSlot(SISHDefineflag,LOG_ALL);
    emit ShowMessageProcessSignal(SIERRorMessageflag,LOG_LOG);
    if(!fileThread->isRunning()&&!excelThread->isRunning()){siFormBean->setSIStatus(SI_READY);}
    this->SendConMessageLog("文件搜索任务结束,可以执行预处理任务!",LOG_INFO);
}

/**
 * @brief SIFormMethod::EndInferRelyIDProcessSlot
 * @param softList
 * @param defineList
 * @param RelyID
 * @param flag
 */
void SIFormMethod::EndInferRelyIDProcessSlot(const QList<SI_SOFTNUMBERTable> softList, const QList<SI_DEFINEMESSAGE> defineList, const QList<SI_ERRORTable> errList, const unsigned int flag)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->EndInferRelyIDProcessSlot() 函数执行!");
    excelThread->quit();
    excelThread->wait();
    switch (flag) {
    case 1:
        (*siFormBean->getRelyIDSoftList())=softList;
        break;
    case 2:
        (*siFormBean->getRelyIDSoftList())=softList;
        (*siFormBean->getDefineList())=defineList;
        break;
    case 11:
        (*siFormBean->getRelyIDSoftList())=softList;
        (*siFormBean->getDefineList())=defineList;
        (*siFormBean->getErrList())=errList;
        break;
    }
    emit ShowMessageProcessSignal(SIRelyIDDealflag,LOG_LOG);
    emit ShowMessageProcessSignal(SIERRorMessageflag,LOG_LOG);
    emit PretreatmentSignal();
}



/**
 * @def 常规信息发送
 * @brief SIFormMethod::SendConMessageLog
 */
void SIFormMethod::SendConMessageLog(const QString txt, const unsigned int level)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->SendConMessageLog() 函数执行!");
    QStringList message;
    message.append(txt);
    emit ShowLogMessageSignal(message,level);
}

/**
 * @def 初始化程序状态，并发送发送信息
 * @brief SIFormMethod::RetSIStatus
 * @param txt
 * @param level
 */
void SIFormMethod::RetSIStatus(const QString txt, const unsigned int level)
{
    QLogHelper::instance()->LogInfo("SIFormMethod->RetSIStatus() 函数执行!");
    this->SendConMessageLog(txt,level);
    if(!fileThread->isRunning()&&!excelThread->isRunning()){siFormBean->setSIStatus(SI_READY);}
}


