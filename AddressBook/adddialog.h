#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

//! [0]
class AddDialog : public QDialog {
    Q_OBJECT

  public:
    AddDialog(QWidget *parent = nullptr);

    QString name() const;
    QString address() const;
    void editAddress(const QString &name, const QString &address);

  private:
    QLineEdit *nameText;
    QTextEdit *addressText;
};
//! [0]

#endif // ADDDIALOG_H
