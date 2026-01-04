#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QMutex>

/**
 * @brief 服务管理器
 * 
 * 负责管理应用程序中的所有服务，提供服务注册、获取和生命周期管理
 */
class ServiceManager : public QObject
{
    Q_OBJECT

public:
    explicit ServiceManager(QObject *parent = nullptr);
    ~ServiceManager();

    /**
     * @brief 注册服务
     * @param name 服务名称
     * @param service 服务实例
     */
    void registerService(const QString& name, QObject* service);

    /**
     * @brief 获取服务
     * @param name 服务名称
     * @return 服务实例，如果不存在返回nullptr
     */
    QObject* getService(const QString& name) const;

    /**
     * @brief 检查服务是否存在
     * @param name 服务名称
     * @return 如果服务存在返回true
     */
    bool hasService(const QString& name) const;

    /**
     * @brief 移除服务
     * @param name 服务名称
     */
    void removeService(const QString& name);

    /**
     * @brief 获取所有服务名称
     * @return 服务名称列表
     */
    QStringList serviceNames() const;

    /**
     * @brief 关闭所有服务
     */
    void shutdown();

signals:
    void serviceRegistered(const QString& name, QObject* service);
    void serviceRemoved(const QString& name);

private:
    QHash<QString, QObject*> m_services;
    mutable QMutex m_mutex;
};

#endif // SERVICEMANAGER_H