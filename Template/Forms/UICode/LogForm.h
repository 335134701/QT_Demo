#ifndef LOGFORM_H
#define LOGFORM_H

#include <QWidget>

#include "COMMONDEFINE.h"
#include "LogFormBean.h"
#include "LogFormMetod.h"

namespace Ui {
class LogForm;
}

class LogForm : public QWidget
{
    Q_OBJECT

public:
    explicit LogForm(QWidget *parent = 0);
    ~LogForm();

    LogFormMetod *getLogFormMethod() const;
    void setLogFormMethod(LogFormMetod *value);

    LogFormBean *getLogFormBean() const;
    void setLogFormBean(LogFormBean *value);

public slots:
    void LogViewClearSlot();

private:
    Ui::LogForm *ui;

    QAction *logViewClearAction;

    LogFormBean *logFormBean;

    LogFormMetod *logFormMethod;

    void InitStytle();

    void Init();

    void ConnectSlot();

};

#endif // LOGFORM_H
