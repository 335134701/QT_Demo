#include "Qlogger.h"
#include "Log.h"
#include <QApplication>
#include <QTextCodec>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qlogger w;
    w.show();

    QThread::currentThread()->setObjectName("主线程");
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);

    Log::instance()->init(QCoreApplication::applicationDirPath() + "/" +"etc/log.conf");
//    Log::instance()->debug("调试测试日志系统当中...");
//    Log::instance()->info("信息测试日志系统当中...");
//    Log::instance()->warn("警告测试日志系统当中...");
//    Log::instance()->error("错误测试日志系统当中...");

    return a.exec();
}
