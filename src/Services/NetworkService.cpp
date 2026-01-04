#include "NetworkService.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <QDebug>
#include <QPromise>

NetworkService::NetworkService(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_timeoutMs(30000)
    , m_maxRetries(3)
    , m_retryDelayMs(1000)
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &NetworkService::onReplyFinished);
}

NetworkService::~NetworkService()
{
    // 清理待处理的请求
    for (auto it = m_pendingRequests.begin(); it != m_pendingRequests.end(); ++it) {
        QNetworkReply* reply = it.key();
        RequestInfo& info = it.value();
        
        if (info.promise && !info.promise->isCanceled()) {
            info.promise->finish();
        }
        
        reply->abort();
        reply->deleteLater();
    }
    m_pendingRequests.clear();
}

QFuture<QJsonObject> NetworkService::get(const QString& url, const QUrlQuery& params)
{
    QUrl requestUrl(url);
    if (!params.isEmpty()) {
        requestUrl.setQuery(params);
    }
    
    QNetworkRequest request(requestUrl);
    setupRequest(request);
    
    QNetworkReply* reply = m_manager->get(request);
    return executeRequest(reply);
}

QFuture<QJsonObject> NetworkService::post(const QString& url, const QJsonObject& data)
{
    QNetworkRequest request(url);
    setupRequest(request);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(data);
    QNetworkReply* reply = m_manager->post(request, doc.toJson());
    return executeRequest(reply);
}

QFuture<QJsonObject> NetworkService::put(const QString& url, const QJsonObject& data)
{
    QNetworkRequest request(url);
    setupRequest(request);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(data);
    QNetworkReply* reply = m_manager->put(request, doc.toJson());
    return executeRequest(reply);
}

QFuture<QJsonObject> NetworkService::deleteResource(const QString& url)
{
    QNetworkRequest request(url);
    setupRequest(request);
    
    QNetworkReply* reply = m_manager->deleteResource(request);
    return executeRequest(reply);
}

QFuture<QJsonObject> NetworkService::uploadFile(const QString& url, const QString& filePath, const QString& fieldName)
{
    // 简化实现
    auto promise = std::make_shared<QPromise<QJsonObject>>();
    promise->start();
    
    QJsonObject result;
    result["success"] = false;
    result["message"] = "文件上传功能尚未实现";
    
    promise->addResult(result);
    promise->finish();
    
    return promise->future();
}

void NetworkService::setDefaultHeaders(const QHash<QString, QString>& headers)
{
    m_defaultHeaders = headers;
}

void NetworkService::setTimeout(int timeoutMs)
{
    m_timeoutMs = timeoutMs;
}

void NetworkService::setRetryPolicy(int maxRetries, int delayMs)
{
    m_maxRetries = maxRetries;
    m_retryDelayMs = delayMs;
}

void NetworkService::setupRequest(QNetworkRequest& request)
{
    // 设置默认请求头
    for (auto it = m_defaultHeaders.begin(); it != m_defaultHeaders.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }
    
    // 设置超时
    request.setTransferTimeout(m_timeoutMs);
    
    // 设置用户代理
    request.setRawHeader("User-Agent", "Qt Enterprise App/2.0");
}

QFuture<QJsonObject> NetworkService::executeRequest(QNetworkReply* reply)
{
    RequestInfo info;
    info.promise->start();
    info.retryCount = 0;
    
    QFuture<QJsonObject> future = info.promise->future();
    m_pendingRequests.insert(reply, info);
    
    return future;
}

void NetworkService::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply || !m_pendingRequests.contains(reply)) {
        return;
    }
    
    RequestInfo info = m_pendingRequests.take(reply);
    
    QJsonObject result;
    
    if (reply->error() != QNetworkReply::NoError) {
        result["success"] = false;
        result["message"] = reply->errorString();
        result["error_code"] = reply->error();
    } else {
        QByteArray data = reply->readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        
        if (parseError.error != QJsonParseError::NoError) {
            result["success"] = false;
            result["message"] = "JSON解析错误: " + parseError.errorString();
        } else {
            result = doc.object();
            if (!result.contains("success")) {
                result["success"] = true;
            }
        }
    }
    
    if (info.promise) {
        info.promise->addResult(result);
        info.promise->finish();
    }
    
    reply->deleteLater();
}