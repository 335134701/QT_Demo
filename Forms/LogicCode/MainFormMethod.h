#ifndef MAINFORMMETHOD_H
#define MAINFORMMETHOD_H

#include <QObject>


#include "COMMONDEFINE.h"
#include "MainFormBean.h"
#include "CommonMethod.h"

class MainFormMethod : public QObject
{
    Q_OBJECT
public:
    explicit MainFormMethod(QObject *parent = nullptr);

    MainFormBean *getMainFormBean() const;
    void setMainFormBean(MainFormBean *value);

    CommonMethod *getCommonMethod() const;
    void setCommonMethod(CommonMethod *value);

signals:

public slots:


private :

    MainFormBean *mainFormBean;

    CommonMethod *commonMethod;

    void Init();

    void ConnectSlot();
};

#endif // MAINFORMMETHOD_H
