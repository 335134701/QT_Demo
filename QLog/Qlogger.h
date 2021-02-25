#ifndef QLOGGER_H
#define QLOGGER_H

#include <QMainWindow>

namespace Ui {
class Qlogger;
}

class Qlogger : public QMainWindow
{
    Q_OBJECT

public:
    explicit Qlogger(QWidget *parent = 0);
    ~Qlogger();

private:
    Ui::Qlogger *ui;
};

#endif // QLOGGER_H
