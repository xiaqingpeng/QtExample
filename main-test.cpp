#include "src/Core/Application.h"
#include <QFont>

int main(int argc, char *argv[])
{
    // 使用企业级应用程序类
    Application app(argc, argv);
    
    // 设置安全的UI配置，避免emoji渲染崩溃
    QFont defaultFont = QApplication::font();
    // 使用macOS上确实存在的系统字体，避免字体警告
    defaultFont.setFamily("Helvetica"); // 使用可靠的系统字体
    defaultFont.setPixelSize(13);
    QApplication::setFont(defaultFont);
    
    // 设置应用程序属性
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
    
    // 初始化企业级应用程序
    app.initialize();
    
    // 简单测试 - 不显示UI
    // qDebug() << "Enterprise Application initialized successfully";
    
    return 0; // 立即退出，只测试初始化
}