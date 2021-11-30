#ifndef GTOOL_H
#define GTOOL_H

#include <QMainWindow>
#include "GToolBean.h"
#include "COMMONDEFINE.h"

namespace Ui {
class GTool;
}

class GTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit GTool(QWidget *parent = 0);
    ~GTool();

signals:
    void Set_ShowMenuLog_Signal(QList<int> menuLog);

private slots:
    void onLogTriggred_Slot();
    void Set_ShowMenuLog_Slot(QList<int> menuLog);

private:
    Ui::GTool *ui;

    GToolBean *gToolBean;

    void InitStytle();
    void Init();
    void ConnectSlot();
};

#endif // GTool_H
