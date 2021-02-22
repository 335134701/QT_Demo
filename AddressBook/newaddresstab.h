#ifndef NEWADDRESSTAB_H
#define NEWADDRESSTAB_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>


//! [0]
class NewAddressTab : public QWidget {
    Q_OBJECT

  public:
    NewAddressTab(QWidget *parent = nullptr);

  public slots:
    void addEntry();

  signals:
    void sendDetails(const QString &name, const QString &address);
};
//! [0]

#endif // NEWADDRESSTAB_H
