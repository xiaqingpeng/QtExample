#ifndef APISERVICE_H
#define APISERVICE_H

#include <QObject>
#include <QJsonObject>
#include <functional>
#include <QUrlQuery>

class NetworkManager;

class ApiService : public QObject
{
    Q_OBJECT

public:
    explicit ApiService(NetworkManager *networkManager, QObject *parent = nullptr);
    ~ApiService();

    using SuccessCallback = std::function<void(const QJsonObject &)>;
    using ErrorCallback = std::function<void(const QString &)>;

    // 用户画像API
    void getUserProfile(const QString &userId,
                       const SuccessCallback &successCallback,
                       const ErrorCallback &errorCallback = nullptr);
    void getUserTags(const QString &userId,
                    const SuccessCallback &successCallback,
                    const ErrorCallback &errorCallback = nullptr);
    void getUserBehaviorStats(const QString &userId,
                             const SuccessCallback &successCallback,
                             const ErrorCallback &errorCallback = nullptr);
    void getUserInterestProfile(const QString &userId,
                                const SuccessCallback &successCallback,
                                const ErrorCallback &errorCallback = nullptr);
    void getUserValueAssessment(const QString &userId,
                                const SuccessCallback &successCallback,
                                const ErrorCallback &errorCallback = nullptr);
    void updateUserProfile(const QString &userId,
                          const QJsonObject &data,
                          const SuccessCallback &successCallback,
                          const ErrorCallback &errorCallback = nullptr);

    // 统计报表API
    void getActivityStats(const QString &startDate, const QString &endDate,
                         const SuccessCallback &successCallback,
                         const ErrorCallback &errorCallback = nullptr);
    void getRetentionStats(const SuccessCallback &successCallback,
                          const ErrorCallback &errorCallback = nullptr);
    void getPageViewStats(const QString &startDate, const QString &endDate,
                         const SuccessCallback &successCallback,
                         const ErrorCallback &errorCallback = nullptr);
    void getEventStats(const QString &startDate, const QString &endDate,
                      const SuccessCallback &successCallback,
                      const ErrorCallback &errorCallback = nullptr);
    void getTrendAnalysis(const QString &metric, const QString &startDate, const QString &endDate,
                         const SuccessCallback &successCallback,
                         const ErrorCallback &errorCallback = nullptr);
    void getTopPages(const QString &startDate, const QString &endDate, int limit,
                    const SuccessCallback &successCallback,
                    const ErrorCallback &errorCallback = nullptr);
    void getTopEvents(const QString &startDate, const QString &endDate, int limit,
                     const SuccessCallback &successCallback,
                     const ErrorCallback &errorCallback = nullptr);
    void getTopUsers(int page, int pageSize,
                    const SuccessCallback &successCallback,
                    const ErrorCallback &errorCallback = nullptr);
    void getRealTimeStats(const SuccessCallback &successCallback,
                         const ErrorCallback &errorCallback = nullptr);

private:
    NetworkManager *m_networkManager;
};

#endif // APISERVICE_H