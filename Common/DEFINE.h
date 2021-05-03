#ifndef DEFINE_H
#define DEFINE_H

/*
* @def 根据获取的文件列表，解析需要的文件路径
*      flag 说明:
*      0   无任何表示
*      1   表示机种番号信息获取
*      2   表示机种类型信息获取
*      3   表示量产管理表信息获取
*      4   表示Ini文件信息获取
*      5   表示P票信息获取
*      6   表示SW确认文件获取
*      7   表示CarInfoMot文件获取
*      8   表示CarMapMot文件获取
*      9   表示OSDMot文件获取
*      10   表示 joinMot 文件获取
*      11   表示 appMot   文件获取
*      12   表示 DR会議運用手順_様式7模板文件 获取
*      13   表示 確認シート 文件获取
*      14   表示 EntryAVM採用車種 文件获取
*/
#define SUCCESS             0
#define IDflag              1
#define IDTypeflag          2
#define RelyFileflag        3
#define IniFileflag         4
#define PFileflag           5
#define SWFileflag          6
#define CarInfoFileflag     7
#define CarMapFileflag      8
#define CarOSDFileflag      9
#define APPFileflag         10
#define JoinFileflag        11
#define EEFileflag          12
#define ReadyFileflag       13
#define ConfigFileflag      14

#define DATETIME                QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
#define DATAStytle              "[ "+DATETIME+" ] "
#define IDRelyID                "IDRelyID"
#define RelyFileError           "RelyFileError"
#define SVNDirError             "SVNDirError"
#define IniFileError            "IniFileError"
#define APPFileError            "APPFileError"
#define JoinFileError           "JoinFileError"
#define PFileError              "PFileError"
#define SWFileError             "SWFileError"
#define CarInfoFileError        "CarInfoFileError"
#define CarMapFileError         "CarMapFileError"
#define CarOSDFileError         "CarOSDFileError"
#define EEFileError             "EEFileError"
#define ReadyFileError          "ReadyFileError"
#define ConfigFileError         "ConfigFileError"

#endif // DEFINE_H
