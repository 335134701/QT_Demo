#include "AuExcelOperateThread.h"


/**
 * @brief AuExcelOperateThread::AuExcelOperateThread
 * @param parent
 */
AuExcelOperateThread::AuExcelOperateThread(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo("AuExcelOperateThread 构造函数执行!");
}

/**
 * @brief AuExcelOperateThread::Init
 */
void AuExcelOperateThread::Init()
{

}
