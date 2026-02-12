#include "NetworkService.h"
#include "PersistentCookieJar.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <QDebug>
#include <QPromise>
#include <QSettings>
#include <QCoreApplication>

NetworkService::NetworkService(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_timeoutMs(30000)
    , m_maxRetries(3)
    , m_retryDelayMs(1000)
    , m_baseUrl("http://120.48.95.51:7001")
{
    m_manager->setCookieJar(new PersistentCookieJar(this));
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &NetworkService::onReplyFinished);
    //  qDebug() << "[NetworkService] Created with base URL:" << m_baseUrl;
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
    Q_UNUSED(url)
    Q_UNUSED(filePath)
    Q_UNUSED(fieldName)
    
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

void NetworkService::getCallback(const QString &url,
                                 const SuccessCallback &successCallback,
                                 const ErrorCallback &errorCallback,
                                 const QUrlQuery &queryParams)
{
    QString fullUrl = buildFullUrl(url);
    if (!queryParams.isEmpty()) {
        QUrl requestUrl(fullUrl);
        requestUrl.setQuery(queryParams);
        fullUrl = requestUrl.toString();
    }
    
    QNetworkRequest request = createRequest(fullUrl);
    QNetworkReply *reply = m_manager->get(request);
    
    RequestInfo info;
    info.promise->start();
    info.successCallback = successCallback;
    info.errorCallback = errorCallback;
    m_pendingRequests.insert(reply, info);
}

void NetworkService::postCallback(const QString &url,
                                   const QJsonObject &data,
                                   const SuccessCallback &successCallback,
                                   const ErrorCallback &errorCallback)
{
    QString fullUrl = buildFullUrl(url);
    QNetworkRequest request = createRequest(fullUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(data);
    //  qDebug() << "[NetworkService] About to call m_manager->post()";
    QNetworkReply *reply = m_manager->post(request, doc.toJson());
    //  qDebug() << "[NetworkService] m_manager->post() returned";
    
    if (!reply) {
        //  qDebug() << "[NetworkService] ERROR: Reply is null!";
        return;
    }
    
    //  qDebug() << "[NetworkService] POST request to:" << fullUrl;
    //  qDebug() << "[NetworkService] Request data:" << doc.toJson(QJsonDocument::Compact);
    //  qDebug() << "[NetworkService] Reply created:" << reply;
    
    RequestInfo info;
    //  qDebug() << "[NetworkService] Creating RequestInfo";
    info.promise->start();
    //  qDebug() << "[NetworkService] Promise started";
    info.successCallback = successCallback;
    info.errorCallback = errorCallback;
    //  qDebug() << "[NetworkService] Inserting into pending requests";
    m_pendingRequests.insert(reply, info);
    //  qDebug() << "[NetworkService] Pending requests count:" << m_pendingRequests.size();
}

void NetworkService::putCallback(const QString &url,
                                  const QJsonObject &data,
                                  const SuccessCallback &successCallback,
                                  const ErrorCallback &errorCallback)
{
    QString fullUrl = buildFullUrl(url);
    QNetworkRequest request = createRequest(fullUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonDocument doc(data);
    QNetworkReply *reply = m_manager->put(request, doc.toJson());
    
    RequestInfo info;
    info.promise->start();
    info.successCallback = successCallback;
    info.errorCallback = errorCallback;
    m_pendingRequests.insert(reply, info);
}

void NetworkService::deleteCallback(const QString &url,
                                      const SuccessCallback &successCallback,
                                      const ErrorCallback &errorCallback)
{
    QString fullUrl = buildFullUrl(url);
    QNetworkRequest request = createRequest(fullUrl);
    
    QNetworkReply *reply = m_manager->deleteResource(request);
    
    RequestInfo info;
    info.promise->start();
    info.successCallback = successCallback;
    info.errorCallback = errorCallback;
    m_pendingRequests.insert(reply, info);
}

void NetworkService::uploadFileCallback(const QString &url,
                                          const QString &filePath,
                                          const QString &fileFieldName,
                                          const SuccessCallback &successCallback,
                                          const ErrorCallback &errorCallback)
{
    Q_UNUSED(url)
    Q_UNUSED(filePath)
    Q_UNUSED(fileFieldName)
    Q_UNUSED(successCallback)
    
    if (errorCallback) {
        errorCallback("文件上传功能尚未实现");
    }
}

QString NetworkService::getPlatform()
{
#ifdef Q_OS_WINDOWS
    return "windows";
#elif defined(Q_OS_MACOS)
    return "macos";
#elif defined(Q_OS_LINUX)
    return "linux";
#else
    return "unknown";
#endif
}

void NetworkService::setBaseUrl(const QString &baseUrl)
{
    m_baseUrl = baseUrl;
}

QNetworkRequest NetworkService::createRequest(const QString &url)
{
    QNetworkRequest request(url);
    setupRequest(request);
    return request;
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
    //  qDebug() << "[NetworkService] Setting up request for:" << request.url();
    
    // 设置默认请求头
    for (auto it = m_defaultHeaders.begin(); it != m_defaultHeaders.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }
    
    // 设置超时
    request.setTransferTimeout(m_timeoutMs);
    //  qDebug() << "[NetworkService] Timeout set to:" << m_timeoutMs << "ms";
    
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

void NetworkService::onReplyFinished(QNetworkReply* reply)
{
    //  qDebug() << "[NetworkService] onReplyFinished called, reply:" << reply;
    
    if (!reply) {
        //  qDebug() << "[NetworkService] Reply is null";
        return;
    }
    
    if (!m_pendingRequests.contains(reply)) {
        //  qDebug() << "[NetworkService] Reply not in pending requests";
        return;
    }
    
    RequestInfo info = m_pendingRequests.take(reply);
    //  qDebug() << "[NetworkService] Processing reply, has callbacks:" << (info.successCallback || info.errorCallback);
    
    QJsonObject result;
    
    if (reply->error() != QNetworkReply::NoError) {
        //  qDebug() << "[NetworkService] Request failed:" << reply->error() << reply->errorString();
        result["success"] = false;
        result["message"] = reply->errorString();
        result["error_code"] = reply->error();
    } else {
        QByteArray data = reply->readAll();
        //  qDebug() << "[NetworkService] Response data:" << data;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        
        if (parseError.error != QJsonParseError::NoError) {
            //  qDebug() << "[NetworkService] JSON parse error:" << parseError.errorString();
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
    
    if (info.successCallback || info.errorCallback) {
        //  qDebug() << "[NetworkService] About to call callbacks, hasSuccess:" << (bool)info.successCallback << "hasError:" << (bool)info.errorCallback;
        if (reply->error() != QNetworkReply::NoError) {
            //  qDebug() << "[NetworkService] Calling error callback with:" << reply->errorString();
            if (info.errorCallback) {
                info.errorCallback(reply->errorString());
                //  qDebug() << "[NetworkService] Error callback completed";
            }
        } else {
            //  qDebug() << "[NetworkService] Calling success callback with result:" << result;
            if (info.successCallback) {
                info.successCallback(result);
                //  qDebug() << "[NetworkService] Success callback completed";
            }
        }
    } else {
        //  qDebug() << "[NetworkService] No callbacks to call";
    }
    
    reply->deleteLater();
}

QString NetworkService::getToken()
{
    QSettings settings;
    return settings.value("token").toString();
}

QString NetworkService::buildFullUrl(const QString &path)
{
    //  qDebug() << "[NetworkService] // qDebug():" << path << "baseUrl:" << m_baseUrl;
    if (m_baseUrl.isEmpty()) {
        //  qDebug() << "[NetworkService] WARNING: Base URL is empty, returning path as is";
        return path;
    }
    
    QUrl baseUrl(m_baseUrl);
    QUrl url(path);
    
    if (url.isRelative()) {
        QString result = baseUrl.resolved(url).toString();
        //  qDebug() << "[NetworkService] Resolved URL:" << result;
        return result;
    }
    
    //  qDebug() << "[NetworkService] URL is absolute, returning as is:" << path;
    return path;
}

void NetworkService::setRequestTimeout(QNetworkReply *reply, int timeoutMs)
{
    QTimer::singleShot(timeoutMs, reply, [reply]() {
        if (reply->isRunning()) {
            reply->abort();
        }
    });
}

void NetworkService::getWithRetry(const QString &url,
                                    const SuccessCallback &successCallback,
                                    const ErrorCallback &errorCallback,
                                    const QUrlQuery &queryParams,
                                    int retryCount)
{
    auto wrappedSuccess = [successCallback](const QJsonObject &result) {
        successCallback(result);
    };
    
    auto wrappedError = [this, url, successCallback, errorCallback, queryParams, retryCount](const QString &error) {
        if (retryCount < m_maxRetries) {
            QTimer::singleShot(m_retryDelayMs, this, [this, url, successCallback, errorCallback, queryParams, retryCount]() {
                getWithRetry(url, successCallback, errorCallback, queryParams, retryCount + 1);
            });
        } else {
            if (errorCallback) {
                errorCallback(error);
            }
        }
    };
    
    getCallback(url, wrappedSuccess, wrappedError, queryParams);
}