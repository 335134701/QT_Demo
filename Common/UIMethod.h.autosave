#ifndef UIMETHOD_H
#define UIMETHOD_H

#include <QObject>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <Qmessagebox>
#include <QFileDialog>
#include "QLoghelper.h"
#include "CommonBean.h"


class UIMethod : public QObject
{
    Q_OBJECT

    #define errFontColor "color:red;"
    #define nomFontColor "color:white;"
public:
    explicit UIMethod(QObject *parent = nullptr);

    void setComBean(CommonBean *value);

    void ErrorCodeDeal(const QString objectName,bool flag);
signals:

public slots:
    void JudgeIDSlot(QLineEdit *Edit,QString *objectID);

    void JudgeIDTypeSlot(QLineEdit *Edit);

    void SelectDirSlot(QLabel *label,QString *objectID);

    void SelectExampleSlot(const QString dirPath, QString condition);

    void SelectResultFileSlot(const QString dirPath);

private :
    QString JudgeIDType(const QString ID);

    CommonBean *comBean;

};

#endif // UIMETHOD_H
