#include "Qlogger.h"
#include <QApplication>
#include "Qloghelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qlogger w;
    w.show();
    QLoghelper *log=new QLoghelper(QApplication::applicationDirPath()+"/Log4qt.properties");
    w.SetQLoghelper(log);
    return a.exec();
}
