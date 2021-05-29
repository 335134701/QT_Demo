#ifndef AUTOMATIONFORMBEAN_H
#define AUTOMATIONFORMBEAN_H

#include <QObject>

#include "COMMONDEFINE.h"
#include "CommonMethod.h"

class AutomationFormBean : public QObject
{
    Q_OBJECT
public:
    explicit AutomationFormBean(QObject *parent = nullptr);

    CommonMethod *getCommonMethod() const;
    void setCommonMethod(CommonMethod *value);

signals:

public slots:

private :

    //公共方法对象
    CommonMethod *commonMethod;

    void Init();
};

#endif // AUTOMATIONFORMBEAN_H
