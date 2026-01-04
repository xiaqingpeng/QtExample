# Qt 埋点上报 SDK

一个轻量级、易用的Qt埋点上报SDK，支持事件追踪、批量上报、持久化存储等功能。

## 功能特性

- ✅ **多种事件类型**：支持页面浏览、点击、自定义、错误、性能等事件
- ✅ **批量上报**：支持批量上报，减少网络请求
- ✅ **定时上报**：支持定时自动上报
- ✅ **持久化存储**：支持本地持久化，应用重启后继续上报
- ✅ **设备信息采集**：自动采集设备、系统、屏幕等信息
- ✅ **会话管理**：自动生成和管理会话ID
- ✅ **调试模式**：支持调试日志输出
- ✅ **用户追踪**：支持用户ID设置和追踪

## 快速开始

### 1. 初始化SDK

```cpp
#include "analytics.h"

using namespace Analytics;

// 在main函数中初始化
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 配置SDK
    Config config;
    config.serverUrl = "https://your-analytics-server.com/api/events";
    config.appId = "your-app-id";
    config.enableDebug = true;        // 开发环境开启调试
    config.batchSize = 10;           // 批量上报数量
    config.flushInterval = 30;       // 30秒自动上报
    config.enablePersistence = true;  // 开启持久化
    
    // 初始化SDK
    SDK::instance()->initialize(config);
    
    // 设置用户ID
    SDK::instance()->setUserId("user_12345");
    
    return app.exec();
}
```

### 2. 追踪事件

#### 页面浏览

```cpp
SDK::instance()->trackView("user_info_page", {
    {"page_title", "个人中心"},
    {"referrer", "home_page"}
});
```

#### 点击事件

```cpp
SDK::instance()->trackClick("upload_avatar_button", {
    {"page", "user_info"},
    {"position", "header"}
});
```

#### 自定义事件

```cpp
SDK::instance()->track("avatar_upload", {
    {"file_size", 102400},
    {"file_type", "png"},
    {"success", true},
    {"duration", 2500}
});
```

#### 错误事件

```cpp
SDK::instance()->trackError("network", "上传头像失败", {
    {"error_code", "NETWORK_ERROR"},
    {"url", "https://api.example.com/user"},
    {"retry_count", 3}
});
```

#### 性能事件

```cpp
SDK::instance()->trackPerformance("page_load_time", 1200, {
    {"page", "user_info"},
    {"network_time", 300},
    {"render_time", 900}
});
```

### 3. 手动上报

```cpp
// 立即上报所有未发送的事件
SDK::instance()->flush();
```

## 配置说明

### Config 参数

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| serverUrl | QString | - | 上报服务器地址（必填） |
| appId | QString | - | 应用ID（必填） |
| enableDebug | bool | false | 是否开启调试模式 |
| batchSize | int | 10 | 批量上报数量 |
| flushInterval | int | 30 | 自动上报间隔（秒） |
| enablePersistence | bool | true | 是否开启持久化存储 |

### 事件类型

| 类型 | 常量 | 说明 |
|------|------|------|
| 页面浏览 | EventType::VIEW | 页面浏览事件 |
| 点击事件 | EventType::CLICK | 元素点击事件 |
| 自定义事件 | EventType::CUSTOM | 自定义业务事件 |
| 错误事件 | EventType::ERROR | 错误追踪事件 |
| 性能事件 | EventType::PERFORMANCE | 性能监控事件 |

## 数据格式

### 上报数据结构

```json
{
  "app_id": "your-app-id",
  "device_info": {
    "os_name": "macos",
    "os_version": "14.0",
    "cpu_architecture": "x86_64",
    "screen_width": 1920,
    "screen_height": 1080,
    "screen_dpr": 2.0,
    "app_version": "1.0.0",
    "qt_version": "6.5.0"
  },
  "events": [
    {
      "event_name": "page_view",
      "event_type": "view",
      "timestamp": 1699999999999,
      "user_id": "user_12345",
      "session_id": "uuid-string",
      "properties": {
        "page_name": "user_info_page",
        "page_title": "个人中心",
        "referrer": "home_page"
      }
    }
  ]
}
```

## 集成到项目

### CMakeLists.txt

```cmake
# 添加analytics模块
add_subdirectory(analytics)

# 链接到你的应用
target_link_libraries(your_app PRIVATE analytics)
``}

### .pro 文件

```qmake
# 添加analytics模块
SUBDIRS += analytics

# 链接到你的应用
LIBS += -L$$PWD/analytics -lanalytics
INCLUDEPATH += $$PWD/analytics
```

## 最佳实践

### 1. 页面埋点

在每个页面的构造函数中添加页面浏览埋点：

```cpp
UserInfoPage::UserInfoPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    
    // 页面浏览埋点
    SDK::instance()->trackView("user_info_page", {
        {"page_title", "个人中心"}
    });
}
```

### 2. 按钮点击埋点

在按钮的clicked槽函数中添加点击埋点：

```cpp
void UserInfoPage::onUploadAvatarClicked() {
    // 点击埋点
    SDK::instance()->trackClick("upload_avatar_button", {
        {"page", "user_info"}
    });
    
    // 业务逻辑...
}
```

### 3. 错误埋点

在错误处理代码中添加错误埋点：

```cpp
void UserInfoPage::handleError(const QString& error) {
    // 错误埋点
    SDK::instance()->trackError("network", error, {
        {"page", "user_info"},
        {"action", "upload_avatar"}
    });
    
    // 错误处理...
}
```

### 4. 性能埋点

在关键操作前后添加性能埋点：

```cpp
void UserInfoPage::loadUserInfo() {
    QElapsedTimer timer;
    timer.start();
    
    // 业务逻辑...
    
    // 性能埋点
    SDK::instance()->trackPerformance("load_user_info", timer.elapsed(), {
        {"page", "user_info"}
    });
}
```

### 5. 应用退出

在应用退出时确保所有事件都已上报：

```cpp
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 初始化SDK...
    
    int ret = app.exec();
    
    // 退出前上报剩余事件
    SDK::instance()->flush();
    
    return ret;
}
```

## 服务器端实现

### 接收接口示例（Node.js）

```javascript
const express = require('express');
const app = express();

app.use(express.json());

app.post('/api/events', (req, res) => {
    const { app_id, device_info, events } = req.body;
    
    // 保存到数据库
    events.forEach(event => {
        console.log(`[${app_id}] ${event.event_type}: ${event.event_name}`);
        // 保存到数据库...
    });
    
    res.json({ success: true, count: events.length });
});

app.listen(3000, () => {
    console.log('Analytics server running on port 3000');
});
```

## 注意事项

1. **隐私合规**：确保埋点数据收集符合GDPR、CCPA等隐私法规
2. **数据脱敏**：不要收集敏感信息（密码、身份证号等）
3. **性能影响**：埋点不应影响应用性能，建议异步上报
4. **网络异常**：SDK会自动重试失败的上报，无需手动处理
5. **调试模式**：生产环境务必关闭调试模式

## 常见问题

### Q: 如何确保数据上报成功？
A: SDK支持持久化存储，未上报成功的数据会保存在本地，下次启动时继续上报。

### Q: 如何减少网络请求？
A: 调整`batchSize`和`flushInterval`参数，增大批量数量和上报间隔。

### Q: 如何追踪匿名用户？
A: 不调用`setUserId()`即可，SDK会自动生成匿名用户ID。

### Q: 如何追踪用户会话？
A: SDK自动生成和管理会话ID，每次应用启动会生成新的会话。

## 许可证

MIT License