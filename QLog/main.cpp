#include "Qlogger.h"
#include <QApplication>
#include "Qloghelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qlogger w;
    w.show();
    return a.exec();
}
