#ifndef UIMETHOD_H
#define UIMETHOD_H

#include <QObject>

class UIMethod : public QObject
{
    Q_OBJECT
public:
    explicit UIMethod(QObject *parent = nullptr);

signals:

public slots:
};

#endif // UIMETHOD_H