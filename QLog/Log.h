#ifndef LOG_H
#define LOG_H


#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>

#include "log4qt/logger.h"
#include "log4qt/basicconfigurator.h"

/************************************************************\
 * 类名：Log
 * 描述：封装log4qt开源库类
 * 注意：模块已带log4qt for qt4 和 qt5两个版本;
 *      模块已带log4qt配置文件，分为4级别：DEBUG > INFO > WARN > ERROR
 * 信号：
 * 函数：
 *      instance() - 获取日志唯一实例
 * 槽函数：
 *      slot_init() - 初始化加载配置文件
 *      slot_debug() - 调试级别日志
 *      slot_info() - 信息级别日志
 *      slot_warn() - 警告级别日志
 *      slot_error() - 错误级别日志
 *
 * 作者：红模仿
 * 联系方式：QQ21497936
 * 博客地址：https://blog.csdn.net/qq21497936
 *      日期          版本号        描述
 *  2019年04月09日    v1.0.0     基础日志模块
\************************************************************/


class Log : public QObject
{
    Q_OBJECT
public:
    explicit Log(QObject *parent = nullptr);

signals:

public slots:
    static Log * instance();

public slots:
    void init(QString configFilePath);

public slots:
    void debug(QString msg);
    void info(QString msg);
    void warn(QString msg);
    void error(QString msg);

private:
    static Log *_pInstance;
    static QMutex _mutex;
    static Log4Qt::Logger * _pLoggerDebug;
    static Log4Qt::Logger * _pLoggerInfo;
    static Log4Qt::Logger * _pLoggerWarn;
    static Log4Qt::Logger * _pLoggerError;
    static QString _configFilePath;

};


#endif // LOG_H
