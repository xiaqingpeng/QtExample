#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <functional>
#include <QUrlQuery>
#include <QSettings>

// 自定义CookieJar，支持持久化存储
class PersistentCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    explicit PersistentCookieJar(QObject *parent = nullptr);
    
    QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const override;
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url) override;
    
private:
    void saveCookies();
    void loadCookies();
};

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    // 回调函数类型定义
    using SuccessCallback = std::function<void(const QJsonObject &)>;
    using ErrorCallback = std::function<void(const QString &)>;

    // GET请求
    void get(const QString &url,
             const SuccessCallback &successCallback,
             const ErrorCallback &errorCallback = nullptr,
             const QUrlQuery &queryParams = QUrlQuery());

    // POST请求
    void post(const QString &url,
              const QJsonObject &data,
              const SuccessCallback &successCallback,
              const ErrorCallback &errorCallback = nullptr);

    // PUT请求
    void put(const QString &url,
             const QJsonObject &data,
             const SuccessCallback &successCallback,
             const ErrorCallback &errorCallback = nullptr);

    // DELETE请求
    void deleteResource(const QString &url,
                        const SuccessCallback &successCallback,
                        const ErrorCallback &errorCallback = nullptr);

    // 文件上传请求
    void uploadFile(const QString &url,
                   const QString &filePath,
                   const QString &fileFieldName,
                   const SuccessCallback &successCallback,
                   const ErrorCallback &errorCallback = nullptr);

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

    // 获取平台标识
    static QString getPlatform();

    // 设置基础URL
    void setBaseUrl(const QString &baseUrl);

    // 创建带有通用请求头的QNetworkRequest
    QNetworkRequest createRequest(const QString &url);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_baseUrl;  // 基础URL
    int m_timeout;  // 超时时间（毫秒）
    int m_maxRetries;  // 最大重试次数

    // 处理响应
    void handleResponse(QNetworkReply *reply,
                       const SuccessCallback &successCallback,
                       const ErrorCallback &errorCallback);

    // 从QSettings获取Token
    QString getToken();

    // 构建完整URL
    QString buildFullUrl(const QString &path);

    // 设置请求超时
    void setRequestTimeout(QNetworkReply *reply, int timeoutMs);

    // 带重试的GET请求
    void getWithRetry(const QString &url,
                     const SuccessCallback &successCallback,
                     const ErrorCallback &errorCallback,
                     const QUrlQuery &queryParams,
                     int retryCount = 0);
};

#endif // NETWORKMANAGER_H