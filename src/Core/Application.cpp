#include "Application.h"
#include "ServiceManager.h"
#include "DependencyContainer.h"
#include "../Services/NetworkService.h"
#include "../Services/AuthenticationService.h"
#include "../Services/AnalyticsService.h"
#include "../Localization/LocalizationManager.h"
#include "../../styles/theme_manager.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

Application* Application::s_instance = nullptr;

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_serviceManager(nullptr)
    , m_container(nullptr)
    , m_initialized(false)
{
    s_instance = this;
    
    // 设置应用程序信息
    setApplicationName("Qt Enterprise App");
    setApplicationVersion("2.0.0");
    setOrganizationName("YourCompany");
    setOrganizationDomain("yourcompany.com");
    
    // 创建核心组件
    m_serviceManager = new ServiceManager(this);
    m_container = new DependencyContainer(this);
}

Application::~Application()
{
    shutdown();
    s_instance = nullptr;
}

Application* Application::instance()
{
    return s_instance;
}

void Application::initialize()
{
    if (m_initialized) {
        return;
    }

    qDebug() << "Initializing Qt Enterprise Application...";

    // 设置应用程序目录
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataDir);

    // 初始化各个子系统
    setupServices();
    setupLocalization();
    setupTheme();

    m_initialized = true;
    emit applicationInitialized();

    qDebug() << "Qt Enterprise Application initialized successfully";
}

ServiceManager* Application::serviceManager() const
{
    return m_serviceManager;
}

DependencyContainer* Application::container() const
{
    return m_container;
}

void Application::shutdown()
{
    if (!m_initialized) {
        return;
    }

    qDebug() << "Shutting down Qt Enterprise Application...";

    emit applicationShutdown();

    // 清理服务
    if (m_serviceManager) {
        m_serviceManager->shutdown();
    }

    m_initialized = false;
    qDebug() << "Qt Enterprise Application shutdown complete";
}

void Application::setupServices()
{
    qDebug() << "Setting up services...";

    // 注册网络服务
    auto networkService = new NetworkService(this);
    m_serviceManager->registerService("NetworkService", networkService);

    // 注册认证服务
    auto authService = new AuthenticationService(networkService, this);
    m_serviceManager->registerService("AuthenticationService", authService);

    // 注册分析服务
    auto analyticsService = new AnalyticsService(networkService, this);
    m_serviceManager->registerService("AnalyticsService", analyticsService);

    qDebug() << "Services setup complete";
}

void Application::setupLocalization()
{
    qDebug() << "Setting up localization...";

    // 初始化本地化管理器
    LocalizationManager::instance()->initialize();

    // 设置默认语言
    QString systemLocale = QLocale::system().name();
    LocalizationManager::instance()->setLanguage(systemLocale);

    qDebug() << "Localization setup complete, system locale:" << systemLocale;
}

void Application::setupTheme()
{
    qDebug() << "Setting up theme system...";

    // 初始化主题管理器
    ThemeManager::instance();

    qDebug() << "Theme system setup complete";
}