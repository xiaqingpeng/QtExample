#include "networkmanager.h"
#include <QDebug>
#include <QNetworkProxy>
#include <QUrl>
#include <QSettings>
#include <QBuffer>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFileInfo>

// PersistentCookieJar 实现
PersistentCookieJar::PersistentCookieJar(QObject *parent)
    : QNetworkCookieJar(parent)
{
    loadCookies();
}

QList<QNetworkCookie> PersistentCookieJar::cookiesForUrl(const QUrl &url) const
{
    QList<QNetworkCookie> cookies = QNetworkCookieJar::cookiesForUrl(url);
    qDebug() << "PersistentCookieJar - Cookies for URL:" << url.toString() << "Count:" << cookies.size();
    for (const QNetworkCookie &cookie : cookies) {
        qDebug() << "  Cookie:" << cookie.name() << "=" << cookie.value();
    }
    return cookies;
}

bool PersistentCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
    qDebug() << "PersistentCookieJar - Setting cookies from URL:" << url.toString() << "Count:" << cookieList.size();
    
    // 修改Cookie的域名，使其适用于整个域名
    QList<QNetworkCookie> modifiedCookies;
    for (const QNetworkCookie &cookie : cookieList) {
        QNetworkCookie modifiedCookie = cookie;
        
        // 设置域名为整个域名（不包括端口）
        QString domain = url.host();
        modifiedCookie.setDomain(domain);
        
        // 设置路径为根路径，使其适用于所有路径
        modifiedCookie.setPath("/");
        
        qDebug() << "  Cookie:" << cookie.name() << "=" << cookie.value();
        qDebug() << "    Domain:" << modifiedCookie.domain() << "Path:" << modifiedCookie.path();
        
        modifiedCookies.append(modifiedCookie);
    }
    
    bool result = QNetworkCookieJar::setCookiesFromUrl(modifiedCookies, url);
    if (result) {
        saveCookies();
    }
    return result;
}

void PersistentCookieJar::saveCookies()
{
    QSettings settings("YourCompany", "QtApp");
    QList<QNetworkCookie> allCookies = QNetworkCookieJar::allCookies();
    
    QByteArray cookieData;
    QBuffer buffer(&cookieData);
    buffer.open(QIODevice::WriteOnly);
    
    for (const QNetworkCookie &cookie : allCookies) {
        buffer.write(cookie.toRawForm());
        buffer.write("\n");
    }
    
    settings.setValue("network/cookies", QString::fromUtf8(cookieData.toBase64()));
    qDebug() << "PersistentCookieJar - Saved" << allCookies.size() << "cookies";
}

void PersistentCookieJar::loadCookies()
{
    QSettings settings("YourCompany", "QtApp");
    QString cookieString = settings.value("network/cookies", "").toString();
    
    if (cookieString.isEmpty()) {
        qDebug() << "PersistentCookieJar - No saved cookies found";
        return;
    }
    
    QByteArray cookieData = QByteArray::fromBase64(cookieString.toUtf8());
    QList<QByteArray> cookieList = cookieData.split('\n');
    
    QList<QNetworkCookie> cookies;
    for (const QByteArray &cookieData : cookieList) {
        if (!cookieData.isEmpty()) {
            QList<QNetworkCookie> parsedCookies = QNetworkCookie::parseCookies(cookieData);
            cookies.append(parsedCookies);
        }
    }
    
    setAllCookies(cookies);
    qDebug() << "PersistentCookieJar - Loaded" << cookies.size() << "cookies";
}

// NetworkManager 实现
NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    // 设置自定义CookieJar，支持持久化存储
    PersistentCookieJar *cookieJar = new PersistentCookieJar(this);
    m_networkManager->setCookieJar(cookieJar);
    
    // 禁用代理，避免认证问题
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    m_networkManager->setProxy(QNetworkProxy::NoProxy);
    
    qDebug() << "NetworkManager - Initialized with persistent cookie jar";
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
    
    // 从QSettings获取Token并添加到请求头
    QString token = getToken();
    if (!token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
        qDebug() << "NetworkManager - Token added to request";
    }
    
    // 手动获取Cookie并添加到请求头
    QUrl requestUrl(url);
    PersistentCookieJar *cookieJar = qobject_cast<PersistentCookieJar*>(m_networkManager->cookieJar());
    if (cookieJar) {
        QList<QNetworkCookie> cookies = cookieJar->cookiesForUrl(requestUrl);
        if (!cookies.isEmpty()) {
            QByteArray cookieHeader;
            for (const QNetworkCookie &cookie : cookies) {
                if (!cookieHeader.isEmpty()) {
                    cookieHeader += "; ";
                }
                cookieHeader += cookie.toRawForm(QNetworkCookie::NameAndValueOnly);
            }
            request.setRawHeader("Cookie", cookieHeader);
            qDebug() << "NetworkManager - Cookie added to request:" << cookieHeader;
        } else {
            qDebug() << "NetworkManager - No cookies found for URL:" << url;
        }
    }
    
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
        
        // 读取并显示服务器的完整响应
        QByteArray responseData = reply->readAll();
        if (!responseData.isEmpty()) {
            qDebug() << "NetworkManager - Response Data:" << QString::fromUtf8(responseData);
        }
        
        if (errorCallback) {
            errorCallback(QString("%1 (HTTP %2)").arg(errorMsg).arg(statusCode));
        }
    }
    
    reply->deleteLater();
}

QString NetworkManager::getToken()
{
    QSettings settings("YourCompany", "QtApp");
    QString token = settings.value("user/token", "").toString();
    qDebug() << "NetworkManager - Token loaded:" << (token.isEmpty() ? "empty" : "exists");
    return token;
}

void NetworkManager::uploadFile(const QString &url,
                                const QString &filePath,
                                const QString &fileFieldName,
                                const SuccessCallback &successCallback,
                                const ErrorCallback &errorCallback)
{
    // 创建HTTP多部分请求
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    
    // 创建文件部分
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant("form-data; name=\"" + fileFieldName + "\"; filename=\"" +
                                QFileInfo(filePath).fileName() + "\""));
    
    // 根据文件扩展名设置Content-Type
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    QString contentType;
    
    if (suffix == "jpg" || suffix == "jpeg") {
        contentType = "image/jpeg";
    } else if (suffix == "png") {
        contentType = "image/png";
    } else if (suffix == "gif") {
        contentType = "image/gif";
    } else if (suffix == "bmp") {
        contentType = "image/bmp";
    } else {
        contentType = "application/octet-stream";
    }
    
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(contentType));
    
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        if (errorCallback) {
            errorCallback("无法打开文件: " + filePath);
        }
        delete multiPart;
        delete file;
        return;
    }
    
    // 验证文件大小
    qint64 fileSize = file->size();
    
    if (fileSize == 0) {
        if (errorCallback) {
            errorCallback("文件为空: " + filePath);
        }
        delete multiPart;
        delete file;
        return;
    }
    
    filePart.setBodyDevice(file);
    file->setParent(multiPart); // 确保文件在上传完成后被删除
    multiPart->append(filePart);
    
    // 创建请求（不要手动设置 Content-Type，让 Qt 自动处理）
    QNetworkRequest request{QUrl(url)};
    
    // 设置通用请求头（使用 */* 而不是 application/json，以匹配 curl 的行为）
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("User-Agent", "QtApp/1.0");
    
    // 添加平台识别头
    QString platform = getPlatform();
    request.setRawHeader("x-platform", platform.toUtf8());
    
    // 从QSettings获取Token并添加到请求头
    QString token = getToken();
    if (!token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
    }
    
    // 手动获取Cookie并添加到请求头
    QUrl requestUrl(url);
    PersistentCookieJar *cookieJar = qobject_cast<PersistentCookieJar*>(m_networkManager->cookieJar());
    if (cookieJar) {
        QList<QNetworkCookie> cookies = cookieJar->cookiesForUrl(requestUrl);
        if (!cookies.isEmpty()) {
            QByteArray cookieHeader;
            for (const QNetworkCookie &cookie : cookies) {
                if (!cookieHeader.isEmpty()) {
                    cookieHeader += "; ";
                }
                cookieHeader += cookie.toRawForm(QNetworkCookie::NameAndValueOnly);
            }
            request.setRawHeader("Cookie", cookieHeader);
        }
    }
    

    
    // 发送请求
    QNetworkReply *reply = m_networkManager->post(request, multiPart);
    
    // 设置多部分对象的父对象，确保在上传完成后被删除
    multiPart->setParent(reply);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, successCallback, errorCallback]() {
        handleResponse(reply, successCallback, errorCallback);
    });
}