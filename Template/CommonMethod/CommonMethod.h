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

    QString GetSVNInstallPath();

    bool CopyFile(const QString desDirPath,const QString srcDirPath);
    

signals:

public slots:
};

#endif // COMMONMETHOD_H
