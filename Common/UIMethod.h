#ifndef UIMETHOD_H
#define UIMETHOD_H

#include <QObject>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QTextEdit>
#include <QThread>
#include <Qmessagebox>
#include <QFileDialog>
#include "QLoghelper.h"
#include "CommonBean.h"
#include "MyThread.h"
#include "CommonMethod.h"


class UIMethod : public QObject
{
    Q_OBJECT

    #define errFontColor "color:red;"
    #define nomFontColor "color:white;"
public:
    explicit UIMethod(QObject *parent = nullptr);

    void setComBean(CommonBean *value);
signals:
    void FindFileThreadSignal(const QString dirPath,CommonMethod *commonMethod,const QStringList filters,unsigned int flag);

    void ActiveThreadSignal(QStringList st,QString dirPath,unsigned int flag);

public slots:
    void JudgeIDSlot(QLineEdit *Edit,QString *objectID);

    void JudgeIDTypeSlot(QLineEdit *Edit,QString *srcobject,QString *desobject);

    void ShowIDmessageSlot(QTextEdit *Edit,int flag);

    void SelectDirSlot(QLabel *label,QString *objectID,const QString errName);

    void SelectFileSlot(const QString dirPath);

    void EndFindFileThreadSlot(QStringList st,QString dirPath,unsigned int flag);

private :
    void Init();

    QString JudgeIDType(const QString ID);

    CommonBean *comBean;

    MyThread *mythread;

    QThread *dealFileFileThread;

    void StartThread(QThread *thread,const QString dirPath,CommonMethod *commonMethod,const QStringList filters,unsigned int flag);

    void EndThread(QThread *thread);
};

#endif // UIMETHOD_H
