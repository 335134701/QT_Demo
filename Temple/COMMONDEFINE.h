#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H

#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include "QLoghelper.h"

#define DATETIME                QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
#define DATAStytle              "[ "+DATETIME+" ] "
#define LOGMESSAGE              "< "+QString(__FILE__)+"  "+QString(__FUNCTION__)+"()"+" > 函数执行!"


#endif // COMMONDEFINE_H
