#ifndef AUCOMMONMETHOD_H
#define AUCOMMONMETHOD_H

#include <QObject>

#include "COMMONDEFINE.h"

class AuCommonMethod : public QObject
{
    Q_OBJECT
public:
    explicit AuCommonMethod(QObject *parent = nullptr);

    QString JudgeIDType(const QString ID);

signals:

public slots:
};

#endif // AUCOMMONMETHOD_H
