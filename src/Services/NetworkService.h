#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include "../Interfaces/INetworkService.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QTimer>
#include <QHash>
#include <QPromise>
#include <QSettings>
#include <functional>

class PersistentCookieJar;

/**
 * @brief 网络服务实现
 * 
 * 实现INetworkService接口，提供HTTP请求功能
 */
class NetworkService : public QObject, public INetworkService
{
    Q_OBJECT
    Q_INTERFACES(INetworkService)

public:
    // 回调函数类型定义
    using SuccessCallback = std::function<void(const QJsonObject &)>;
    using ErrorCallback = std::function<void(const QString &)>;

    explicit NetworkService(QObject *parent = nullptr);
    ~NetworkService();

    // INetworkService接口实现
    QFuture<QJsonObject> get(const QString& url, const QUrlQuery& params = {}) override;
    QFuture<QJsonObject> post(const QString& url, const QJsonObject& data) override;
    QFuture<QJsonObject> put(const QString& url, const QJsonObject& data) override;
    QFuture<QJsonObject> deleteResource(const QString& url) override;
    QFuture<QJsonObject> uploadFile(const QString& url, const QString& filePath, const QString& fieldName = "file") override;
    
    void setDefaultHeaders(const QHash<QString, QString>& headers) override;
    void setTimeout(int timeoutMs) override;
    void setRetryPolicy(int maxRetries, int delayMs) override;

    // 回调风格的API（用于兼容旧代码）
    void getCallback(const QString &url,
                    const SuccessCallback &successCallback,
                    const ErrorCallback &errorCallback = nullptr,
                    const QUrlQuery &queryParams = QUrlQuery());

    void postCallback(const QString &url,
                      const QJsonObject &data,
                      const SuccessCallback &successCallback,
                      const ErrorCallback &errorCallback = nullptr);

    void putCallback(const QString &url,
                     const QJsonObject &data,
                     const SuccessCallback &successCallback,
                     const ErrorCallback &errorCallback = nullptr);

    void deleteCallback(const QString &url,
                         const SuccessCallback &successCallback,
                         const ErrorCallback &errorCallback = nullptr);

    void uploadFileCallback(const QString &url,
                             const QString &filePath,
                             const QString &fileFieldName,
                             const SuccessCallback &successCallback,
                             const ErrorCallback &errorCallback = nullptr);

    // 平台标识
    static QString getPlatform();

    // 设置基础URL
    void setBaseUrl(const QString &baseUrl);

    // 创建带有通用请求头的QNetworkRequest
    QNetworkRequest createRequest(const QString &url);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    void setupRequest(QNetworkRequest& request);
    QFuture<QJsonObject> executeRequest(QNetworkReply* reply);
    QString getToken();
    QString buildFullUrl(const QString &path);
    void setRequestTimeout(QNetworkReply *reply, int timeoutMs);
    void getWithRetry(const QString &url,
                      const SuccessCallback &successCallback,
                      const ErrorCallback &errorCallback,
                      const QUrlQuery &queryParams,
                      int retryCount = 0);

private:
    QNetworkAccessManager* m_manager;
    QHash<QString, QString> m_defaultHeaders;
    int m_timeoutMs;
    int m_maxRetries;
    int m_retryDelayMs;
    QString m_baseUrl;

    struct RequestInfo {
        std::shared_ptr<QPromise<QJsonObject>> promise;
        int retryCount;
        SuccessCallback successCallback;
        ErrorCallback errorCallback;
        
        RequestInfo() : promise(std::make_shared<QPromise<QJsonObject>>()), retryCount(0) {}
    };
    
    QHash<QNetworkReply*, RequestInfo> m_pendingRequests;
};

#endif // NETWORKSERVICE_H