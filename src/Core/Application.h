#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QHash>
#include <QString>
#include <QObject>
#include <memory>

class ServiceManager;
class DependencyContainer;

/**
 * @brief 应用程序核心类
 * 
 * 负责应用程序的初始化、服务管理和生命周期管理
 */
class Application : public QApplication
{
    Q_OBJECT

public:
    explicit Application(int &argc, char **argv);
    ~Application();

    /**
     * @brief 获取应用程序单例实例
     */
    static Application* instance();

    /**
     * @brief 初始化应用程序
     */
    void initialize();

    /**
     * @brief 获取服务管理器
     */
    ServiceManager* serviceManager() const;

    /**
     * @brief 获取依赖注入容器
     */
    DependencyContainer* container() const;

    /**
     * @brief 注册服务
     */
    template<typename T>
    void registerService(T* service);

    /**
     * @brief 获取服务
     */
    template<typename T>
    T* getService();

    /**
     * @brief 关闭应用程序
     */
    void shutdown();

signals:
    void applicationInitialized();
    void applicationShutdown();

private:
    void setupServices();
    void setupLocalization();
    void setupTheme();

private:
    static Application* s_instance;
    ServiceManager* m_serviceManager;
    DependencyContainer* m_container;
    bool m_initialized;
};

// 包含模板方法实现
#include "Application.inl"

#endif // APPLICATION_H