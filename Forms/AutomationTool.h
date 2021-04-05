#ifndef AUTOMATIONTOOL_H
#define AUTOMATIONTOOL_H

#include "QLoghelper.h"
#include "CommonBean.h"
#include <QMainWindow>
#include <QMouseEvent>
#include <Qmessagebox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QObject>


namespace Ui {
class AutomationTool;
}

class AutomationTool : public QMainWindow
{
    Q_OBJECT
    #define errFontColor "color:red;"
    #define comBakColor "background-color: rgb(255, 255, 255)"
    #define comFont "font: 75 11pt \"Consolas\";"
    #define nomFontColor "color:black;"

public:
    explicit AutomationTool(QWidget *parent = 0);
    ~AutomationTool();

private slots:

    void on_IDEdit_editingFinished();

    void on_RelyIDEdit_editingFinished();

    void on_ResultButton_clicked();

    void on_MotButton_clicked();

    void on_OutputButton_clicked();

    void on_CreateButton_clicked();

private:
    Ui::AutomationTool *ui;

    QString desktopPath;

    CommonBean *comBean;

    void init();

    void initStyle();
signals:
};

#endif // AUTOMATIONTOOL_H
