#ifndef TEMPLE_H
#define TEMPLE_H

#include <QMainWindow>
#include "COMMONDEFINE.h"

namespace Ui {
class Temple;
}

class Temple : public QMainWindow
{
    Q_OBJECT

public:
    explicit Temple(QWidget *parent = 0);
    ~Temple();

private:
    Ui::Temple *ui;
    void InitStytle();
    void Init();
    void ConnectSlot();
};

#endif // TEMPLE_H
