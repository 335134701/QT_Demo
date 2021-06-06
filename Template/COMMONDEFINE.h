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
*      10  表示项目源码压缩包路径
*      11  表示Build模板文件路径
*      12  表示SW宏定义一览表文件路径
*      13  表示BeforeAfter文件是否存在(包括版本号检查)
*      14  解析量产管理表内容
*      15  表示文件压缩状态标记
*      16  表示SW一览表解析完成
*      17  表示项目源码复制标记位
*      19  表示joinmot文件标记位
*      150 表示错误信息标记位
*      151 表示普通信息标记位
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
#define SICodeFileflag        10
#define SIBuildFileflag       11
#define SISHDefineFileflag    12
#define SIBADirflag           13
#define SIRelyMessageflag     14
#define SIUnzipFileflag       15
#define SISHDefineflag        16
#define SICopyCodeflag        17
#define SIJoinMotflag         18
#define SIERRorMessageflag    150
#define SIMessageflag         151


/*                       AutomationForm                         */
/*
* @def 根据获取的文件列表，解析需要的文件路径
*      flag 说明:
*      0   无任何表示
*      1   表示机种番号信息获取
*      2   表示机种类型信息获取
*      3   表示依赖机种番号信息获取
*      4   表示依赖机种番号类型信息获取
*      5   表示量产管理表信息获取
*      6   表示Ini文件信息获取
*      7   表示P票信息获取
*      8   表示SW确认文件获取
*      9   表示CarInfoMot文件获取
*      10   表示CarMapMot文件获取
*      11   表示OSDMot文件获取
*      12   表示 joinMot 文件获取
*      13   表示 appMot   文件获取
*      14   表示 DR会議運用手順_様式7模板文件 获取
*      15   表示 確認シート 文件获取
*      16   表示 EntryAVM採用車種 文件获取
*      17   宏定义
*      18   解析量产管理表内容
*      19   解析採用車種コンフィグ詳細表内容
*      20   写入EE文件结束状态
*      21   写入确认表返回值状态
*      22   其他文件check
*      23   写入EE文件结束状态
*      24   写入确认表返回值状态
*
*/
#define AuSUCCESS             0
#define AuIDflag              1
#define AuIDTypeflag          2
#define AuRelyIDflag          3
#define AuRelyIDTypeflag      4
#define AuRelyFileflag        5
#define AuIniFileflag         6
#define AuPFileflag           7
#define AuSWFileflag          8
#define AuCarInfoFileflag     9
#define AuCarMapFileflag      10
#define AuCarOSDFileflag      11
#define AuAPPFileflag         12
#define AuJoinFileflag        13
#define AuEEFileflag          14
#define AuReadyFileflag       15
#define AuConfigFileflag      16
#define AuDefineConfig        17
#define AuRelyMessageflag     18
#define AuConfigMessageflag   19
#define AuEEFileWrieflag      20
#define AuReadyFileWriteflag  21
#define AuFileCheck           22
#define AuEEFileReadflag      23
#define AuReadyFileReadflag   24
#define AuZIPFile             25


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
    RET_RELYID              =   2,
    RET_SVNFilePath         =   3,
    RET_OutPutFilePath      =   4
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
