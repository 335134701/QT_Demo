#ifndef XMLOPERATE_H
#define XMLOPERATE_H

#include <QObject>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include "QLoghelper.h"

typedef struct ERRCodeType{
    QString ID;
    QString Def;
    QString ErrDef;
    QString Level;
    QString Code;

}ERRCODETYPE;

typedef struct ERRCodeType{
    QString ID;
    QString Def;
    QString ErrDef;
    QString Level;
    QString Code;

}ERRCODETYPE;

class XMLOperate : public QObject
{
    Q_OBJECT
public:
    explicit XMLOperate(QObject *parent = nullptr);

    QMap<QString,ERRCODETYPE> ReadXML(const QString filePath);

    QMap<QString, ERRCODETYPE> getErrCodeType() const;

private:
    const QString xmlPath=":/ERRCode.xml";
    QMap<QString,ERRCODETYPE> errCodeType;
};

#endif // XMLOPERATE_H
