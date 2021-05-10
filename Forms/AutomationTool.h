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

    void LogViewClearSlot();

    void on_CheckButton_clicked();

    void on_MessageView_doubleClicked(const QModelIndex &index);

private:
    Ui::AutomationTool *ui;

    CommonBean *comBean;

    UIMethod *uiMethod;

    QAction *logViewClearAction;

    void Init();

    void ConnectSlot();

    void InitTableView();

    void InitStyle();

    bool MessageWarn();
signals:
    void JudgeIDSignal(QLineEdit *Edit,QString *objectID);

    void JudgeIDTypeSignal(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowIDmessageSignal(int flag);

    void SelectDirSignal(QLabel *label,QString *objectID);

    void SelectFileSignal(QString dirPath,unsigned int flag, bool goOn);

    void MessageViewModelEditedSignal(const QStandardItem *item);

    void CreateSignal();

    void CheckSignal();
};

#endif // AUTOMATIONTOOL_H
