#include "Qlogger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qlogger w;
    w.show();

    return a.exec();
}
