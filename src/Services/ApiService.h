#ifndef APISERVICE_H
#define APISERVICE_H

#include <QObject>
#include <QJsonObject>
#include <functional>
#include <QUrlQuery>

class NetworkManagerAdapter;

class ApiService : public QObject
{
    Q_OBJECT

public:
    explicit ApiService(NetworkManagerAdapter *networkManager, QObject *parent = nullptr);
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
    void get(const QString &url,
             const SuccessCallback &successCallback,
             const ErrorCallback &errorCallback,
             const QUrlQuery &queryParams = QUrlQuery());
    void post(const QString &url,
              const QJsonObject &data,
              const SuccessCallback &successCallback,
              const ErrorCallback &errorCallback = nullptr);
    void put(const QString &url,
             const QJsonObject &data,
             const SuccessCallback &successCallback,
             const ErrorCallback &errorCallback = nullptr);
    void deleteResource(const QString &url,
                        const SuccessCallback &successCallback,
                        const ErrorCallback &errorCallback = nullptr);
    void uploadFile(const QString &url,
                    const QString &filePath,
                    const QString &fileFieldName,
                    const SuccessCallback &successCallback,
                    const ErrorCallback &errorCallback = nullptr);

    NetworkManagerAdapter *m_networkManagerAdapter;
};

#endif // APISERVICE_H