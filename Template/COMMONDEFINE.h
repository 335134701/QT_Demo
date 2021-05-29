#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H

#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QMap>
#include "QLoghelper.h"

/*                       MainForm                       */



/*                       SIForm                         */
/*
* @def 根据获取的文件列表，解析需要的文件路径
*      flag 说明:
*      0   无任何表示
*      1   表示机种番号信息获取
*      2   表示机种类型信息获取
*      3   表示依赖机种番号信息获取
*      4   表示依赖机种番号类型信息获取
*      5   表示量产管理表信息获取
*      6   表示P票信息获取
*      7   表示SW确认文件获取
*      8   表示CarInfoMot文件获取
*
*      160 表示文件压缩状态
*
*/


#define SUCCESS             0
#define IDflag              1
#define IDTypeflag          2
#define RelyIDflag          3
#define RelyIDTypeflag      4
#define RelyFileflag        5
#define PFileflag           6
#define SWFileflag          7
#define CarInfoFileflag     8
#define Compression         160



/*                       LogForm                        */
#define DATETIME                QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
#define DATAStytle              "[ "+DATETIME+" ] "

enum Log_Level{
    LOG_INFO    =   0,
    LOG_WARN    =   1,
    LOG_DEBUG   =   2,
    LOG_ERROR   =   3
};

#endif // COMMONDEFINE_H
