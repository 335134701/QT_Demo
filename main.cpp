#include "MainForm.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLogHelper::instance()->LogInfo("Tool 程序开始执行!");
    MainForm w;
    w.show();

    return a.exec();
}
