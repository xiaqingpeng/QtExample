#ifndef APISERVICE_H
#define APISERVICE_H

#include "../Interfaces/INetworkService.h"
#include <QObject>
#include <QJsonObject>
#include <functional>
#include <QUrlQuery>

class NetworkManagerAdapter;

class ApiService : public QObject, public INetworkService
{
    Q_OBJECT
    Q_INTERFACES(INetworkService)

public:
    explicit ApiService(NetworkManagerAdapter *networkManager, QObject *parent = nullptr);
    explicit ApiService(QObject *parent = nullptr);
    ~ApiService();

    using SuccessCallback = std::function<void(const QJsonObject &)>;
    using ErrorCallback = std::function<void(const QString &)>;

    void setBaseUrl(const QString &baseUrl);

    // INetworkService 接口实现（返回 QFuture）
    QFuture<QJsonObject> get(const QString& url, const QUrlQuery& params = {}) override;
    QFuture<QJsonObject> post(const QString& url, const QJsonObject& data) override;
    QFuture<QJsonObject> put(const QString& url, const QJsonObject& data) override;
    QFuture<QJsonObject> deleteResource(const QString& url) override;
    QFuture<QJsonObject> uploadFile(const QString& url, const QString& filePath, const QString& fieldName = "file") override;
    void setDefaultHeaders(const QHash<QString, QString>& headers) override;
    void setTimeout(int timeoutMs) override;
    void setRetryPolicy(int maxRetries, int delayMs) override;

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

    // 基础网络请求方法（回调风格，供外部直接使用）
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

private:
    NetworkManagerAdapter *m_networkManagerAdapter;
    bool m_ownsNetworkManager;
    class NetworkService *m_networkService;
};

#endif // APISERVICE_H