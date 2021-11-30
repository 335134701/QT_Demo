#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H

#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include "QLoghelper.h"

#define DATETIME                QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
#define DATAStytle              "[ "+DATETIME+" ] "
#define LOGMESSAGE              "<< "+QString(__FILE__)+" >> "+QString(__FUNCTION__)+"() 函数执行!"


enum MenuLogIndex{
    Single_MenuLog=0,
    Info_MenuLog=1,
    Warn_MenuLog=2,
    Debug_MenuLog=3,
    Error_MenuLog=4,
    Other_MenuLog=5
};


#endif // COMMONDEFINE_H
