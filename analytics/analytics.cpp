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

void SDK::initialize(const Config& config) {
    m_config = config;
    m_sessionId = generateSessionId();
    
    if (m_config.enablePersistence) {
        m_settings = new QSettings("Analytics", "SDK", this);
        loadFromDisk();
    }
    
    // 设置定时上报
    m_flushTimer->start(m_config.flushInterval * 1000);
    
    if (m_config.enableDebug) {
        qDebug() << "[Analytics] SDK initialized with appId:" << m_config.appId;
    }
}

void SDK::setUserId(const QString& userId) {
    m_userId = userId;
    if (m_config.enableDebug) {
        qDebug() << "[Analytics] User ID set:" << userId;
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
        qDebug() << "[Analytics] Track event:" << eventType << eventName;
    }
}

void SDK::trackView(const QString& pageName, const QVariantMap& properties) {
    QVariantMap props = properties;
    props["page_name"] = pageName;
    track(EventType::VIEW, "page_view", props);
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
        eventArray.append(event.toJson());
    }
    
    QJsonObject payload;
    payload["app_id"] = m_config.appId;
    payload["events"] = eventArray;
    payload["device_info"] = QJsonObject::fromVariantMap(getDeviceInfo());
    
    QJsonDocument doc(payload);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    QNetworkRequest request(m_config.serverUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply* reply = m_networkManager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, &SDK::onNetworkReply);
    
    if (m_config.enableDebug) {
        qDebug() << "[Analytics] Sending" << events.size() << "events to server";
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
        if (m_config.enableDebug) {
            qDebug() << "[Analytics] Events sent successfully";
        }
    } else {
        qWarning() << "[Analytics] Failed to send events:" << reply->errorString();
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