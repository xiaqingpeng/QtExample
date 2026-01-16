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
    
    // 配置字体以支持多语言（中、日、韩）
    // 优先使用系统默认字体，但配置字体回退以支持 CJK 字符
    QStringList fontFamilies;
    
    // 检测平台并设置合适的字体
    #ifdef Q_OS_MACOS
        defaultFont.setFamily("Helvetica"); // macOS 系统字体
        fontFamilies << "Helvetica" << "PingFang SC" << "Hiragino Sans GB" << "STHeiti";
    #elif defined(Q_OS_LINUX)
        // Linux 上使用 Noto Sans CJK 或文泉驿字体
        defaultFont.setFamily("Noto Sans CJK SC"); // 优先使用 Noto Sans CJK
        fontFamilies << "Noto Sans CJK SC" << "Noto Sans CJK JP" << "Noto Sans CJK KR"
                     << "WenQuanYi Micro Hei" << "WenQuanYi Zen Hei" 
                     << "DejaVu Sans" << "Liberation Sans" << "sans-serif";
    #elif defined(Q_OS_WIN)
        defaultFont.setFamily("Microsoft YaHei"); // Windows 中文字体
        fontFamilies << "Microsoft YaHei" << "SimSun" << "Malgun Gothic" << "Meiryo";
    #else
        // 其他平台使用通用字体
        fontFamilies << "sans-serif" << "Arial" << "Helvetica";
    #endif
    
    defaultFont.setPixelSize(13);
    
    // 设置字体回退列表，确保 CJK 字符能正确显示
    defaultFont.setFamilies(fontFamilies);
    
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