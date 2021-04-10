#ifndef COMMONMETHOD_H
#define COMMONMETHOD_H

#include <QObject>
#include "QLoghelper.h"

class CommonMethod : public QObject
{
    Q_OBJECT
public:
    explicit CommonMethod(QObject *parent = nullptr);

signals:

public slots:
};

#endif // COMMONMETHOD_H
