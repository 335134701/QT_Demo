#ifndef COMMONBEAN_H
#define COMMONBEAN_H

#include <QObject>
#include "QLoghelper.h"

class CommonBean : public QObject
{
    Q_OBJECT
public:
    explicit CommonBean(QObject *parent = nullptr);

    void ParameterInit();

    QMap<QString, QString> getRExpression() const;

    QString getID() const;
    void setID(const QString &value);

    QMap<QString, QChar> getErrCode() const;

    QString getRelyID() const;
    void setRelyID(const QString &value);

signals:

public slots:
private :
    QMap<QString,QString> RExpression;
    //错误状态码集合
    QMap<QString,QChar> errCode;
    //机种番号
    QString ID;
    //依赖机种番号
    QString RelyID;
    void Init();

};

#endif // COMMONBEAN_H
