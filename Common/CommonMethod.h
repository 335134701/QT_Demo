#ifndef COMMONMETHOD_H
#define COMMONMETHOD_H

#include <QObject>

class CommonMethod : public QObject
{
    Q_OBJECT
public:
    explicit CommonMethod(QObject *parent = nullptr);

signals:

public slots:
};

#endif // COMMONMETHOD_H