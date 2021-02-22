#include "textfind.h"
#include "ui_textfind.h"

TextFind::TextFind(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextFind)
{
    ui->setupUi(this);
}

TextFind::~TextFind()
{
    delete ui;
}

void TextFind::on_find_filepath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(fileName);
    currentFile = fileName;
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();
    file.close();



    ui->filepath->setText(currentFile);
    ui->textEdit->setPlainText(text);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);

}

void TextFind::on_find_word_clicked()
{
    if (currentFile.isEmpty()) {
            QMessageBox::critical(this,"Error","文件不存!");
            return;
    }
    QString searchString = ui->find_word->text();
    ui->textEdit->find(searchString, QTextDocument::FindWholeWords);
}
