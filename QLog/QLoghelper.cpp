#include "QLogHelper.h"

using namespace Log4Qt;


QLogHelper::QLogHelper(QObject *parent) : QObject(parent)
{
    this->Init();
}

QLogHelper::~QLogHelper(){
}

QLogHelper *QLogHelper::instance(){
    static QLogHelper log;
    return &log;
}
void QLogHelper::Init(){
    //判断文件是否存在
    QFileInfo configFile(this->confFilePath);
    if(!this->confFilePath.isEmpty()&&configFile.exists()){
        PropertyConfigurator::configure(this->confFilePath);
        //实例化节点对象
        if(logInfo==NULL){ logInfo = Log4Qt::Logger::logger("info");}
        if(logWarn==NULL){ logWarn = Log4Qt::Logger::logger("warn");}
        if(logDebug==NULL){ logDebug = Log4Qt::Logger::logger("debug");}
        if(logError==NULL){ logError = Log4Qt::Logger::logger("error");}
        if(logConsole==NULL){ logConsole = Log4Qt::Logger::logger("all");}
    }
}
/**
 * @brief QLogHelper::GetLogInfo
 * @return
 */
Logger* QLogHelper::GetLogInfo(){
    return this->logInfo;
}
/**
 * @brief QLogHelper::GetLogWarn
 * @return
 */
Logger* QLogHelper::GetLogWarn(){
    return this->logWarn;
}
/**
 * @brief QLogHelper::GetLogDebug
 * @return
 */
Logger* QLogHelper::GetLogDebug(){
    return this->logDebug;
}
/**
 * @brief QLogHelper::GetLogError
 * @return
 */
Logger* QLogHelper::GetLogError(){
    return this->logError;
}
/**
 * @brief QLogHelper::GetLogConsole
 * @return
 */
Logger* QLogHelper::GetLogConsole(){
    return this->logConsole;
}
/**
 * @brief QLogHelper::LogInfo
 * @param txt
 */
void QLogHelper::LogInfo(QString txt){
    this->logConsole->info(txt);
    this->logInfo->info(txt);
}
/**
 * @brief QLogHelper::LogWarn
 * @param txt
 */
void QLogHelper::LogWarn(QString txt){
    this->logConsole->warn(txt);
    this->logWarn->warn(txt);
}
/**
 * @brief QLogHelper::LogDebug
 * @param txt
 */
void QLogHelper::LogDebug(QString txt){
    this->logConsole->debug(txt);
    this->logDebug->debug(txt);
}
/**
 * @brief QLogHelper::LogError
 * @param txt
 */
void QLogHelper::LogError(QString txt){
    this->logConsole->error(txt);
    this->logError->error(txt);
}

