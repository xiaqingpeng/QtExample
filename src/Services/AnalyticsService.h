#pragma once

#include <QObject>
#include <QJsonObject>
#include <QFuture>

class ApiService;  // 前向声明

class AnalyticsService : public QObject
{
    Q_OBJECT
public:
    explicit AnalyticsService(ApiService* apiService, QObject* parent = nullptr);
    ~AnalyticsService();

    void trackEvent(const QString& eventName, const QJsonObject& properties = {});
    void trackPageView(const QString& pageName, const QJsonObject& properties = {});
    void trackUserAction(const QString& action, const QJsonObject& properties = {});

    // 统计相关接口（QFuture 版本）
    QFuture<QJsonObject> getActivityStats(const QString& startDate, const QString& endDate);
    QFuture<QJsonObject> getEventStats(const QString& startDate, const QString& endDate);
    QFuture<QJsonObject> getRetentionStats();
    QFuture<QJsonObject> getPageViewStats(const QString& startDate, const QString& endDate);
    QFuture<QJsonObject> getTrendAnalysis(const QString& metric,
                                          const QString& startDate,
                                          const QString& endDate);
    QFuture<QJsonObject> getTopPages(const QString& startDate, const QString& endDate, int limit);
    QFuture<QJsonObject> getTopEvents(const QString& startDate, const QString& endDate, int limit);
    QFuture<QJsonObject> getTopUsers(int page, int pageSize);
    QFuture<QJsonObject> getRealTimeStats();

    // 用户画像相关接口
    QFuture<QJsonObject> getUserProfile(const QString& userId);
    QFuture<QJsonObject> getUserTags(const QString& userId);
    QFuture<QJsonObject> getUserBehaviorStats(const QString& userId);
    QFuture<QJsonObject> getUserInterestProfile(const QString& userId);
    QFuture<QJsonObject> getUserValueAssessment(const QString& userId);

    void setUserId(const QString& userId);
    void setSessionId(const QString& sessionId);

signals:
    void eventTracked(const QString& eventName, const QJsonObject& properties);

private:
    void sendEvent(const QString& endpoint, const QJsonObject& data);
    QString generateSessionId();

private:
    ApiService* m_apiService;   // 统一 HTTP 出口
    QString     m_baseUrl;      // 仍保留完整 URL，兼容现有后端
    QString     m_userId;
    QString     m_sessionId;
};
