#include "analytics.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSysInfo>
#include <QScreen>
#include <QGuiApplication>
#include <QUuid>
#include <QDateTime>
#include <QDebug>

namespace Analytics {

// 静态成员初始化
const QString EventType::VIEW = "view";
const QString EventType::CLICK = "click";
const QString EventType::CUSTOM = "custom";
const QString EventType::ERROR = "error";
const QString EventType::PERFORMANCE = "performance";

SDK* SDK::s_instance = nullptr;

// Event实现
QJsonObject Event::toJson() const {
    QJsonObject obj;
    obj["event_name"] = eventName;
    obj["event_type"] = eventType;
    obj["timestamp"] = timestamp;
    obj["user_id"] = userId;
    obj["session_id"] = sessionId;
    
    QJsonObject props;
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        props[it.key()] = QJsonValue::fromVariant(it.value());
    }
    obj["properties"] = props;
    
    return obj;
}

// SDK实现
SDK::SDK(QObject* parent) 
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_flushTimer(new QTimer(this))
    , m_settings(nullptr) {
    
    connect(m_flushTimer, &QTimer::timeout, this, &SDK::onFlushTimer);
}

SDK::~SDK() {
    if (m_settings) {
        delete m_settings;
    }
}

SDK* SDK::instance() {
    if (!s_instance) {
        s_instance = new SDK();
    }
    return s_instance;
}

void SDK::cleanup() {
    if (s_instance) {
        // 停止定时器，防止在对象销毁后继续触发
        if (s_instance->m_flushTimer) {
            s_instance->m_flushTimer->stop();
        }
        
        // 断开所有信号槽连接
        s_instance->disconnect();
        
        // 删除实例
        delete s_instance;
        s_instance = nullptr;
    }
}

void SDK::initialize(const Config& config) {
    m_config = config;
    m_sessionId = generateSessionId();
    
    // 初始化时设置临时匿名用户ID
    m_userId = "anonymous_" + generateSessionId();
    
    if (m_config.enablePersistence) {
        m_settings = new QSettings("Analytics", "SDK", this);
        loadFromDisk();
    }
    
    // 设置定时上报
    m_flushTimer->start(m_config.flushInterval * 1000);
    
    if (m_config.enableDebug) {
        // Analytics SDK已初始化
    }
}

void SDK::setUserId(const QString& userId) {
    m_userId = userId;
    if (m_config.enableDebug) {
        // 用户ID已设置
    }
}

void SDK::track(const QString& eventName, const QVariantMap& properties) {
    track(EventType::CUSTOM, eventName, properties);
}

void SDK::track(const QString& eventType, const QString& eventName, const QVariantMap& properties) {
    Event event;
    event.eventName = eventName;
    event.eventType = eventType;
    event.properties = properties;
    event.timestamp = QDateTime::currentMSecsSinceEpoch();
    event.userId = m_userId;
    event.sessionId = m_sessionId;
    
    // 添加设备信息
    event.properties["device_info"] = getDeviceInfo();
    
    addToQueue(event);
    
    if (m_config.enableDebug) {
        // 追踪事件
    }
}

void SDK::trackView(const QString& pageName, const QVariantMap& properties) {
    QVariantMap props = properties;
    props["page_name"] = pageName;
    track(EventType::VIEW, "page_view", props);
}

void SDK::trackViewStart(const QString& pageName) {
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    m_pageViewStartTimes[pageName] = startTime;
    
    if (m_config.enableDebug) {
        // 页面浏览开始
    }
}

void SDK::trackViewEnd(const QString& pageName, const QVariantMap& properties) {
    QVariantMap props = properties;
    props["page_name"] = pageName;
    
    // 计算页面停留时间
    qint64 duration = 0;
    if (m_pageViewStartTimes.contains(pageName)) {
        qint64 startTime = m_pageViewStartTimes[pageName];
        duration = QDateTime::currentMSecsSinceEpoch() - startTime;
        props["duration_ms"] = duration;
        m_pageViewStartTimes.remove(pageName);
    }
    
    track(EventType::VIEW, "page_view", props);
    
    if (m_config.enableDebug) {
        // 页面浏览结束
    }
}

void SDK::trackClick(const QString& elementName, const QVariantMap& properties) {
    QVariantMap props = properties;
    props["element_name"] = elementName;
    track(EventType::CLICK, "element_click", props);
}

void SDK::trackError(const QString& errorType, const QString& errorMessage, const QVariantMap& properties) {
    QVariantMap props = properties;
    props["error_type"] = errorType;
    props["error_message"] = errorMessage;
    track(EventType::ERROR, "error_occurred", props);
}

void SDK::trackPerformance(const QString& metricName, qint64 value, const QVariantMap& properties) {
    QVariantMap props = properties;
    props["metric_name"] = metricName;
    props["metric_value"] = value;
    // 添加duration_ms字段，以便后端API能够识别duration字段用于性能指标统计
    props["duration_ms"] = value;
    track(EventType::PERFORMANCE, "performance_metric", props);
}

void SDK::flush() {
    if (m_eventQueue.isEmpty()) {
        return;
    }
    
    QList<Event> eventsToSend;
    while (!m_eventQueue.isEmpty() && eventsToSend.size() < m_config.batchSize) {
        eventsToSend.append(m_eventQueue.dequeue());
    }
    
    sendEvents(eventsToSend);
}

void SDK::addToQueue(const Event& event) {
    m_eventQueue.enqueue(event);
    
    // 达到批量上报数量时立即上报
    if (m_eventQueue.size() >= m_config.batchSize) {
        flush();
    }
    
    // 持久化到磁盘
    if (m_config.enablePersistence) {
        saveToDisk();
    }
}

void SDK::sendEvents(const QList<Event>& events) {
    QJsonArray eventArray;
    for (const Event& event : events) {
        // 转换为后端API期望的格式
        QJsonObject eventObj;
        eventObj["event"] = event.eventName;
        eventObj["eventType"] = event.eventType;
        eventObj["properties"] = QJsonObject::fromVariantMap(event.properties);
        eventObj["userId"] = event.userId;
        eventObj["sessionId"] = event.sessionId;
        
        // 添加duration字段（如果properties中有duration_ms）
        if (event.properties.contains("duration_ms")) {
            eventObj["duration"] = event.properties["duration_ms"].toLongLong();
        }
        
        eventArray.append(eventObj);
    }
    
    QJsonObject payload;
    payload["events"] = eventArray;
    
    QJsonDocument doc(payload);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    // 使用批量事件接口
    QString batchUrl = m_config.serverUrl;
    if (batchUrl.endsWith("/events")) {
        batchUrl += "/batch";
    } else if (!batchUrl.endsWith("/events/batch")) {
        batchUrl += "/events/batch";
    }
    
    QNetworkRequest request(batchUrl);
    
    // 立即设置User-Agent，确保不被Qt网络库覆盖
    request.setRawHeader("User-Agent", "QtApp/1.0");
    request.setHeader(QNetworkRequest::UserAgentHeader, "QtApp/1.0");
    
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    if (m_config.enableDebug) {
        // 发送网络请求
    }
    
    QNetworkReply* reply = m_networkManager->post(request, data);
    
    // 使用lambda捕获reply对象，避免使用不安全的sender()
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 请求成功
        } else {
            qWarning() << "[Analytics] === Request Failed ===";
            qWarning() << "[Analytics] Failed to send events:" << reply->errorString();
            // 请求失败，记录错误详情
        }
        reply->deleteLater();
    });
    
    if (m_config.enableDebug) {
        // 发送事件到服务器
    }
}

void SDK::onFlushTimer() {
    if (!m_eventQueue.isEmpty()) {
        flush();
    }
}

void SDK::onNetworkReply() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }
    
    if (reply->error() == QNetworkReply::NoError) {
        // 事件发送成功
    } else {
        qWarning() << "[Analytics] Failed to send events:" << reply->errorString();
        // 事件发送失败
    }
    
    reply->deleteLater();
}

void SDK::saveToDisk() {
    if (!m_settings) {
        return;
    }
    
    QJsonArray eventArray;
    for (const Event& event : m_eventQueue) {
        eventArray.append(event.toJson());
    }
    
    m_settings->setValue("pending_events", eventArray);
    m_settings->sync();
}

void SDK::loadFromDisk() {
    if (!m_settings) {
        return;
    }
    
    QJsonArray eventArray = m_settings->value("pending_events").toJsonArray();
    
    for (const QJsonValue& value : eventArray) {
        QJsonObject obj = value.toObject();
        Event event;
        event.eventName = obj["event_name"].toString();
        event.eventType = obj["event_type"].toString();
        event.timestamp = obj["timestamp"].toVariant().toLongLong();
        event.userId = obj["user_id"].toString();
        event.sessionId = obj["session_id"].toString();
        
        QJsonObject props = obj["properties"].toObject();
        for (auto it = props.begin(); it != props.end(); ++it) {
            event.properties[it.key()] = it.value().toVariant();
        }
        
        m_eventQueue.enqueue(event);
    }
    
    m_settings->remove("pending_events");
}

QString SDK::generateSessionId() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QVariantMap SDK::getDeviceInfo() {
    QVariantMap deviceInfo;
    
    // 系统信息
    deviceInfo["os_name"] = QSysInfo::productType();
    deviceInfo["os_version"] = QSysInfo::productVersion();
    deviceInfo["kernel_version"] = QSysInfo::kernelVersion();
    deviceInfo["machine_name"] = QSysInfo::machineHostName();
    deviceInfo["cpu_architecture"] = QSysInfo::currentCpuArchitecture();
    
    // 屏幕信息
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QSize screenSize = screen->availableSize();
        deviceInfo["screen_width"] = screenSize.width();
        deviceInfo["screen_height"] = screenSize.height();
        deviceInfo["screen_dpr"] = screen->devicePixelRatio();
    }
    
    // 应用信息
    deviceInfo["app_version"] = qApp->applicationVersion();
    deviceInfo["qt_version"] = QT_VERSION_STR;
    
    return deviceInfo;
}

} // namespace Analytics