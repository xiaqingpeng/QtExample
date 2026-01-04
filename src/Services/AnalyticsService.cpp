#include "AnalyticsService.h"
#include <QUuid>
#include <QDateTime>
#include <QDebug>
#include <QFutureWatcher>

AnalyticsService::AnalyticsService(INetworkService* networkService, QObject *parent)
    : QObject(parent)
    , m_networkService(networkService)
    , m_baseUrl("http://120.48.95.51:7001/api/analytics")
{
    m_sessionId = generateSessionId();
}

AnalyticsService::~AnalyticsService()
{
}

void AnalyticsService::trackEvent(const QString& eventName, const QJsonObject& properties)
{
    QJsonObject eventData;
    eventData["event"] = eventName;
    eventData["properties"] = properties;
    eventData["userId"] = m_userId;
    eventData["sessionId"] = m_sessionId;
    eventData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    sendEvent("/events", eventData);
    emit eventTracked(eventName, properties);
}

void AnalyticsService::trackPageView(const QString& pageName, const QJsonObject& properties)
{
    QJsonObject pageData = properties;
    pageData["page"] = pageName;
    trackEvent("page_view", pageData);
}

void AnalyticsService::trackUserAction(const QString& action, const QJsonObject& properties)
{
    QJsonObject actionData = properties;
    actionData["action"] = action;
    trackEvent("user_action", actionData);
}

QFuture<QJsonObject> AnalyticsService::getActivityStats(const QString& startDate, const QString& endDate)
{
    QString url = QString("%1/activity?startDate=%2&endDate=%3")
                  .arg(m_baseUrl, startDate, endDate);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getEventStats(const QString& startDate, const QString& endDate)
{
    QString url = QString("%1/events?startDate=%2&endDate=%3")
                  .arg(m_baseUrl, startDate, endDate);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getRetentionStats()
{
    QString url = QString("%1/retention").arg(m_baseUrl);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getPageViewStats(const QString& startDate, const QString& endDate)
{
    QString url = QString("%1/pageviews?startDate=%2&endDate=%3")
                  .arg(m_baseUrl, startDate, endDate);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getTrendAnalysis(const QString& metric, const QString& startDate, const QString& endDate)
{
    QString url = QString("%1/trends?metric=%2&startDate=%3&endDate=%4")
                  .arg(m_baseUrl, metric, startDate, endDate);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getTopPages(const QString& startDate, const QString& endDate, int limit)
{
    QString url = QString("%1/top-pages?startDate=%2&endDate=%3&limit=%4")
                  .arg(m_baseUrl, startDate, endDate).arg(limit);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getTopEvents(const QString& startDate, const QString& endDate, int limit)
{
    QString url = QString("%1/top-events?startDate=%2&endDate=%3&limit=%4")
                  .arg(m_baseUrl, startDate, endDate).arg(limit);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getTopUsers(int page, int pageSize)
{
    QString url = QString("%1/top-users?page=%2&pageSize=%3")
                  .arg(m_baseUrl).arg(page).arg(pageSize);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getRealTimeStats()
{
    QString url = QString("%1/realtime").arg(m_baseUrl);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getUserProfile(const QString& userId)
{
    QString url = QString("http://120.48.95.51:7001/api/user-profile/%1").arg(userId);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getUserTags(const QString& userId)
{
    QString url = QString("http://120.48.95.51:7001/api/user-profile/%1/tags").arg(userId);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getUserBehaviorStats(const QString& userId)
{
    QString url = QString("http://120.48.95.51:7001/api/user-profile/%1/behavior").arg(userId);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getUserInterestProfile(const QString& userId)
{
    QString url = QString("http://120.48.95.51:7001/api/user-profile/%1/interests").arg(userId);
    return m_networkService->get(url);
}

QFuture<QJsonObject> AnalyticsService::getUserValueAssessment(const QString& userId)
{
    QString url = QString("http://120.48.95.51:7001/api/user-profile/%1/value").arg(userId);
    return m_networkService->get(url);
}

void AnalyticsService::setUserId(const QString& userId)
{
    m_userId = userId;
}

void AnalyticsService::setSessionId(const QString& sessionId)
{
    m_sessionId = sessionId;
}

void AnalyticsService::sendEvent(const QString& endpoint, const QJsonObject& data)
{
    QString url = m_baseUrl + endpoint;
    
    auto future = m_networkService->post(url, data);
    
    // 异步处理结果，不阻塞UI
    auto watcher = new QFutureWatcher<QJsonObject>(this);
    connect(watcher, &QFutureWatcher<QJsonObject>::finished, [watcher]() {
        QJsonObject result = watcher->result();
        if (!result["success"].toBool()) {
            qWarning() << "Analytics event failed:" << result["message"].toString();
        }
        watcher->deleteLater();
    });
    
    watcher->setFuture(future);
}

QString AnalyticsService::generateSessionId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}