#ifndef DEPENDENCYCONTAINER_H
#define DEPENDENCYCONTAINER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QMutex>
#include <functional>
#include <memory>
#include <typeinfo>

/**
 * @brief 依赖注入容器
 * 
 * 提供依赖注入功能，支持单例和瞬态对象的注册和解析
 */
class DependencyContainer : public QObject
{
    Q_OBJECT

public:
    explicit DependencyContainer(QObject *parent = nullptr);
    ~DependencyContainer();

    /**
     * @brief 注册单例服务
     * @tparam Interface 接口类型
     * @tparam Implementation 实现类型
     */
    template<typename Interface, typename Implementation>
    void registerSingleton();

    /**
     * @brief 注册瞬态服务
     * @tparam Interface 接口类型
     * @tparam Implementation 实现类型
     */
    template<typename Interface, typename Implementation>
    void registerTransient();

    /**
     * @brief 注册单例实例
     * @tparam T 类型
     * @param instance 实例
     */
    template<typename T>
    void registerInstance(std::shared_ptr<T> instance);

    /**
     * @brief 解析服务
     * @tparam T 服务类型
     * @return 服务实例
     */
    template<typename T>
    std::shared_ptr<T> resolve();

    /**
     * @brief 检查服务是否已注册
     * @tparam T 服务类型
     * @return 如果已注册返回true
     */
    template<typename T>
    bool isRegistered() const;

    /**
     * @brief 清除所有注册
     */
    void clear();

private:
    enum class ServiceLifetime {
        Singleton,
        Transient
    };

    struct ServiceDescriptor {
        ServiceLifetime lifetime;
        std::function<std::shared_ptr<QObject>()> factory;
        std::shared_ptr<QObject> instance; // 用于单例
    };

    QString getTypeName(const std::type_info& typeInfo) const;

private:
    QHash<QString, ServiceDescriptor> m_services;
    mutable QMutex m_mutex;
};

// 模板方法实现
template<typename Interface, typename Implementation>
void DependencyContainer::registerSingleton()
{
    QMutexLocker locker(&m_mutex);
    
    QString typeName = getTypeName(typeid(Interface));
    
    ServiceDescriptor descriptor;
    descriptor.lifetime = ServiceLifetime::Singleton;
    descriptor.factory = []() -> std::shared_ptr<QObject> {
        return std::make_shared<Implementation>();
    };
    
    m_services.insert(typeName, descriptor);
}

template<typename Interface, typename Implementation>
void DependencyContainer::registerTransient()
{
    QMutexLocker locker(&m_mutex);
    
    QString typeName = getTypeName(typeid(Interface));
    
    ServiceDescriptor descriptor;
    descriptor.lifetime = ServiceLifetime::Transient;
    descriptor.factory = []() -> std::shared_ptr<QObject> {
        return std::make_shared<Implementation>();
    };
    
    m_services.insert(typeName, descriptor);
}

template<typename T>
void DependencyContainer::registerInstance(std::shared_ptr<T> instance)
{
    QMutexLocker locker(&m_mutex);
    
    QString typeName = getTypeName(typeid(T));
    
    ServiceDescriptor descriptor;
    descriptor.lifetime = ServiceLifetime::Singleton;
    descriptor.instance = std::static_pointer_cast<QObject>(instance);
    
    m_services.insert(typeName, descriptor);
}

template<typename T>
std::shared_ptr<T> DependencyContainer::resolve()
{
    QMutexLocker locker(&m_mutex);
    
    QString typeName = getTypeName(typeid(T));
    
    if (!m_services.contains(typeName)) {
        return nullptr;
    }
    
    ServiceDescriptor& descriptor = m_services[typeName];
    
    if (descriptor.lifetime == ServiceLifetime::Singleton) {
        if (!descriptor.instance && descriptor.factory) {
            descriptor.instance = descriptor.factory();
        }
        return std::static_pointer_cast<T>(descriptor.instance);
    } else {
        // Transient
        if (descriptor.factory) {
            return std::static_pointer_cast<T>(descriptor.factory());
        }
    }
    
    return nullptr;
}

template<typename T>
bool DependencyContainer::isRegistered() const
{
    QMutexLocker locker(&m_mutex);
    QString typeName = getTypeName(typeid(T));
    return m_services.contains(typeName);
}

#endif // DEPENDENCYCONTAINER_H