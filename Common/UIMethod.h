#ifndef UIMETHOD_H
#define UIMETHOD_H

#include <QObject>
#include "QLoghelper.h"

class UIMethod : public QObject
{
    Q_OBJECT
public:
    explicit UIMethod(QObject *parent = nullptr);


signals:

public slots:
    QString JudgeIDType(const QString ID);
};

#endif // UIMETHOD_H
