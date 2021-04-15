#ifndef AUTOMATIONTOOL_H
#define AUTOMATIONTOOL_H

#include "QLoghelper.h"
#include "CommonBean.h"
#include "UIMethod.h"
#include <QMainWindow>
#include <QMouseEvent>
#include <Qmessagebox>
#include <QFileDialog>
#include <QObject>


namespace Ui {
class AutomationTool;
}

class AutomationTool : public QMainWindow
{
    Q_OBJECT
    #define errFontColor "color:red;"
    #define nomFontColor "color:white;"

public:
    explicit AutomationTool(QWidget *parent = 0);
    ~AutomationTool();

private slots:

    void on_IDEdit_editingFinished();

    void on_RelyIDEdit_editingFinished();

    void on_OutputButton_clicked();

    void on_CreateButton_clicked();

    void on_SVNButton_clicked();

private:
    Ui::AutomationTool *ui;

    CommonBean *comBean;

    UIMethod *uiMethod;

    void init();

    void ConnectSlot();

    void initStyle();
signals:
    void JudgeIDSignal(QLineEdit *Edit,QString *objectID);

    void JudgeIDTypeSignal(QLineEdit *Edit);

    void SelectDirSignal(QLabel *label,QString *objectID);

    void SelectExampleSignal(const QString dirPath, bool flag);

    //void SelectResultFileSignal(QString *dirPath);
};

#endif // AUTOMATIONTOOL_H
