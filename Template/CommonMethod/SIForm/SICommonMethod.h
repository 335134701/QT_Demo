#ifndef SICOMMONMETHOD_H
#define SICOMMONMETHOD_H

#include <QObject>
#include <QProcess>

#include "COMMONDEFINE.h"
#include "SIFormBean.h"

class SICommonMethod : public QObject
{
    Q_OBJECT
public:
    explicit SICommonMethod(QObject *parent = nullptr);

    QString JudgeIDType(const QString ID);

    QStringList GetBeforeAfterDirPath(const QString dirPath,const QList<SI_SOFTNUMBERTable> softList);

    bool CompressionCompareMotFile(const QString dirPath);

    bool CompressionChangeBatFile(const QString dirPath,const QString txt);

    bool CompressionRunCmd(const QString filePath);

    bool CompressionRemoveDir(const QString dirPath, const QString txt);

    static SI_ERRORTable SetERRMessage(const QString fileName,const QString errMessage);
    static SI_ERRORTable SetERRMessage(const QString fileName,const QString sheetName,const QString errMessage);
    static SI_ERRORTable SetERRMessage(const QString fileName, const QString sheetName,const unsigned int row,const unsigned int col,const QString errMessage);

signals:

public slots:
};

#endif // SICOMMONMETHOD_H
