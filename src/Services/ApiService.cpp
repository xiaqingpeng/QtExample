#include "ApiService.h"
#include "NetworkManagerAdapter.h"
#include "NetworkService.h"
#include <QDate>
#include <QPromise>

ApiService::ApiService(NetworkManagerAdapter *networkManager, QObject *parent)
    : QObject(parent)
    , m_networkManagerAdapter(networkManager)
    , m_ownsNetworkManager(false)
    , m_networkService(nullptr)
{
}

ApiService::ApiService(QObject *parent)
    : QObject(parent)
    , m_networkManagerAdapter(nullptr)
    , m_ownsNetworkManager(true)
    , m_networkService(nullptr)
{
    m_networkService = new NetworkService(this);
    m_networkManagerAdapter = new NetworkManagerAdapter(m_networkService, this);
}

ApiService::~ApiService()
{
}

void ApiService::setBaseUrl(const QString &baseUrl)
{
    if (m_networkManagerAdapter) {
        m_networkManagerAdapter->setBaseUrl(baseUrl);
    }
}

QFuture<QJsonObject> ApiService::get(const QString& url, const QUrlQuery& params)
{
    if (m_networkService) {
        return m_networkService->get(url, params);
    }
    auto promise = std::make_shared<QPromise<QJsonObject>>();
    promise->start();
    promise->addResult(QJsonObject{{"error", "Network service not available"}});
    promise->finish();
    return promise->future();
}

QFuture<QJsonObject> ApiService::post(const QString& url, const QJsonObject& data)
{
    if (m_networkService) {
        return m_networkService->post(url, data);
    }
    auto promise = std::make_shared<QPromise<QJsonObject>>();
    promise->start();
    promise->addResult(QJsonObject{{"error", "Network service not available"}});
    promise->finish();
    return promise->future();
}

QFuture<QJsonObject> ApiService::put(const QString& url, const QJsonObject& data)
{
    if (m_networkService) {
        return m_networkService->put(url, data);
    }
    auto promise = std::make_shared<QPromise<QJsonObject>>();
    promise->start();
    promise->addResult(QJsonObject{{"error", "Network service not available"}});
    promise->finish();
    return promise->future();
}

QFuture<QJsonObject> ApiService::deleteResource(const QString& url)
{
    if (m_networkService) {
        return m_networkService->deleteResource(url);
    }
    auto promise = std::make_shared<QPromise<QJsonObject>>();
    promise->start();
    promise->addResult(QJsonObject{{"error", "Network service not available"}});
    promise->finish();
    return promise->future();
}

QFuture<QJsonObject> ApiService::uploadFile(const QString& url, const QString& filePath, const QString& fieldName)
{
    if (m_networkService) {
        return m_networkService->uploadFile(url, filePath, fieldName);
    }
    auto promise = std::make_shared<QPromise<QJsonObject>>();
    promise->start();
    promise->addResult(QJsonObject{{"error", "Network service not available"}});
    promise->finish();
    return promise->future();
}

void ApiService::setDefaultHeaders(const QHash<QString, QString>& headers)
{
    if (m_networkService) {
        m_networkService->setDefaultHeaders(headers);
    }
}

void ApiService::setTimeout(int timeoutMs)
{
    if (m_networkService) {
        m_networkService->setTimeout(timeoutMs);
    }
}

void ApiService::setRetryPolicy(int maxRetries, int delayMs)
{
    if (m_networkService) {
        m_networkService->setRetryPolicy(maxRetries, delayMs);
    }
}

void ApiService::get(const QString &url,
                     const SuccessCallback &successCallback,
                     const ErrorCallback &errorCallback,
                     const QUrlQuery &queryParams)
{
    m_networkManagerAdapter->get(url, successCallback, errorCallback, queryParams);
}

void ApiService::post(const QString &url,
                      const QJsonObject &data,
                      const SuccessCallback &successCallback,
                      const ErrorCallback &errorCallback)
{
    m_networkManagerAdapter->post(url, data, successCallback, errorCallback);
}

void ApiService::put(const QString &url,
                     const QJsonObject &data,
                     const SuccessCallback &successCallback,
                     const ErrorCallback &errorCallback)
{
    m_networkManagerAdapter->put(url, data, successCallback, errorCallback);
}

void ApiService::deleteResource(const QString &url,
                                const SuccessCallback &successCallback,
                                const ErrorCallback &errorCallback)
{
    m_networkManagerAdapter->deleteResource(url, successCallback, errorCallback);
}

void ApiService::uploadFile(const QString &url,
                             const QString &filePath,
                             const QString &fileFieldName,
                             const SuccessCallback &successCallback,
                             const ErrorCallback &errorCallback)
{
    m_networkManagerAdapter->uploadFile(url, filePath, fileFieldName, successCallback, errorCallback);
}

void ApiService::getUserProfile(const QString &userId,
                                const SuccessCallback &successCallback,
                                const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    get("/api/analytics/user/profile", successCallback, errorCallback, queryParams);
}

void ApiService::getUserTags(const QString &userId,
                           const SuccessCallback &successCallback,
                           const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    get("/api/analytics/user/tags", successCallback, errorCallback, queryParams);
}

void ApiService::getUserBehaviorStats(const QString &userId,
                                      const SuccessCallback &successCallback,
                                      const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    get("/api/analytics/user/behavior", successCallback, errorCallback, queryParams);
}

void ApiService::getUserInterestProfile(const QString &userId,
                                       const SuccessCallback &successCallback,
                                       const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    get("/api/analytics/user/interest", successCallback, errorCallback, queryParams);
}

void ApiService::getUserValueAssessment(const QString &userId,
                                       const SuccessCallback &successCallback,
                                       const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    get("/api/analytics/user/value", successCallback, errorCallback, queryParams);
}

void ApiService::updateUserProfile(const QString &userId,
                                  const QJsonObject &data,
                                  const SuccessCallback &successCallback,
                                  const ErrorCallback &errorCallback)
{
    QString url = "/api/user-profile/update/" + userId;
    post(url, data, successCallback, errorCallback);
}

void ApiService::getActivityStats(const QString &startDate, const QString &endDate,
                                 const SuccessCallback &successCallback,
                                 const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    
    get("/api/analytics/activity", successCallback, errorCallback, queryParams);
}

void ApiService::getRetentionStats(const SuccessCallback &successCallback,
                                  const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("days", "7");
    
    get("/api/analytics/retention", successCallback, errorCallback, queryParams);
}

void ApiService::getPageViewStats(const QString &startDate, const QString &endDate,
                                 const SuccessCallback &successCallback,
                                 const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    
    get("/api/analytics/page-views", successCallback, errorCallback, queryParams);
}

void ApiService::getEventStats(const QString &startDate, const QString &endDate,
                              const SuccessCallback &successCallback,
                              const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    
    get("/api/analytics/event-stats", successCallback, errorCallback, queryParams);
}

void ApiService::getTrendAnalysis(const QString &metric, const QString &startDate, const QString &endDate,
                                 const SuccessCallback &successCallback,
                                 const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    queryParams.addQueryItem("metric", metric);
    queryParams.addQueryItem("interval", "day");
    
    get("/api/analytics/trends", successCallback, errorCallback, queryParams);
}

void ApiService::getTopPages(const QString &startDate, const QString &endDate, int limit,
                             const SuccessCallback &successCallback,
                             const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    queryParams.addQueryItem("limit", QString::number(limit));
    
    get("/api/analytics/page-views", successCallback, errorCallback, queryParams);
}

void ApiService::getTopEvents(const QString &startDate, const QString &endDate, int limit,
                              const SuccessCallback &successCallback,
                              const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    queryParams.addQueryItem("limit", QString::number(limit));
    
    get("/api/analytics/event-stats", successCallback, errorCallback, queryParams);
}

void ApiService::getTopUsers(int page, int pageSize,
                              const SuccessCallback &successCallback,
                              const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("page", QString::number(page));
    queryParams.addQueryItem("pageSize", QString::number(pageSize));
    
    get("/api/analytics/user/list", successCallback, errorCallback, queryParams);
}

void ApiService::getRealTimeStats(const SuccessCallback &successCallback,
                                  const ErrorCallback &errorCallback)
{
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", today);
    queryParams.addQueryItem("endDate", today);
    queryParams.addQueryItem("interval", "day");
    
    get("/api/analytics/trends", successCallback, errorCallback, queryParams);
}