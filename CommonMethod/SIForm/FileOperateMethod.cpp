#include "FileOperateMethod.h"

/**
 * @brief FileOperateMethod::FileOperateMethod
 * @param parent
 */
FileOperateMethod::FileOperateMethod(QObject *parent) : QObject(parent)
{
     QLogHelper::instance()->LogInfo("FileOperateMethod 构造函数执行!");
}
