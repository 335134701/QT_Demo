#include "Qlogger.h"
#include "ui_Qlogger.h"

Qlogger::Qlogger(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Qlogger)
{
    ui->setupUi(this);
}

Qlogger::~Qlogger()
{
    delete ui;
}
