#ifndef LOGFORM_H
#define LOGFORM_H

#include <QWidget>

namespace Ui {
class LogForm;
}

class LogForm : public QWidget
{
    Q_OBJECT

public:
    explicit LogForm(QWidget *parent = 0);
    ~LogForm();

private:
    Ui::LogForm *ui;
};

#endif // LOGFORM_H
