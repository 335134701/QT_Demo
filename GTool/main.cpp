#include "GTool.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLogHelper::instance()->LogInfo("Tool 程序开始执行!");
    GTool gTool;
    gTool.show();

    return a.exec();
}
