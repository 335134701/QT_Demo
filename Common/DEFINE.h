#ifndef DEFINE_H
#define DEFINE_H

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
*
*
*/
#define SUCCESS             0
#define IDflag              1
#define IDTypeflag          2
#define RelyIDflag          3
#define RelyIDTypeflag      4
#define RelyFileflag        5
#define IniFileflag         6
#define PFileflag           7
#define SWFileflag          8
#define CarInfoFileflag     9
#define CarMapFileflag      10
#define CarOSDFileflag      11
#define APPFileflag         12
#define JoinFileflag        13
#define EEFileflag          14
#define ReadyFileflag       15
#define ConfigFileflag      16
#define DefineConfig        17
#define RelyMessageflag     18
#define ConfigMessageflag   19
#define EEFileWrieflag      20
#define ReadyFileWriteflag   21

#define DATETIME                QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
#define DATAStytle              "[ "+DATETIME+" ] "

#endif // DEFINE_H
