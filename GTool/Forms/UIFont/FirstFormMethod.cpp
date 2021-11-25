#include "FirstFormMethod.h"


/**
 * @brief FirstFormMethod::FirstFormMethod
 * @param parent
 */
FirstFormMethod::FirstFormMethod(QObject *parent) : QObject(parent)
{
    QLogHelper::instance()->LogInfo(LOGMESSAGE);
}
