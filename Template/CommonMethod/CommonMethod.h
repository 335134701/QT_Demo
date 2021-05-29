#ifndef COMMONMETHOD_H
#define COMMONMETHOD_H

#include <QObject>
#include <QSettings>
#include <QStandardPaths>
#include "COMMONDEFINE.h"

class CommonMethod : public QObject
{
    Q_OBJECT
public:

    explicit CommonMethod(QObject *parent = nullptr);

    const QString desktopDirPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    const QString applicationDirPath=QApplication::applicationDirPath();

    /********************************** SIForm **********************************/

    QString Get7zInstallPath();

    QString JudgeIDType(const QString ID);

signals:

public slots:
};

#endif // COMMONMETHOD_H
