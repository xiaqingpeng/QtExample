#include "ServiceManager.h"
#include <QMutexLocker>
#include <QDebug>

ServiceManager::ServiceManager(QObject *parent)
    : QObject(parent)
{
}

ServiceManager::~ServiceManager()
{
    shutdown();
}

void ServiceManager::registerService(const QString& name, QObject* service)
{
    if (!service) {
        qWarning() << "Attempting to register null service:" << name;
        return;
    }

    QMutexLocker locker(&m_mutex);
    
    if (m_services.contains(name)) {
        qWarning() << "Service already registered:" << name;
        return;
    }

    m_services.insert(name, service);
    
    // 设置服务的父对象为ServiceManager，确保生命周期管理
    if (service->parent() != this) {
        service->setParent(this);
    }

    // qDebug() << "Service registered:" << name;
    emit serviceRegistered(name, service);
}

QObject* ServiceManager::getService(const QString& name) const
{
    QMutexLocker locker(&m_mutex);
    return m_services.value(name, nullptr);
}

bool ServiceManager::hasService(const QString& name) const
{
    QMutexLocker locker(&m_mutex);
    return m_services.contains(name);
}

void ServiceManager::removeService(const QString& name)
{
    QMutexLocker locker(&m_mutex);
    
    if (!m_services.contains(name)) {
        qWarning() << "Service not found for removal:" << name;
        return;
    }

    QObject* service = m_services.take(name);
    if (service) {
        service->deleteLater();
    }

    // qDebug() << "Service removed:" << name;
    emit serviceRemoved(name);
}

QStringList ServiceManager::serviceNames() const
{
    QMutexLocker locker(&m_mutex);
    return m_services.keys();
}

void ServiceManager::shutdown()
{
    QMutexLocker locker(&m_mutex);
    
    // qDebug() << "Shutting down ServiceManager, services count:" << m_services.size();
    
    // 清理所有服务
    for (auto it = m_services.begin(); it != m_services.end(); ++it) {
        const QString& name = it.key();
        QObject* service = it.value();
        
        // qDebug() << "Shutting down service:" << name;
        
        if (service) {
            service->deleteLater();
        }
        
        emit serviceRemoved(name);
    }
    
    m_services.clear();
    // qDebug() << "ServiceManager shutdown complete";
}