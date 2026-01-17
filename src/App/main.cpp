#include "../UI/mainuiwindow.h"
#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include <QStyleFactory>
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

// 检测并返回最佳字体族
QString getBestFontFamily()
{
    QFontDatabase fontDb;
    QStringList availableFamilies = fontDb.families();
    
#ifdef Q_OS_MACOS
    // macOS 优先使用 PingFang SC（更现代的中文字体）
    if (availableFamilies.contains("PingFang SC")) {
        return "PingFang SC";
    }
    if (availableFamilies.contains("Helvetica Neue")) {
        return "Helvetica Neue";
    }
    return "Helvetica";
#elif defined(Q_OS_LINUX)
    // Linux 按优先级检测字体
    QStringList preferredFonts = {
        "Noto Sans CJK SC",
        "Source Han Sans SC",
        "WenQuanYi Micro Hei",
        "WenQuanYi Zen Hei",
        "Noto Sans",
        "DejaVu Sans",
        "Liberation Sans"
    };
    for (const QString &font : preferredFonts) {
        if (availableFamilies.contains(font)) {
            return font;
        }
    }
    return "sans-serif";
#elif defined(Q_OS_WIN)
    // Windows 优先使用更现代的 UI 字体
    if (availableFamilies.contains("Microsoft YaHei UI")) {
        return "Microsoft YaHei UI";  // 比 Microsoft YaHei 更现代
    }
    if (availableFamilies.contains("Microsoft YaHei")) {
        return "Microsoft YaHei";
    }
    if (availableFamilies.contains("Segoe UI")) {
        return "Segoe UI";
    }
    return "Arial";
#else
    return "sans-serif";
#endif
}

// 配置平台原生样式
void setupPlatformStyle()
{
#ifdef Q_OS_MACOS
    // macOS 使用原生样式
    QStringList availableStyles = QStyleFactory::keys();
    if (availableStyles.contains("macintosh")) {
        QApplication::setStyle(QStyleFactory::create("macintosh"));
        // qDebug() << "Using macOS native style";
    }
#elif defined(Q_OS_WIN)
    // Windows 使用 Windows 11 样式（如果可用）
    QStringList availableStyles = QStyleFactory::keys();
    if (availableStyles.contains("windows11")) {
        QApplication::setStyle(QStyleFactory::create("windows11"));
        // qDebug() << "Using Windows 11 style";
    } else if (availableStyles.contains("windowsvista")) {
        QApplication::setStyle(QStyleFactory::create("windowsvista"));
        // qDebug() << "Using Windows Vista style";
    }
#elif defined(Q_OS_LINUX)
    // Linux 使用 GTK 样式（如果可用）
    QStringList availableStyles = QStyleFactory::keys();
    if (availableStyles.contains("gtk3")) {
        QApplication::setStyle(QStyleFactory::create("gtk3"));
        // qDebug() << "Using GTK3 style";
    } else if (availableStyles.contains("gtk2")) {
        QApplication::setStyle(QStyleFactory::create("gtk2"));
        // qDebug() << "Using GTK2 style";
    }
#endif
}

int main(int argc, char *argv[])
{
    // 使用企业级应用程序类作为标准
    Application app(argc, argv);
    
    // qDebug() << "Using Qt Enterprise Architecture";
    
    // ========== 跨平台 UI 优化配置 ==========
    
    // 1. 启用高 DPI 支持（必须在创建 QApplication 后立即设置）
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    QApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    
    // 2. 设置平台原生样式（必须在设置字体之前）
    setupPlatformStyle();
    
    // 初始化企业级应用程序
    app.initialize();
    
    // 初始化本地化管理器
    LocalizationManager::instance()->initialize();
    
    // 初始化企业级服务
    initializeEnterpriseServices(&app);
    
    // 3. 配置优化的字体设置
    QFont defaultFont = QApplication::font();
    
    // 检测并设置最佳字体族
    QString bestFontFamily = getBestFontFamily();
    defaultFont.setFamily(bestFontFamily);
    
    // 配置字体回退列表以支持多语言（中、日、韩）
    QStringList fontFamilies;
    
#ifdef Q_OS_MACOS
    fontFamilies << bestFontFamily 
                 << "PingFang SC" << "PingFang TC" << "PingFang HK"
                 << "Hiragino Sans GB" << "STHeiti" << "Helvetica Neue" << "Helvetica";
#elif defined(Q_OS_LINUX)
    fontFamilies << bestFontFamily
                 << "Noto Sans CJK SC" << "Noto Sans CJK JP" << "Noto Sans CJK KR"
                 << "Source Han Sans SC" << "Source Han Sans JP" << "Source Han Sans KR"
                 << "WenQuanYi Micro Hei" << "WenQuanYi Zen Hei"
                 << "Noto Sans" << "DejaVu Sans" << "Liberation Sans" << "sans-serif";
#elif defined(Q_OS_WIN)
    fontFamilies << bestFontFamily
                 << "Microsoft YaHei UI" << "Microsoft YaHei"
                 << "SimSun" << "SimHei"
                 << "Malgun Gothic" << "Meiryo"
                 << "Segoe UI" << "Arial";
#else
    fontFamilies << bestFontFamily << "sans-serif" << "Arial" << "Helvetica";
#endif
    
    // 使用点大小而不是像素大小（DPI 自适应）
    // macOS: 10pt, Linux: 10pt, Windows: 9pt（Windows 字体通常稍大）
#ifdef Q_OS_WIN
    defaultFont.setPointSize(9);
#else
    defaultFont.setPointSize(10);
#endif
    
    // 启用字体平滑和优化渲染
    defaultFont.setStyleStrategy(QFont::PreferAntialias);
    defaultFont.setHintingPreference(QFont::PreferFullHinting);
    
    // 设置字体回退列表，确保 CJK 字符能正确显示
    defaultFont.setFamilies(fontFamilies);
    
    QApplication::setFont(defaultFont);
    
    // 4. 设置应用程序属性
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
    
    // 5. 平台特定的字体替换优化
#ifdef Q_OS_LINUX
    // Linux 上为常见字体设置替换
    QFontDatabase fontDb;
    if (fontDb.families().contains("sans-serif")) {
        QFont::insertSubstitution("sans-serif", bestFontFamily);
    }
#endif
    
#ifdef Q_OS_WIN
    // Windows 上优化中文字体替换
    if (bestFontFamily.contains("YaHei")) {
        QFont::insertSubstitution("Microsoft YaHei", "Microsoft YaHei UI");
    }
#endif
    
    // qDebug() << "Platform style:" << QApplication::style()->objectName();
    // qDebug() << "Font family:" << defaultFont.family();
    // qDebug() << "Font point size:" << defaultFont.pointSize();
    
    MainUIWindow w;
    w.show();
    
    int result = app.exec();
    
    // 清理Analytics SDK单例
    Analytics::SDK::cleanup();
    
    return result;
}