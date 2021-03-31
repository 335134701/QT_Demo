#ifndef COMMONBEAN_H
#define COMMONBEAN_H

#include <QObject>
#include "QLoghelper.h"

class CommonBean : public QObject
{
    Q_OBJECT
public:
    explicit CommonBean(QObject *parent = nullptr);

    static CommonBean *instance();

    QMap<QString, QString> getRExpression() const;
    void setRExpression(const QMap<QString, QString> &value);

    QString getID() const;
    void setID(const QString &value);

signals:

public slots:
private :
    QMap<QString,QString> RExpression;
    QString ID;
    void Init();
};

#endif // COMMONBEAN_H
