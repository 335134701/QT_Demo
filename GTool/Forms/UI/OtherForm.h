#ifndef OTHERFORM_H
#define OTHERFORM_H

#include <QWidget>
#include "COMMONDEFINE.h"

namespace Ui {
class OtherForm;
}

class OtherForm : public QWidget
{
    Q_OBJECT

public:
    explicit OtherForm(QWidget *parent = 0);
    ~OtherForm();

private slots:
    void on_pushButton_clicked();

private:
    Ui::OtherForm *ui;
    void Init();
    void ConnectSlot();
};

#endif // OTHERFORM_H
