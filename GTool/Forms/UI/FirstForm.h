#ifndef FIRSTFORM_H
#define FIRSTFORM_H

#include <QWidget>
#include "COMMONDEFINE.h"

namespace Ui {
class FirstForm;
}

class FirstForm : public QWidget
{
    Q_OBJECT

public:
    explicit FirstForm(QWidget *parent = 0);
    ~FirstForm();

private:
    Ui::FirstForm *ui;
    void Init();
    void ConnectSlot();
};

#endif // FIRSTFORM_H
