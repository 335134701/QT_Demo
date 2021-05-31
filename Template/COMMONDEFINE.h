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
*      9   表示SVN更新状态标记
*
*      160 表示文件压缩状态
*
*/
#define SISUCCESS             0
#define SIIDflag              1
#define SIIDTypeflag          2
#define SIRelyIDflag          3
#define SIRelyIDTypeflag      4
#define SIRelyFileflag        5
#define SIPFileflag           6
#define SISWFileflag          7
#define SICarInfoFileflag     8
#define SISVNUpdateflag       9
#define SICompression         160



/*                       LogForm                        */
#define DATETIME                QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
#define DATAStytle              "[ "+DATETIME+" ] "

/**
 * @def LogForm日志输出显示级别
 * @brief The Log_Level enum
 */
enum Log_Level{
    LOG_INFO    =   1,
    LOG_WARN    =   2,
    LOG_DEBUG   =   3,
    LOG_ERROR   =   4
};

/**
 * @def 初始化标记位，根据标记位，初始化相关参数
 * @brief The RestParameter enum
 */
enum RestParameter{
    RET_ID                  =   1,
    RET_SVNFilePath         =   2,
    RET_OutPutFilePath      =   3
};

/**
 * @def 标记位，Log 全部显示，Log输出显示，Table显示
 * @brief The Log_Flag enum
 */
enum Log_Flag{
    LOG_ALL     =   1,
    LOG_LOG     =   2,
    LOG_TABLE   =   3
};


#endif // COMMONDEFINE_H
