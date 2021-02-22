#include <iostream>
#include <QCoreApplication>
#include <QStringList>
#include "downloadmanager.h"

#include <cstdio>
using namespace std;

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    /*
    QStringList arguments = app.arguments();
    arguments.takeFirst();      // remove the first argument, which is the program's name

    if (arguments.isEmpty()) {
        printf("Qt Download example\n"
               "Usage: downloadmanager url1 [url2... urlN]\n"
               "\n"
               "Downloads the URLs passed in the command-line to the local directory\n"
               "If the target file already exists, a .0, .1, .2, etc. is appended to\n"
               "differentiate.\n");
        return 0;
    }
    */
    QStringList demo;
    demo.append("https://img.ivsky.com/img/tupian/pre/202003/12/xiaoji-001.jpg");
    demo.append("https://img.ivsky.com/img/tupian/pre/202003/11/meihualu-001.jpg");
    demo.append("https://img.ivsky.com/img/tupian/pre/202003/11/meihualu-002.jpg");
    demo.append("https://img.ivsky.com/img/tupian/pre/202003/11/meihualu-003.jpg");
    DownloadManager dmanage;
   // manager.append(QUrl::fromEncoded(demo.toLocal8Bit()));
    dmanage.append(demo);
    QObject::connect(&dmanage, &DownloadManager::finished,
                     &app, &QCoreApplication::quit);
    app.exec();
}
