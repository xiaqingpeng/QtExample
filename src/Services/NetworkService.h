#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include "../Interfaces/INetworkService.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QHash>
#include <QPromise>

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

private slots:
    void onReplyFinished();

private:
    void setupRequest(QNetworkRequest& request);
    QFuture<QJsonObject> executeRequest(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_manager;
    QHash<QString, QString> m_defaultHeaders;
    int m_timeoutMs;
    int m_maxRetries;
    int m_retryDelayMs;
    
    struct RequestInfo {
        std::shared_ptr<QPromise<QJsonObject>> promise;
        int retryCount;
        
        RequestInfo() : promise(std::make_shared<QPromise<QJsonObject>>()), retryCount(0) {}
    };
    
    QHash<QNetworkReply*, RequestInfo> m_pendingRequests;
};

#endif // NETWORKSERVICE_H