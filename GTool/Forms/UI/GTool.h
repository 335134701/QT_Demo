#ifndef GTOOL_H
#define GTOOL_H

#include <QMainWindow>
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

private slots:
    void onLogTriggred();

private:
    Ui::GTool *ui;

    unsigned int currentLodindex=0;

    void InitStytle();
    void Init();
    void ConnectSlot();
};

#endif // GTool_H
