#include "networkmanager.h"
#include <QDebug>
#include <QNetworkProxy>
#include <QUrl>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    // 禁用代理，避免认证问题
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    m_networkManager->setProxy(QNetworkProxy::NoProxy);
}

NetworkManager::~NetworkManager()
{
}

QString NetworkManager::getPlatform()
{
#ifdef Q_OS_MAC
    return "Mac";
#elif defined(Q_OS_WIN)
    return "Windows";
#elif defined(Q_OS_LINUX)
    return "Linux";
#elif defined(Q_OS_ANDROID)
    return "Android";
#elif defined(Q_OS_IOS)
    return "iOS";
#else
    return "Web";
#endif
}

QNetworkRequest NetworkManager::createRequest(const QString &url)
{
    QNetworkRequest request{QUrl(url)};
    
    // 设置通用请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("User-Agent", "QtApp/1.0");
    
    // 添加平台识别头，让后端能够正确识别平台
    QString platform = getPlatform();
    request.setRawHeader("x-platform", platform.toUtf8());
    
    qDebug() << "NetworkManager - Creating request for:" << url;
    qDebug() << "NetworkManager - Platform:" << platform;
    
    return request;
}

void NetworkManager::get(const QString &url,
                          const SuccessCallback &successCallback,
                          const ErrorCallback &errorCallback,
                          const QUrlQuery &queryParams)
{
    QString fullUrl = url;
    if (!queryParams.isEmpty()) {
        fullUrl += "?" + queryParams.toString();
    }
    
    QNetworkRequest request = createRequest(fullUrl);
    
    qDebug() << "NetworkManager - GET request:" << fullUrl;
    
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, successCallback, errorCallback]() {
        handleResponse(reply, successCallback, errorCallback);
    });
}

void NetworkManager::post(const QString &url,
                           const QJsonObject &data,
                           const SuccessCallback &successCallback,
                           const ErrorCallback &errorCallback)
{
    QNetworkRequest request = createRequest(url);
    
    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();
    
    qDebug() << "NetworkManager - POST request:" << url;
    qDebug() << "NetworkManager - Data:" << doc.toJson(QJsonDocument::Compact);
    
    QNetworkReply *reply = m_networkManager->post(request, jsonData);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, successCallback, errorCallback]() {
        handleResponse(reply, successCallback, errorCallback);
    });
}

void NetworkManager::put(const QString &url,
                          const QJsonObject &data,
                          const SuccessCallback &successCallback,
                          const ErrorCallback &errorCallback)
{
    QNetworkRequest request = createRequest(url);
    
    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();
    
    qDebug() << "NetworkManager - PUT request:" << url;
    qDebug() << "NetworkManager - Data:" << doc.toJson(QJsonDocument::Compact);
    
    QNetworkReply *reply = m_networkManager->put(request, jsonData);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, successCallback, errorCallback]() {
        handleResponse(reply, successCallback, errorCallback);
    });
}

void NetworkManager::deleteResource(const QString &url,
                                      const SuccessCallback &successCallback,
                                      const ErrorCallback &errorCallback)
{
    QNetworkRequest request = createRequest(url);
    
    qDebug() << "NetworkManager - DELETE request:" << url;
    
    QNetworkReply *reply = m_networkManager->deleteResource(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, successCallback, errorCallback]() {
        handleResponse(reply, successCallback, errorCallback);
    });
}

void NetworkManager::handleResponse(QNetworkReply *reply,
                                     const SuccessCallback &successCallback,
                                     const ErrorCallback &errorCallback)
{
    if (!reply) {
        if (errorCallback) {
            errorCallback("Invalid reply");
        }
        return;
    }
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        // qDebug() << "NetworkManager - Response:" << doc.toJson(QJsonDocument::Compact);
        
        if (doc.isObject()) {
            QJsonObject rootObj = doc.object();
            if (successCallback) {
                successCallback(rootObj);
            }
        } else {
            qDebug() << "NetworkManager - Invalid JSON response";
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
        }
    } else {
        QString errorMsg = reply->errorString();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        
        qDebug() << "NetworkManager - Error:" << errorMsg;
        qDebug() << "NetworkManager - HTTP Status:" << statusCode;
        // qDebug() << "NetworkManager - Response Data:" << reply->readAll();
        
        if (errorCallback) {
            errorCallback(QString("%1 (HTTP %2)").arg(errorMsg).arg(statusCode));
        }
    }
    
    reply->deleteLater();
}