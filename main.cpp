#include "mainuiwindow.h"
#include <QApplication>
#include <QFont>
#include "analytics/analytics.h"

// 尝试使用企业级架构，如果不可用则回退到标准QApplication
#ifdef ENTERPRISE_EDITION
#include "src/Core/Application.h"
#endif

int main(int argc, char *argv[])
{
#ifdef ENTERPRISE_EDITION
    // 使用企业级应用程序类
    Application app(argc, argv);
    
    qDebug() << "Using Enterprise Edition";
    
    // 初始化企业级应用程序
    app.initialize();
#else
    // 使用标准QApplication
    QApplication app(argc, argv);
    
    qDebug() << "Using Standard Edition";
#endif
    
    // 设置安全的UI配置，避免emoji渲染崩溃
    QFont defaultFont = QApplication::font();
    // 使用macOS上确实存在的系统字体，避免字体警告
    defaultFont.setFamily("Helvetica"); // 使用可靠的系统字体
    defaultFont.setPixelSize(13);
    QApplication::setFont(defaultFont);
    
    // 设置应用程序属性
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
    
    MainUIWindow w;
    w.show();
    
    int result = app.exec();
    
    // 清理Analytics SDK单例
    Analytics::SDK::cleanup();
    
    return result;
}