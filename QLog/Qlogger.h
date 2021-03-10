#ifndef QLOGGER_H
#define QLOGGER_H

#include <QMainWindow>
#include "Qloghelper.h"

namespace Ui {
class Qlogger;
}

class Qlogger : public QMainWindow
{
    Q_OBJECT

public:
    explicit Qlogger(QWidget *parent = 0);
    ~Qlogger();


private slots:
    void on_InfoButton_clicked();

    void on_WarnButton_clicked();

    void on_DebugButton_clicked();

    void on_ErrorButton_clicked();

    void on_ClearButton_clicked();

private:
    Ui::Qlogger *ui;
};

#endif // QLOGGER_H
