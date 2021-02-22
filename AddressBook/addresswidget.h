#ifndef ADDRESSWIDGET_H
#define ADDRESSWIDGET_H

#include <QItemSelection>
#include <QTabWidget>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include "newaddresstab.h"
#include "tablemodel.h"


//! [0]
class AddressWidget : public QTabWidget {
    Q_OBJECT

  public:
    AddressWidget(QWidget *parent = nullptr);
    void readFromFile(const QString &fileName);
    void writeToFile(const QString &fileName);

  public slots:
    void showAddEntryDialog();
    void addEntry(const QString &name, const QString &address);
    void editEntry();
    void removeEntry();

  signals:
    void selectionChanged(const QItemSelection &selected);

  private:
    void setupTabs();

    TableModel *table;
    NewAddressTab *newAddressTab;
};
//! [0]

#endif // ADDRESSWIDGET_H
