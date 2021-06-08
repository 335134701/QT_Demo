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

    QStringList *getTextList() const;
    void setTextList(QStringList *value);

    int getLastIndex() const;
    void setLastIndex(int value);

signals:

public slots:

private :

    QTextEdit *logView;

    QStringList *textList;

    int lastIndex;

    void Init();
};

#endif // LOGFORMBEAN_H
