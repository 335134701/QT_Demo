#include "GToolBean.h"

/**
 * @brief GToolBean::GToolBean
 * @param parent
 */
GToolBean::GToolBean(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
    this->Init();
}

/**
 * @brief GToolBean::Init
 */
void GToolBean::Init()
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
    menuLog=new QList<int>();
    menuLogSingleflag=false;
}

/**
 * @brief GToolBean::getMenuLog
 * @return
 */
QList<int> *GToolBean::getMenuLog() const
{
    return menuLog;
}

/**
 * @brief GToolBean::setMenuLog
 * @param value
 */
void GToolBean::setMenuLog(QList<int> *value)
{
    menuLog = value;
}

/**
 * @brief GToolBean::getMenuLogSingleflag
 * @return
 */
bool GToolBean::getMenuLogSingleflag() const
{
    return menuLogSingleflag;
}

/**
 * @brief GToolBean::setMenuLogSingleflag
 * @param value
 */
void GToolBean::setMenuLogSingleflag(bool value)
{
    menuLogSingleflag = value;
}





