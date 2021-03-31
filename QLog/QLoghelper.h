#ifndef QLOGHELPER_H
#define QLOGHELPER_H

#include <QObject>
#include <QFileInfo>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include "log4qt/logger.h"
#include "log4qt/basicconfigurator.h"
#include "log4qt/propertyconfigurator.h"


class QLogHelper : public QObject
{
    Q_OBJECT
public:
    explicit QLogHelper(QObject *parent = nullptr);
    ~QLogHelper();
    static QLogHelper *instance();
    void Init();
    Log4Qt::Logger* GetLogInfo();
    Log4Qt::Logger* GetLogWarn();
    Log4Qt::Logger* GetLogDebug();
    Log4Qt::Logger* GetLogError();
    Log4Qt::Logger* GetLogConsole();
    void LogInfo(QString);
    void LogWarn(QString);
    void LogDebug(QString);
    void LogError(QString);
    static QString confFilePath;

signals:

public slots:

private:
    Log4Qt::Logger *logInfo=NULL;
    Log4Qt::Logger *logWarn=NULL;
    Log4Qt::Logger *logDebug=NULL;
    Log4Qt::Logger *logError=NULL;
    Log4Qt::Logger *logConsole=NULL;
};

#endif // QLOGHELPER_H
