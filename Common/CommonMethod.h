#ifndef COMMONMETHOD_H
#define COMMONMETHOD_H

#include <QObject>

class CommonMethod : public QObject
{
    Q_OBJECT
public:
    explicit CommonMethod(QObject *parent = nullptr);

    QString JudgeIDType(QString ID);

signals:

public slots:
};

#endif // COMMONMETHOD_H
