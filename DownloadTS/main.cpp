#include "downloadts.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DownloadTS w;
    w.show();

    return a.exec();
}
