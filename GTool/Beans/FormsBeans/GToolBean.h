#ifndef GTOOLBEAN_H
#define GTOOLBEAN_H

#include <QObject>
#include "COMMONDEFINE.h"

class GToolBean : public QObject
{
    Q_OBJECT
public:
    explicit GToolBean(QObject *parent = nullptr);

    QList<int> *getMenuLog() const;
    void setMenuLog(QList<int> *value);

    bool getMenuLogSingleflag() const;
    void setMenuLogSingleflag(bool value);

signals:

public slots:

private:

    //菜单Log选择集合
    QList<int> *menuLog;

    //菜单Log单选标记
    bool menuLogSingleflag;

    void Init();
};

#endif // GTOOLBEAN_H
