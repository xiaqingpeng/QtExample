#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QQueue>
#include <QTimer>
#include <QSettings>

namespace Analytics {

// 事件类型枚举
class EventType {
public:
    static const QString VIEW;          // 页面浏览
    static const QString CLICK;         // 点击事件
    static const QString CUSTOM;        // 自定义事件
    static const QString ERROR;         // 错误事件
    static const QString PERFORMANCE;   // 性能事件
};

// 埋点事件结构
class Event {
public:
    QString eventName;      // 事件名称
    QString eventType;      // 事件类型
    QVariantMap properties; // 事件属性
    qint64 timestamp;       // 时间戳
    QString userId;         // 用户ID
    QString sessionId;      // 会话ID
    
    QJsonObject toJson() const;
};

// SDK配置
class Config {
public:
    QString serverUrl;      // 上报服务器地址
    QString appId;          // 应用ID
    bool enableDebug;       // 是否开启调试
    int batchSize;          // 批量上报数量
    int flushInterval;     // 上报间隔(秒)
    bool enablePersistence; // 是否开启持久化
    
    Config() : enableDebug(false), batchSize(10), flushInterval(30), enablePersistence(true) {}
};

// 埋点SDK核心类
class SDK : public QObject {
    Q_OBJECT
    
public:
    static SDK* instance();
    
    // 初始化SDK
    void initialize(const Config& config);
    
    // 设置用户ID
    void setUserId(const QString& userId);
    
    // 追踪事件
    void track(const QString& eventName, const QVariantMap& properties = QVariantMap());
    void track(const QString& eventType, const QString& eventName, const QVariantMap& properties = QVariantMap());
    
    // 页面浏览
    void trackView(const QString& pageName, const QVariantMap& properties = QVariantMap());
    
    // 点击事件
    void trackClick(const QString& elementName, const QVariantMap& properties = QVariantMap());
    
    // 错误事件
    void trackError(const QString& errorType, const QString& errorMessage, const QVariantMap& properties = QVariantMap());
    
    // 性能事件
    void trackPerformance(const QString& metricName, qint64 value, const QVariantMap& properties = QVariantMap());
    
    // 手动上报
    void flush();
    
    // 获取设备信息
    QVariantMap getDeviceInfo();
    
    // 页面浏览开始（用于计算停留时间）
    void trackViewStart(const QString& pageName);
    
    // 页面浏览结束（自动计算停留时间）
    void trackViewEnd(const QString& pageName, const QVariantMap& properties = QVariantMap());
    
private slots:
    void onFlushTimer();
    void onNetworkReply();
    
private:
    explicit SDK(QObject* parent = nullptr);
    ~SDK();
    
    void addToQueue(const Event& event);
    void sendEvents(const QList<Event>& events);
    void saveToDisk();
    void loadFromDisk();
    QString generateSessionId();
    
    // 页面浏览时间跟踪
    QMap<QString, qint64> m_pageViewStartTimes; // 页面名称 -> 开始时间戳
    
    static SDK* s_instance;
    
    Config m_config;
    QString m_userId;
    QString m_sessionId;
    QQueue<Event> m_eventQueue;
    QNetworkAccessManager* m_networkManager;
    QTimer* m_flushTimer;
    QSettings* m_settings;
};

} // namespace Analytics

#endif // ANALYTICS_H