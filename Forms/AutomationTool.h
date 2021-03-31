#ifndef AUTOMATIONTOOL_H
#define AUTOMATIONTOOL_H

#include "QLoghelper.h"
#include "CommonBean.h"
#include <QMainWindow>

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


    void on_IDEdit_textChanged(const QString &arg1);

private:
    Ui::AutomationTool *ui;
    const QString errFontStytle="color:red;background-color: rgb(255, 255, 255);font: 75 11pt \"Consolas\";";
    const QString nomalFontStytle="color:black;background-color: rgb(255, 255, 255);font: 75 11pt \"Consolas\";";
    void init();
};

#endif // AUTOMATIONTOOL_H
