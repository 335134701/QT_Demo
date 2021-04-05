#ifndef XMLOPERATE_H
#define XMLOPERATE_H

#include <QObject>
#include <QtXml>
#include <QDomDocument>


class XMLOperate : public QObject
{
    Q_OBJECT
public:
    explicit XMLOperate(QObject *parent = nullptr);

    bool OpenXML(const QString filePath);

    QDomNode* listDom(QDomElement *docElem);

signals:

public slots:

private :

};

#endif // XMLOPERATE_H
