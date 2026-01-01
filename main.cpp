#include "mainuiwindow.h"
#include <QApplication>
#include <QFont>
#include "analytics/analytics.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置安全的UI配置，避免emoji渲染崩溃
    QFont defaultFont = QApplication::font();
    // 使用macOS上确实存在的系统字体
    defaultFont.setFamily(".AppleSystemUIFont"); // macOS系统UI字体
    defaultFont.setPixelSize(13);
    QApplication::setFont(defaultFont);
    
    // 设置应用程序属性
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
    
    MainUIWindow w;
    w.show();
    
    int result = a.exec();
    
    // 清理Analytics SDK单例
    Analytics::SDK::cleanup();
    
    return result;
}