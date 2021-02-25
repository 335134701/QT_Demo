#ifndef DEFINE_H
#define DEFINE_H

// 开发过程中暂时使用qDebug替代 1-使用debug打印输出  0-使用日志输出
#if 0
// 以文件行列记录信息，非以类名对象形式
#define LOG_DEBUG(msg) qDebug() << QString("[%1][%2][%3][%4]%5")  \
                                           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"))  \
                                           .arg(QString("%1:%2:%3").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__))  \
                                           .arg(QThread::currentThread()->objectName())  \
                                           .arg("DEBUG")  \
                                           .arg(msg);

#define LOG_INFO(msg) qDebug() << QString("[%1][%2][%3][%4]%5")  \
                                           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"))  \
                                           .arg(QString("%1:%2:%3").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__))  \
                                           .arg(QThread::currentThread()->objectName())  \
                                           .arg("INFO")  \
                                           .arg(msg);

#define LOG_WARN(msg) qDebug() << QString("[%1][%2][%3][%4]%5")  \
                                           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"))  \
                                           .arg(QString("%1:%2:%3").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__))  \
                                           .arg(QThread::currentThread()->objectName())  \
                                           .arg("WARN")  \
                                           .arg(msg);

#define LOG_ERROR(msg) qDebug() << QString("[%1][%2][%3][%4]%5")  \
                                           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"))  \
                                           .arg(QString("%1:%2:%3").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__))  \
                                           .arg(QThread::currentThread()->objectName())  \
                                           .arg("ERROR")  \
                                           .arg(msg);


// 以类名对象形式记录信息
#define LOG_OBJECT_DEBUG(msg) qDebug() << QString("[%1][%2][%3][%4]%5")  \
                                           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"))  \
                                           .arg(QString("%1::%2:%3").arg(this->metaObject()->className()).arg(__FUNCTION__).arg(__LINE__))  \
                                           .arg(QThread::currentThread()->objectName())  \
                                           .arg("DEBUG")  \
                                           .arg(msg);

#define LOG_OBJECT_INFO(msg) qDebug() << QString("[%1][%2][%3][%4]%5")  \
                                           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"))  \
                                           .arg(QString("%1::%2:%3").arg(this->metaObject()->className()).arg(__FUNCTION__).arg(__LINE__))  \
                                           .arg(QThread::currentThread()->objectName())  \
                                           .arg("INFO")  \
                                           .arg(msg);

#define LOG_OBJECT_WARN(msg) qDebug() << QString("[%1][%2][%3][%4]%5")  \
                                           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"))  \
                                           .arg(QString("%1::%2:%3").arg(this->metaObject()->className()).arg(__FUNCTION__).arg(__LINE__))  \
                                           .arg(QThread::currentThread()->objectName())  \
                                           .arg("WARN")  \
                                           .arg(msg);

#define LOG_OBJECT_ERROR(msg) qDebug() << QString("[%1][%2][%3][%4]%5")  \
                                           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"))  \
                                           .arg(QString("%1::%2:%3").arg(this->metaObject()->className()).arg(__FUNCTION__).arg(__LINE__))  \
                                           .arg(QThread::currentThread()->objectName())  \
                                           .arg("ERROR")  \
                                           .arg(msg);
// 写入日志系统
#else

// 以文件行列记录信息，非以类名对象形式
#define LOG_DEBUG(msg) Log::instance()->debug(QString("[%1]%2").arg(QString("%1:%2:%3").arg(__FILE__).arg(__LINE__).arg(__FUNCTION__)).arg(msg));

#define LOG_INFO(msg) Log::instance()->info(msg);

#define LOG_WARN(msg) Log::instance()->warn(msg);

#define LOG_ERROR(msg) Log::instance()->error(msg);


// 以类名对象形式记录信息
#define LOG_OBJECT_DEBUG(msg) Log::instance()->debug(QString("[%1]%2").arg(QString("%1::%2:%3").arg(this->metaObject()->className()).arg(__FUNCTION__).arg(__LINE__)).arg(msg));

#define LOG_OBJECT_INFO(msg) Log::instance()->info(msg);

#define LOG_OBJECT_WARN(msg) Log::instance()->warn(msg);

#define LOG_OBJECT_ERROR(msg) Log::instance()->error(msg);

————————————————
版权声明：本文为CSDN博主「Empty_Android」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/empty_android/article/details/104024598

#endif // DEFINE_H
