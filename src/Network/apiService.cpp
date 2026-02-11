#include "apiservice.h"
#include "networkmanager.h"
#include <QDate>

ApiService::ApiService(NetworkManager *networkManager, QObject *parent)
    : QObject(parent)
    , m_networkManager(networkManager)
{
}

ApiService::~ApiService()
{
}

// 用户画像API实现
void ApiService::getUserProfile(const QString &userId,
                                const SuccessCallback &successCallback,
                                const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    m_networkManager->get("/api/analytics/user/profile", successCallback, errorCallback, queryParams);
}

void ApiService::getUserTags(const QString &userId,
                           const SuccessCallback &successCallback,
                           const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    m_networkManager->get("/api/analytics/user/tags", successCallback, errorCallback, queryParams);
}

void ApiService::getUserBehaviorStats(const QString &userId,
                                      const SuccessCallback &successCallback,
                                      const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    m_networkManager->get("/api/analytics/user/behavior", successCallback, errorCallback, queryParams);
}

void ApiService::getUserInterestProfile(const QString &userId,
                                       const SuccessCallback &successCallback,
                                       const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    m_networkManager->get("/api/analytics/user/interest", successCallback, errorCallback, queryParams);
}

void ApiService::getUserValueAssessment(const QString &userId,
                                       const SuccessCallback &successCallback,
                                       const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    m_networkManager->get("/api/analytics/user/value", successCallback, errorCallback, queryParams);
}

void ApiService::updateUserProfile(const QString &userId,
                                  const QJsonObject &data,
                                  const SuccessCallback &successCallback,
                                  const ErrorCallback &errorCallback)
{
    QString url = "/api/user-profile/update/" + userId;
    m_networkManager->post(url, data, successCallback, errorCallback);
}

// 统计报表API实现
void ApiService::getActivityStats(const QString &startDate, const QString &endDate,
                                 const SuccessCallback &successCallback,
                                 const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    
    m_networkManager->get("/api/analytics/activity", successCallback, errorCallback, queryParams);
}

void ApiService::getRetentionStats(const SuccessCallback &successCallback,
                                  const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("days", "7");
    
    m_networkManager->get("/api/analytics/retention", successCallback, errorCallback, queryParams);
}

void ApiService::getPageViewStats(const QString &startDate, const QString &endDate,
                                 const SuccessCallback &successCallback,
                                 const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    
    m_networkManager->get("/api/analytics/page-views", successCallback, errorCallback, queryParams);
}

void ApiService::getEventStats(const QString &startDate, const QString &endDate,
                              const SuccessCallback &successCallback,
                              const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    
    m_networkManager->get("/api/analytics/event-stats", successCallback, errorCallback, queryParams);
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
    
    m_networkManager->get("/api/analytics/trends", successCallback, errorCallback, queryParams);
}

void ApiService::getTopPages(const QString &startDate, const QString &endDate, int limit,
                             const SuccessCallback &successCallback,
                             const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    queryParams.addQueryItem("limit", QString::number(limit));
    
    m_networkManager->get("/api/analytics/page-views", successCallback, errorCallback, queryParams);
}

void ApiService::getTopEvents(const QString &startDate, const QString &endDate, int limit,
                              const SuccessCallback &successCallback,
                              const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    queryParams.addQueryItem("limit", QString::number(limit));
    
    m_networkManager->get("/api/analytics/event-stats", successCallback, errorCallback, queryParams);
}

void ApiService::getTopUsers(int page, int pageSize,
                              const SuccessCallback &successCallback,
                              const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("page", QString::number(page));
    queryParams.addQueryItem("pageSize", QString::number(pageSize));
    
    m_networkManager->get("/api/analytics/user/list", successCallback, errorCallback, queryParams);
}

void ApiService::getRealTimeStats(const SuccessCallback &successCallback,
                                  const ErrorCallback &errorCallback)
{
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", today);
    queryParams.addQueryItem("endDate", today);
    queryParams.addQueryItem("interval", "day");
    
    m_networkManager->get("/api/analytics/trends", successCallback, errorCallback, queryParams);
}