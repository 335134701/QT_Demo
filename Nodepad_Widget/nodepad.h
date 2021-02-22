#ifndef NODEPAD_H
#define NODEPAD_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

namespace Ui {
class Nodepad;
}

class Nodepad : public QMainWindow
{
    Q_OBJECT

public:
    explicit Nodepad(QWidget *parent = 0);
    ~Nodepad();

private slots:
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionPrint_triggered();

    void on_actionExit_triggered();

    void on_actionSaveAs_triggered();

private:
    Ui::Nodepad *ui;
    QString currentFile;
};

#endif // NODEPAD_H
