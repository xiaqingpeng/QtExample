#include "mainuiwindow.h"
#include <QApplication>
#include "analytics/analytics.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainUIWindow w;
    w.show();
    
    int result = a.exec();
    
    // 清理Analytics SDK单例
    Analytics::SDK::cleanup();
    
    return result;
}