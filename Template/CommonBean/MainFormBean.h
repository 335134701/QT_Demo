#ifndef MAINFORMBEAN_H
#define MAINFORMBEAN_H

#include <QObject>


#include "COMMONDEFINE.h"
#include "CommonMethod.h"

class MainFormBean : public QObject
{
    Q_OBJECT
public:
    explicit MainFormBean(QObject *parent = nullptr);

    CommonMethod *getCommonMethod() const;
    void setCommonMethod(CommonMethod *value);

signals:

public slots:

private :

    //公共方法对象
    CommonMethod *commonMethod;

    void Init();
};

#endif // MAINFORMBEAN_H
