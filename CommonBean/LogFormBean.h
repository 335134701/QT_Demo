#ifndef LOGFORMBEAN_H
#define LOGFORMBEAN_H

#include <QObject>
#include <QTextEdit>

#include "COMMONDEFINE.h"

class LogFormBean : public QObject
{
    Q_OBJECT
public:
    explicit LogFormBean(QObject *parent = nullptr);

    QTextEdit *getLogView() const;
    void setLogView(QTextEdit *value);

signals:

public slots:

private :

    QTextEdit *logView;

    void Init();
};

#endif // LOGFORMBEAN_H
