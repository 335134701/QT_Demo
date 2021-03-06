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
 * @brief Qlogger::SetQLoghelper
 * @param log
 */
void Qlogger::SetQLoghelper(QLoghelper *log){
    this->log=log;
}
/**
 * @brief Qlogger::on_InfoButton_clicked
 */
void Qlogger::on_InfoButton_clicked()
{
    this->ui->textEdit->append("Info 按钮点击了一下!");
    this->log->LogInfo("Info 按钮点击了一下!");
}
/**
 * @brief Qlogger::on_WarnButton_clicked
 */
void Qlogger::on_WarnButton_clicked()
{
    this->ui->textEdit->append("Warn 按钮点击了一下!");
    this->log->LogWarn("Warn 按钮点击了一下!");
}
/**
 * @brief Qlogger::on_DebugButton_clicked
 */
void Qlogger::on_DebugButton_clicked()
{
    this->ui->textEdit->append("Debug 按钮点击了一下!");
    this->log->LogDebug("Debug 按钮点击了一下!");
}
/**
 * @brief Qlogger::on_ErrorButton_clicked
 */
void Qlogger::on_ErrorButton_clicked()
{
    this->ui->textEdit->append("Error 按钮点击了一下!");
    this->log->LogError( "Info 按钮点击了一下!");
}
