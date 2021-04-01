#ifndef AUTOMATIONTOOL_H
#define AUTOMATIONTOOL_H

#include "QLoghelper.h"
#include "CommonBean.h"
#include <QMainWindow>
#include<QMouseEvent>

namespace Ui {
class AutomationTool;
}

class AutomationTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit AutomationTool(QWidget *parent = 0);
    ~AutomationTool();

private slots:

    void on_IDEdit_editingFinished();

    void on_RelyIDEdit_editingFinished();

    void on_RelyButton_clicked();

    void on_ResultButton_clicked();

    void on_MotButton_clicked();

    void on_OutputButton_clicked();

    void mousePressEvent(QMouseEvent *event);

    void IDEditMouse_Clicked();

private:
    Ui::AutomationTool *ui;
    const QString errFontStytle="color:red;background-color: rgb(255, 255, 255);font: 75 11pt \"Consolas\";";
    const QString nomalFontStytle="color:black;background-color: rgb(255, 255, 255);font: 75 11pt \"Consolas\";";
    CommonBean *comBean;
    void init();


signals:
    void IDEditMouse_Signal();
};

#endif // AUTOMATIONTOOL_H
