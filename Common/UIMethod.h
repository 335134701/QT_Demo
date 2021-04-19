#ifndef UIMETHOD_H
#define UIMETHOD_H

#include <QObject>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QTextEdit>
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
signals:

public slots:
    void JudgeIDSlot(QLineEdit *Edit,QString *objectID);

    void JudgeIDTypeSlot(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowIDmessageSlot(QTextEdit *Edit,int flag);

    void SelectDirSlot(QLabel *label,QString *objectID,const QString errName);

    void SelectFileSlot(const QString dirPath);

private :
    QString JudgeIDType(const QString ID);

    CommonBean *comBean;

};

#endif // UIMETHOD_H
