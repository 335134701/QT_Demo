#ifndef SIFORM_H
#define SIFORM_H

#include <QWidget>

namespace Ui {
class SIForm;
}

class SIForm : public QWidget
{
    Q_OBJECT

public:
    explicit SIForm(QWidget *parent = 0);
    ~SIForm();

private:
    Ui::SIForm *ui;
};

#endif // SIFORM_H
