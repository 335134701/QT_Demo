#include <QApplication>
#include "AutomationTool.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLogHelper::instance()->LogInfo("AutomationTool程序开始执行!");
    AutomationTool w;
    w.show();

    return a.exec();
}
