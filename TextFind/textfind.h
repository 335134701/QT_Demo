#ifndef TEXTFIND_H
#define TEXTFIND_H

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTextDocument>

namespace Ui {
class TextFind;
}

class TextFind : public QWidget
{
    Q_OBJECT

public:
    explicit TextFind(QWidget *parent = 0);
    ~TextFind();

private slots:
    void on_find_filepath_clicked();

    void on_find_word_clicked();

private:
    Ui::TextFind *ui;
    QString currentFile;
};

#endif // TEXTFIND_H
