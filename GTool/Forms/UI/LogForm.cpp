#include "LogForm.h"
#include "ui_LogForm.h"

LogForm::LogForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogForm)
{
    ui->setupUi(this);
}

LogForm::~LogForm()
{
    delete ui;
}
