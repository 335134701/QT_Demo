#ifndef EXCELOPERATION_H
#define EXCELOPERATION_H

#include <QObject>

class ExcelOperation : public QObject
{
    Q_OBJECT
public:
    explicit ExcelOperation(QObject *parent = nullptr);

signals:

public slots:
};

#endif // EXCELOPERATION_H