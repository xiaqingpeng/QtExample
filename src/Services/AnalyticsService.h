#ifndef ANALYTICSSERVICE_H
#define ANALYTICSSERVICE_H

#include "../Interfaces/INetworkService.h"
#include <QObject>
#include <QJsonObject>
#include <QFuture>

/**
 * @brief 分析服务
 * 
 * 提供数据分析和统计功能
 */
class AnalyticsService : public QObject
{
    Q_OBJECT

public:
    explicit AnalyticsService(INetworkService* networkService, QObject *parent = nullptr);
    ~AnalyticsService();

    // 事件追踪
    void trackEvent(const QString& eventName, const QJsonObject& properties = {});
    void trackPageView(const QString& pageName, const QJsonObject& properties = {});
    void trackUserAction(const QString& action, const QJsonObject& properties = {});

    // 统计数据获取
    QFuture<QJsonObject> getActivityStats(const QString& startDate, const QString& endDate);
    QFuture<QJsonObject> getEventStats(const QString& startDate, const QString& endDate);
    QFuture<QJsonObject> getRetentionStats();
    QFuture<QJsonObject> getPageViewStats(const QString& startDate, const QString& endDate);
    QFuture<QJsonObject> getTrendAnalysis(const QString& metric, const QString& startDate, const QString& endDate);
    QFuture<QJsonObject> getTopPages(const QString& startDate, const QString& endDate, int limit = 10);
    QFuture<QJsonObject> getTopEvents(const QString& startDate, const QString& endDate, int limit = 10);
    QFuture<QJsonObject> getTopUsers(int page = 1, int pageSize = 20);
    QFuture<QJsonObject> getRealTimeStats();

    // 用户画像
    QFuture<QJsonObject> getUserProfile(const QString& userId);
    QFuture<QJsonObject> getUserTags(const QString& userId);
    QFuture<QJsonObject> getUserBehaviorStats(const QString& userId);
    QFuture<QJsonObject> getUserInterestProfile(const QString& userId);
    QFuture<QJsonObject> getUserValueAssessment(const QString& userId);

    // 配置
    void setUserId(const QString& userId);
    void setSessionId(const QString& sessionId);

signals:
    void eventTracked(const QString& eventName, const QJsonObject& properties);
    void statsUpdated(const QString& statsType, const QJsonObject& data);

private:
    void sendEvent(const QString& endpoint, const QJsonObject& data);
    QString generateSessionId();

private:
    INetworkService* m_networkService;
    QString m_userId;
    QString m_sessionId;
    QString m_baseUrl;
};

#endif // ANALYTICSSERVICE_H