#include <QDebug>
#include "Qlogger.h"
#include "ui_Qlogger.h"
#include <QApplication>

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
/**
 * @brief Qlogger::on_InfoButton_clicked
 */
void Qlogger::on_InfoButton_clicked()
{
    this->ui->textEdit->append("Info 按钮点击了一下!");
    QLogHelper::instance()->LogInfo("Info 按钮点击了一下!");
}
/**
 * @brief Qlogger::on_WarnButton_clicked
 */
void Qlogger::on_WarnButton_clicked()
{
    this->ui->textEdit->append("Warn 按钮点击了一下!");
    QLogHelper::instance()->LogWarn("Warn 按钮点击了一下!");
}
/**
 * @brief Qlogger::on_DebugButton_clicked
 */
void Qlogger::on_DebugButton_clicked()
{
    this->ui->textEdit->append("Debug 按钮点击了一下!");
    QLogHelper::instance()->LogDebug("Debug 按钮点击了一下!");
}
/**
 * @brief Qlogger::on_ErrorButton_clicked
 */
void Qlogger::on_ErrorButton_clicked()
{
    this->ui->textEdit->append("Error 按钮点击了一下!");
    QLogHelper::instance()->LogError("Error 按钮点击了一下!");
}

void Qlogger::on_ClearButton_clicked()
{
    this->ui->textEdit->clear();
    QLogHelper::instance()->LogInfo("Clear 按钮点击了一下!");
}
