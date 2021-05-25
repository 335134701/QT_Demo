#include "MessageForm.h"
#include "ui_MessageForm.h"

MessageForm::MessageForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageForm)
{
    ui->setupUi(this);
}

MessageForm::~MessageForm()
{
    delete ui;
}
