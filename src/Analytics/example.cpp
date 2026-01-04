#include "analytics.h"
#include <QDebug>

using namespace Analytics;

// 初始化SDK示例
void initializeAnalytics() {
    Config config;
    config.serverUrl = "https://your-analytics-server.com/api/events";
    config.appId = "your-app-id";
    config.enableDebug = true;        // 开发环境开启调试
    config.batchSize = 10;           // 批量上报数量
    config.flushInterval = 30;       // 30秒自动上报
    config.enablePersistence = true;  // 开启持久化
    
    SDK::instance()->initialize(config);
    
    // 设置用户ID
    SDK::instance()->setUserId("user_12345");
}

// 页面浏览埋点示例
void trackPageView(const QString& pageName) {
    QVariantMap properties;
    properties["page_title"] = "个人中心";
    properties["referrer"] = "home_page";
    properties["duration"] = 5000; // 页面停留时间(ms)
    
    SDK::instance()->trackView(pageName, properties);
}

// 按钮点击埋点示例
void trackButtonClick(const QString& buttonName) {
    QVariantMap properties;
    properties["page"] = "user_info";
    properties["position"] = "header";
    properties["button_type"] = "primary";
    
    SDK::instance()->trackClick(buttonName, properties);
}

// 自定义事件埋点示例
void trackCustomEvent() {
    QVariantMap properties;
    properties["action"] = "upload_avatar";
    properties["file_size"] = 102400;
    properties["file_type"] = "png";
    properties["success"] = true;
    properties["duration"] = 2500; // 上传耗时(ms)
    
    SDK::instance()->track("avatar_upload", properties);
}

// 错误埋点示例
void trackError(const QString& errorMessage) {
    QVariantMap properties;
    properties["error_code"] = "NETWORK_ERROR";
    properties["error_detail"] = errorMessage;
    properties["url"] = "https://api.example.com/user";
    properties["retry_count"] = 3;
    
    SDK::instance()->trackError("network", errorMessage, properties);
}

// 性能埋点示例
void trackPerformance() {
    // 页面加载时间
    SDK::instance()->trackPerformance("page_load_time", 1200, {
        {"page", "user_info"},
        {"network_time", 300},
        {"render_time", 900}
    });
    
    // API响应时间
    SDK::instance()->trackPerformance("api_response_time", 450, {
        {"api_endpoint", "/api/user/info"},
        {"method", "GET"}
    });
    
    // 内存使用
    SDK::instance()->trackPerformance("memory_usage", 102400, {
        {"unit", "KB"},
        {"page", "user_info"}
    });
}

// 在UserInfoPage中的实际使用示例
void UserInfoPage::setupAnalytics() {
    // 页面加载埋点
    trackPageView("user_info_page");
}

void UserInfoPage::onUploadAvatarClicked() {
    // 按钮点击埋点
    trackButtonClick("upload_avatar_button");
    
    // ... 上传逻辑 ...
    
    // 上传成功埋点
    if (uploadSuccess) {
        trackCustomEvent();
    } else {
        trackError("上传头像失败");
    }
}

// 手动上报示例
void forceFlush() {
    SDK::instance()->flush();
}

// 应用退出时上报剩余事件
void cleanupAnalytics() {
    SDK::instance()->flush();
}