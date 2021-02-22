#include "textfind.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TextFind w;
    w.show();

    return a.exec();
}
