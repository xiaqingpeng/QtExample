#include "../UI/mainuiwindow.h"
#include <QApplication>
#include <QFont>
#include "../Analytics/analytics.h"
#include "../Core/Application.h"
#include "../Services/NetworkService.h"
#include "../Services/AuthenticationService.h"
#include "../Services/AnalyticsService.h"
#include "../Localization/LocalizationManager.h"

void initializeEnterpriseServices(Application* app)
{
    // 获取服务管理器
    ServiceManager* serviceManager = app->serviceManager();
    if (!serviceManager) {
        qWarning() << "Service Manager not available!";
        return;
    }

    // 注册网络服务
    auto networkService = new NetworkService(app);
    serviceManager->registerService("NetworkService", networkService);

    // 注册认证服务
    auto authService = new AuthenticationService(networkService, app);
    serviceManager->registerService("AuthenticationService", authService);

    // 注册分析服务
    auto analyticsService = new AnalyticsService(networkService, app);
    serviceManager->registerService("AnalyticsService", analyticsService);

    // qDebug() << "Enterprise services initialized successfully";
}

int main(int argc, char *argv[])
{
    // 使用企业级应用程序类作为标准
    Application app(argc, argv);
    
    // qDebug() << "Using Qt Enterprise Architecture";
    
    // 初始化企业级应用程序
    app.initialize();
    
    // 初始化本地化管理器
    LocalizationManager::instance()->initialize();
    
    // 初始化企业级服务
    initializeEnterpriseServices(&app);
    
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