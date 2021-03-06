#include "Qloghelper.h"

using namespace Log4Qt;

QLoghelper::QLoghelper()
{

}

QLoghelper::QLoghelper(QString confFilePath)
{
    this->confFilePath=confFilePath;
    this->Init();
}
/**
 * @brief QLogHelper::Init
 * @descr 实例化配置文件中节点对象
 */
void QLoghelper::Init(){
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

void QLoghelper::SetConfigFilePath(QString confFilePath){
    this->confFilePath=confFilePath;
}
/**
 * @brief QLoghelper::GetLogInfo
 * @return
 */
Logger* QLoghelper::GetLogInfo(){
    return this->logInfo;
}
/**
 * @brief QLoghelper::GetLogWarn
 * @return
 */
Logger* QLoghelper::GetLogWarn(){
    return this->logWarn;
}
/**
 * @brief QLoghelper::GetLogDebug
 * @return
 */
Logger* QLoghelper::GetLogDebug(){
    return this->logDebug;
}
/**
 * @brief QLoghelper::GetLogError
 * @return
 */
Logger* QLoghelper::GetLogError(){
    return this->logError;
}
/**
 * @brief QLoghelper::GetLogConsole
 * @return
 */
Logger* QLoghelper::GetLogConsole(){
    return this->logConsole;
}
/**
 * @brief QLoghelper::LogInfo
 * @param txt
 */
void QLoghelper::LogInfo(QString txt){
    this->logConsole->info(txt);
    this->logInfo->info(txt);
}
/**
 * @brief QLoghelper::LogWarn
 * @param txt
 */
void QLoghelper::LogWarn(QString txt){
    this->logConsole->warn(txt);
    this->logWarn->warn(txt);
}
/**
 * @brief QLogHelper::LogDebug
 * @param txt
 */
void QLoghelper::LogDebug(QString txt){
    this->logConsole->debug(txt);
    this->logDebug->debug(txt);
}
/**
 * @brief QLoghelper::LogError
 * @param txt
 */
void QLoghelper::LogError(QString txt){
    this->logConsole->error(txt);
    this->logError->error(txt);
}
