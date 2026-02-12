#include "NetworkManagerAdapter.h"
#include "../Services/NetworkService.h"
#include <QDebug>

NetworkManagerAdapter::NetworkManagerAdapter(NetworkService *networkService, QObject *parent)
    : QObject(parent)
    , m_networkService(networkService)
{
}

NetworkManagerAdapter::~NetworkManagerAdapter()
{
}

void NetworkManagerAdapter::get(const QString &url,
                                const SuccessCallback &successCallback,
                                const ErrorCallback &errorCallback,
                                const QUrlQuery &queryParams)
{
    m_networkService->getCallback(url, successCallback, errorCallback, queryParams);
}

void NetworkManagerAdapter::post(const QString &url,
                                 const QJsonObject &data,
                                 const SuccessCallback &successCallback,
                                 const ErrorCallback &errorCallback)
{
    qDebug() << "[NetworkManagerAdapter] POST request to:" << url;
    m_networkService->postCallback(url, data, successCallback, errorCallback);
}

void NetworkManagerAdapter::put(const QString &url,
                                const QJsonObject &data,
                                const SuccessCallback &successCallback,
                                const ErrorCallback &errorCallback)
{
    m_networkService->putCallback(url, data, successCallback, errorCallback);
}

void NetworkManagerAdapter::deleteResource(const QString &url,
                                           const SuccessCallback &successCallback,
                                           const ErrorCallback &errorCallback)
{
    m_networkService->deleteCallback(url, successCallback, errorCallback);
}

void NetworkManagerAdapter::uploadFile(const QString &url,
                                       const QString &filePath,
                                       const QString &fileFieldName,
                                       const SuccessCallback &successCallback,
                                       const ErrorCallback &errorCallback)
{
    m_networkService->uploadFileCallback(url, filePath, fileFieldName, successCallback, errorCallback);
}

QString NetworkManagerAdapter::getPlatform()
{
    return NetworkService::getPlatform();
}

void NetworkManagerAdapter::setBaseUrl(const QString &baseUrl)
{
    m_networkService->setBaseUrl(baseUrl);
}