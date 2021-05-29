#ifndef LOGFORMMETOD_H
#define LOGFORMMETOD_H

#include <QObject>
#include <QStringListModel>

#include "COMMONDEFINE.h"
#include "CommonMethod.h"
#include "LogFormBean.h"

class LogFormMetod : public QObject
{
    Q_OBJECT
public:
    explicit LogFormMetod(QObject *parent = nullptr);

    LogFormBean *getLogFormBean() const;
    void setLogFormBean(LogFormBean *value);

    void PrintMessage(const QStringList message,const unsigned int level);

signals:

public slots:

    void ShowLogMessageSlot(const QStringList message,const unsigned int level);


private :

    LogFormBean *logFormBean;

    void Init();

    void ConnectSlot();
};

#endif // LOGFORMMETOD_H
