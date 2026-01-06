#include "src/Core/Application.h"
#include "src/Services/NetworkService.h"
#include "src/Services/AuthenticationService.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    // 使用企业级应用程序类
    Application app(argc, argv);
    
    // qDebug() << "Testing Qt Enterprise Architecture...";
    
    // 初始化企业级应用程序
    app.initialize();
    
    // 测试服务获取
    auto networkService = app.getService<NetworkService>();
    if (networkService) {
        // qDebug() << "✅ NetworkService successfully retrieved";
    } else {
        // qDebug() << "❌ Failed to retrieve NetworkService";
    }
    
    auto authService = app.getService<AuthenticationService>();
    if (authService) {
        // qDebug() << "✅ AuthenticationService successfully retrieved";
    } else {
        // qDebug() << "❌ Failed to retrieve AuthenticationService";
    }
    
    // qDebug() << "🎉 Enterprise Application test completed successfully!";
    
    return 0; // 立即退出，只测试初始化
}