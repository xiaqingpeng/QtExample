#include "DependencyContainer.h"
#include <QMutexLocker>
#include <QDebug>

DependencyContainer::DependencyContainer(QObject *parent)
    : QObject(parent)
{
}

DependencyContainer::~DependencyContainer()
{
    clear();
}

QString DependencyContainer::getTypeName(const std::type_info& typeInfo) const
{
    return QString::fromLatin1(typeInfo.name());
}

void DependencyContainer::clear()
{
    QMutexLocker locker(&m_mutex);
    
    // qDebug() << "Clearing DependencyContainer, services count:" << m_services.size();
    
    // 清理所有单例实例
    for (auto it = m_services.begin(); it != m_services.end(); ++it) {
        ServiceDescriptor& descriptor = it.value();
        if (descriptor.instance) {
            descriptor.instance.reset();
        }
    }
    
    m_services.clear();
    // qDebug() << "DependencyContainer cleared";
}