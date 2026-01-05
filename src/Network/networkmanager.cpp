#include "networkmanager.h"
#include <QDebug>
#include "common.h"
#include <QNetworkProxy>
#include <QUrl>
#include <QSettings>
#include <QBuffer>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFileInfo>
#include <QTimer>
#include <QTimer>

// PersistentCookieJar 实现
PersistentCookieJar::PersistentCookieJar(QObject *parent)
    : QNetworkCookieJar(parent)
{
    loadCookies();
}

QList<QNetworkCookie> PersistentCookieJar::cookiesForUrl(const QUrl &url) const
{
    QList<QNetworkCookie> cookies = QNetworkCookieJar::cookiesForUrl(url);
    // 获取指定URL的cookies
    return cookies;
}

bool PersistentCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
    // 修改Cookie的域名，使其适用于整个域名
    QList<QNetworkCookie> modifiedCookies;
    for (const QNetworkCookie &cookie : cookieList) {
        QNetworkCookie modifiedCookie = cookie;
        
        // 设置域名为整个域名（不包括端口）
        QString domain = url.host();
        modifiedCookie.setDomain(domain);
        
        // 设置路径为根路径，使其适用于所有路径
        modifiedCookie.setPath("/");
        
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
    // 保存cookies到持久化存储
}

void PersistentCookieJar::loadCookies()
{
    QSettings settings("YourCompany", "QtApp");
    QString cookieString = settings.value("network/cookies", "").toString();
    
    if (cookieString.isEmpty()) {
        // 没有保存的cookies
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
    // 从持久化存储加载cookies
}

// NetworkManager 实现
NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_baseUrl("http://120.48.95.51:7001")
    , m_timeout(30000)  // 默认30秒超时
    , m_maxRetries(3)   // 默认最多重试3次
{
    // 设置自定义CookieJar，支持持久化存储
    PersistentCookieJar *cookieJar = new PersistentCookieJar(this);
    m_networkManager->setCookieJar(cookieJar);
    
    // 禁用代理，避免认证问题
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    m_networkManager->setProxy(QNetworkProxy::NoProxy);
    
    // 初始化网络管理器
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
    
    // 立即设置User-Agent，确保不被Qt网络库覆盖
    request.setRawHeader("User-Agent", "QtApp/1.0");
    request.setHeader(QNetworkRequest::UserAgentHeader, "QtApp/1.0");
    
    // 设置Content-Type
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    
    // 添加平台识别头，让后端能够正确识别平台
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
        } else {
            // 没有找到该URL的cookies
        }
    }
    
    // 创建网络请求
    
    return request;
}

void NetworkManager::get(const QString &url,
                          const SuccessCallback &successCallback,
                          const ErrorCallback &errorCallback,
                          const QUrlQuery &queryParams)
{
    getWithRetry(url, successCallback, errorCallback, queryParams, 0);
}

void NetworkManager::getWithRetry(const QString &url,
                                   const SuccessCallback &successCallback,
                                   const ErrorCallback &errorCallback,
                                   const QUrlQuery &queryParams,
                                   int retryCount)
{
    QString fullUrl = buildFullUrl(url);
    if (!queryParams.isEmpty()) {
        fullUrl += "?" + queryParams.toString();
    }
    
    QNetworkRequest request = createRequest(fullUrl);
    
    // 打印GET请求信息（格式与POST保持一致）
    QString queryData = queryParams.isEmpty() ? "(无查询参数)" : queryParams.toString();
    LOG_DEBUG() << "请求URL:" << fullUrl << "请求的数据为:" << queryData;
    
    // 发送GET请求
    QNetworkReply *reply = m_networkManager->get(request);
    
    // 设置超时
    setRequestTimeout(reply, m_timeout);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, successCallback, errorCallback, url, queryParams, retryCount]() {
        // 检查是否是超时错误
        if (reply->error() == QNetworkReply::OperationCanceledError) {
            qWarning() << "NetworkManager - Request timed out for URL:" << url;
            
            // 如果还有重试次数，则重试
            if (retryCount < m_maxRetries) {
                qWarning() << "NetworkManager - Retrying... (" << (retryCount + 1) << "/" << m_maxRetries << ")";
                reply->deleteLater();
                
                // 延迟重试（指数退避）
                QTimer::singleShot(1000 * (retryCount + 1), this, [this, url, successCallback, errorCallback, queryParams, retryCount]() {
                    getWithRetry(url, successCallback, errorCallback, queryParams, retryCount + 1);
                });
                return;
            } else {
                qWarning() << "NetworkManager - Max retries reached, giving up";
                if (errorCallback) {
                    errorCallback(QString("Request timed out after %1 retries").arg(m_maxRetries));
                }
                reply->deleteLater();
                return;
            }
        }
        
        handleResponse(reply, successCallback, errorCallback);
    });
}

void NetworkManager::setRequestTimeout(QNetworkReply *reply, int timeoutMs)
{
    // 使用QTimer实现超时机制
    QTimer *timer = new QTimer(reply);
    timer->setSingleShot(true);
    
    connect(timer, &QTimer::timeout, this, [reply, timer]() {
        if (reply && reply->isRunning()) {
            qWarning() << "NetworkManager - Request timeout, aborting...";
            reply->abort();
        }
        timer->deleteLater();
    });
    
    timer->start(timeoutMs);
}

void NetworkManager::setBaseUrl(const QString &baseUrl)
{
    m_baseUrl = baseUrl;
}

QString NetworkManager::buildFullUrl(const QString &path)
{
    QString fullPath = path;
    
    // 如果path不是以http://或https://开头，则拼接基础URL
    if (!path.startsWith("http://") && !path.startsWith("https://")) {
        fullPath = m_baseUrl;
        if (!path.startsWith("/")) {
            fullPath += "/";
        }
        fullPath += path;
    }
    
    return fullPath;
}

// 用户画像API实现
void NetworkManager::getUserProfile(const QString &userId,
                                    const SuccessCallback &successCallback,
                                    const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    get("/api/analytics/user/profile", successCallback, errorCallback, queryParams);
}

void NetworkManager::getUserTags(const QString &userId,
                                 const SuccessCallback &successCallback,
                                 const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    get("/api/analytics/user/tags", successCallback, errorCallback, queryParams);
}

void NetworkManager::getUserBehaviorStats(const QString &userId,
                                          const SuccessCallback &successCallback,
                                          const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    get("/api/analytics/user/behavior", successCallback, errorCallback, queryParams);
}

void NetworkManager::getUserInterestProfile(const QString &userId,
                                             const SuccessCallback &successCallback,
                                             const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    get("/api/analytics/user/interest", successCallback, errorCallback, queryParams);
}

void NetworkManager::getUserValueAssessment(const QString &userId,
                                             const SuccessCallback &successCallback,
                                             const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("userId", userId);
    get("/api/analytics/user/value", successCallback, errorCallback, queryParams);
}

void NetworkManager::updateUserProfile(const QString &userId,
                                        const QJsonObject &data,
                                        const SuccessCallback &successCallback,
                                        const ErrorCallback &errorCallback)
{
    QString url = "/api/user-profile/update/" + userId;
    post(url, data, successCallback, errorCallback);
}

// 统计报表API实现
void NetworkManager::getActivityStats(const QString &startDate, const QString &endDate,
                                     const SuccessCallback &successCallback,
                                     const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    
    get("/api/analytics/activity", successCallback, errorCallback, queryParams);
}

void NetworkManager::getRetentionStats(const SuccessCallback &successCallback,
                                        const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("days", "7");  // 默认计算7日留存率
    
    get("/api/analytics/retention", successCallback, errorCallback, queryParams);
}

void NetworkManager::getPageViewStats(const QString &startDate, const QString &endDate,
                                      const SuccessCallback &successCallback,
                                      const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    
    get("/api/analytics/page-views", successCallback, errorCallback, queryParams);
}

void NetworkManager::getEventStats(const QString &startDate, const QString &endDate,
                                   const SuccessCallback &successCallback,
                                   const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    
    get("/api/analytics/event-stats", successCallback, errorCallback, queryParams);
}

void NetworkManager::getTrendAnalysis(const QString &metric, const QString &startDate, const QString &endDate,
                                     const SuccessCallback &successCallback,
                                     const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    queryParams.addQueryItem("metric", metric);
    queryParams.addQueryItem("interval", "day"); // 默认按天分组
    
    get("/api/analytics/trends", successCallback, errorCallback, queryParams);
}

void NetworkManager::getTopPages(const QString &startDate, const QString &endDate, int limit,
                                 const SuccessCallback &successCallback,
                                 const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    queryParams.addQueryItem("limit", QString::number(limit));
    
    get("/api/analytics/page-views", successCallback, errorCallback, queryParams);
}

void NetworkManager::getTopEvents(const QString &startDate, const QString &endDate, int limit,
                                  const SuccessCallback &successCallback,
                                  const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", startDate);
    queryParams.addQueryItem("endDate", endDate);
    queryParams.addQueryItem("limit", QString::number(limit));
    
    get("/api/analytics/event-stats", successCallback, errorCallback, queryParams);
}

void NetworkManager::getTopUsers(int page, int pageSize,
                                 const SuccessCallback &successCallback,
                                 const ErrorCallback &errorCallback)
{
    QUrlQuery queryParams;
    queryParams.addQueryItem("page", QString::number(page));
    queryParams.addQueryItem("pageSize", QString::number(pageSize));
    
    get("/api/analytics/user/list", successCallback, errorCallback, queryParams);
}

void NetworkManager::getRealTimeStats(const SuccessCallback &successCallback,
                                     const ErrorCallback &errorCallback)
{
    // 获取今日事件总数和独立用户数
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QUrlQuery queryParams;
    queryParams.addQueryItem("startDate", today);
    queryParams.addQueryItem("endDate", today);
    queryParams.addQueryItem("interval", "day");
    
    get("/api/analytics/trends", successCallback, errorCallback, queryParams);
}

void NetworkManager::post(const QString &url,
                           const QJsonObject &data,
                           const SuccessCallback &successCallback,
                           const ErrorCallback &errorCallback)
{
    QString fullUrl = buildFullUrl(url);
    QNetworkRequest request = createRequest(fullUrl);
    
    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();
    
    LOG_DEBUG() << "请求URL:" << fullUrl << "请求的数据为:" << doc.toJson(QJsonDocument::Compact);
    
    
    QNetworkReply *reply = m_networkManager->post(request, jsonData);
    
    // 设置超时
    setRequestTimeout(reply, m_timeout);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, successCallback, errorCallback]() {
        handleResponse(reply, successCallback, errorCallback);
    });
}

void NetworkManager::put(const QString &url,
                          const QJsonObject &data,
                          const SuccessCallback &successCallback,
                          const ErrorCallback &errorCallback)
{
    QString fullUrl = buildFullUrl(url);
    QNetworkRequest request = createRequest(fullUrl);
    
    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();
    
    LOG_DEBUG() << "请求URL:" << fullUrl << "请求的数据为:" << doc.toJson(QJsonDocument::Compact);
   
    
    QNetworkReply *reply = m_networkManager->put(request, jsonData);
    
    // 设置超时
    setRequestTimeout(reply, m_timeout);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, successCallback, errorCallback]() {
        handleResponse(reply, successCallback, errorCallback);
    });
}

void NetworkManager::deleteResource(const QString &url,
                                      const SuccessCallback &successCallback,
                                      const ErrorCallback &errorCallback)
{
    QString fullUrl = buildFullUrl(url);
    QNetworkRequest request = createRequest(fullUrl);
    
    LOG_DEBUG() << "NetworkManager - DELETE request:" << fullUrl;
    
    QNetworkReply *reply = m_networkManager->deleteResource(request);
    
    // 设置超时
    setRequestTimeout(reply, m_timeout);
    
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
        
        // 获取请求的URL
        QString requestUrl = reply->request().url().toString();
        LOG_DEBUG() << "请求URL:" << requestUrl << "请求响应数据:" << doc.toJson(QJsonDocument::Compact);
        if (doc.isObject()) {
            QJsonObject rootObj = doc.object();
            if (successCallback) {
                successCallback(rootObj);
            }
        } else {
            qWarning() << "NetworkManager - Invalid JSON response";
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
        }
    } else {
        QString errorMsg = reply->errorString();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        
        qWarning() << "NetworkManager - Error:" << errorMsg << "HTTP Status:" << statusCode;
        
        // 读取并显示服务器的完整响应
        QByteArray responseData = reply->readAll();
        if (!responseData.isEmpty()) {
            qWarning() << "NetworkManager - Response Data:" << QString::fromUtf8(responseData);
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
    
    // 设置超时（文件上传可能需要更长时间，使用2倍超时时间）
    setRequestTimeout(reply, m_timeout * 2);
    
    // 设置多部分对象的父对象，确保在上传完成后被删除
    multiPart->setParent(reply);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, successCallback, errorCallback]() {
        handleResponse(reply, successCallback, errorCallback);
    });
}
