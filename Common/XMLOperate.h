#ifndef XMLOPERATE_H
#define XMLOPERATE_H

#include <QObject>
#include <QXmlStreamReader>
#include <QFile>
#include "QLoghelper.h"

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

    void ReadXML();

    QMap<QString, ERRCODETYPE> getErrCodeType() const;

private:
    const QString xmlPath=":/ERRCode.xml";
    QMap<QString,ERRCODETYPE> errCodeType;
};

#endif // XMLOPERATE_H
