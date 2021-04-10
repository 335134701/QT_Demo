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
    #define nomFontColor "color:white;"

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

    void JudgeIDTypeSlot(const QString ID);

private:
    Ui::AutomationTool *ui;

    const QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    CommonBean *comBean;

    void init();

    void ConnectSlot();

    void initStyle();
signals:
    void JudgeIDTypeSignal(QString ID);
};

#endif // AUTOMATIONTOOL_H
