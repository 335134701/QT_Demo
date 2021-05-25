#include "SIForm.h"
#include "ui_SIForm.h"

SIForm::SIForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SIForm)
{
    ui->setupUi(this);
}

SIForm::~SIForm()
{
    delete ui;
}
