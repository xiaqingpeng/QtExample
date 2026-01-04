#ifndef APPLICATION_INL
#define APPLICATION_INL

#include "ServiceManager.h"

// 模板方法实现
template<typename T>
void Application::registerService(T* service)
{
    if (m_serviceManager) {
        m_serviceManager->registerService(T::staticMetaObject.className(), service);
    }
}

template<typename T>
T* Application::getService()
{
    if (m_serviceManager) {
        return qobject_cast<T*>(m_serviceManager->getService(T::staticMetaObject.className()));
    }
    return nullptr;
}

#endif // APPLICATION_INL