#include "ApiService.h"
#include "NetworkManagerAdapter.h"
#include <QDate>

ApiService::ApiService(NetworkManagerAdapter *networkManager, QObject *parent)
    : QObject(parent)
    , m_networkManagerAdapter(networkManager)
{
}

ApiService::~ApiService()
{
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