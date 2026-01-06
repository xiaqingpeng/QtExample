#include "serverconfigtab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QWebEngineSettings>
#include "theme_manager.h"
#include "theme_manager.h"

ServerConfigTab::ServerConfigTab(QWidget *parent)
    : QWidget(parent)
    , m_webView(nullptr)
    , m_channel(nullptr)
    , m_bridge(nullptr)
    , m_networkManager(new NetworkManager(this))
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    // 标题
    m_titleLabel = new QLabel("服务器配置", this);
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; padding: 10px 0;");
    mainLayout->addWidget(m_titleLabel);
    
    // 创建WebView用于显示图表
    qDebug() << "[ServerConfigTab] Creating WebView";
    m_webView = new QWebEngineView(this);
    m_webView->setObjectName("webView");
    
    // 配置WebEngineView设置
    QWebEngineSettings *settings = m_webView->settings();
    qDebug() << "[ServerConfigTab] Setting up WebEngine settings";
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebEngineSettings::ErrorPageEnabled, true);
    
    // 检查设置是否生效
    qDebug() << "[ServerConfigTab] JavaScriptEnabled:" << settings->testAttribute(QWebEngineSettings::JavascriptEnabled);
    qDebug() << "[ServerConfigTab] LocalContentCanAccessRemoteUrls:" << settings->testAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls);
    qDebug() << "[ServerConfigTab] LocalContentCanAccessFileUrls:" << settings->testAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls);
    
    // 连接所有相关信号，用于调试
    connect(m_webView->page(), &QWebEnginePage::loadStarted, this, [this]() {
        qDebug() << "[ServerConfigTab] Page load started:" << m_webView->url().toString();
    });
    
    connect(m_webView->page(), &QWebEnginePage::loadProgress, this, [](int progress) {
        qDebug() << "[ServerConfigTab] Page load progress:" << progress << "%";
    });
    
    connect(m_webView, &QWebEngineView::loadFinished, this, &ServerConfigTab::onPageLoaded);
    
    // 连接加载完成信号 (Qt 6)
    connect(m_webView->page(), &QWebEnginePage::loadFinished, this, [this](bool ok) {
        if (!ok) {
            qWarning() << "[ServerConfigTab] Page load failed";
        } else {
            qDebug() << "[ServerConfigTab] Page load succeeded";
        }
    });
    
    // 创建WebChannel用于Qt与JS通信
    qDebug() << "[ServerConfigTab] Creating WebChannel";
    m_channel = new QWebChannel(this);
    m_bridge = new ServerConfigBridge(this);
    m_channel->registerObject("qtBridge", m_bridge);
    
    mainLayout->addWidget(m_webView);
    
    // 使用包含WebChannel初始化的HTML内容
    qDebug() << "[ServerConfigTab] Loading HTML content with WebChannel support";
    QString htmlContent = R"HTML(
<html>
<head>
    <title>服务器配置</title>
    <script src="qrc:/qtwebchannel/qwebchannel.js"></script>
    <script>
        var qtBridge;
        
        // 初始化WebChannel
        new QWebChannel(qt.webChannelTransport, function(channel) {
            qtBridge = channel.objects.qtBridge;
            console.log("WebChannel initialized");
        });
        
        // 测试函数，用于更新图表
        function updateServerCharts(data) {
            console.log("Received data for charts:", data);
            document.getElementById('serverInfo').innerHTML = JSON.stringify(data, null, 2);
        }
    </script>
</head>
<body>
    <h1>服务器配置</h1>
    <p>页面加载成功！</p>
    <pre id="serverInfo"></pre>
</body>
</html>
)HTML";
    
    // 加载HTML内容
    qDebug() << "[ServerConfigTab] HTML内容长度:" << htmlContent.length();
    qDebug() << "[ServerConfigTab] 开始加载HTML内容...";
    m_webView->setHtml(htmlContent, QUrl("qrc:/html/serverconfig.html"));
    
    // 立即检查WebEngineView状态
    qDebug() << "[ServerConfigTab] WebView页面地址:" << m_webView->url().toString();
    qDebug() << "[ServerConfigTab] WebView标题:" << m_webView->title();
    
    // WebChannel将在页面加载完成后设置
    
    // 应用主题
    applyTheme();
    
    qDebug() << "[ServerConfigTab] Constructor finished";
}

ServerConfigTab::~ServerConfigTab()
{
}

void ServerConfigTab::onPageLoaded(bool ok){
    qDebug() << "[ServerConfigTab] ********************页面加载完成回调********************";
    qDebug() << "[ServerConfigTab] 加载状态:" << ok;
    qDebug() << "[ServerConfigTab] 页面标题:" << m_webView->title();
    qDebug() << "[ServerConfigTab] 页面URL:" << m_webView->url().toString();
    qDebug() << "[ServerConfigTab] WebView是否可见:" << m_webView->isVisible();
    qDebug() << "[ServerConfigTab] WebView尺寸:" << m_webView->size();
    
    // 检查WebChannel状态
    if (m_webView->page()->webChannel()) {
        qDebug() << "[ServerConfigTab] WebChannel is set up after page load";
    } else {
        qWarning() << "[ServerConfigTab] WebChannel is NOT set up after page load";
        // 再次尝试设置WebChannel
        m_webView->page()->setWebChannel(m_channel);
        if (m_webView->page()->webChannel()) {
            qDebug() << "[ServerConfigTab] WebChannel set up successfully after retry";
        } else {
            qCritical() << "[ServerConfigTab] Failed to set up WebChannel after retry";
        }
    }
    
    if (ok) {
        qDebug() << "[ServerConfigTab] Server config page loaded successfully";
        
        // 获取页面内容验证
        m_webView->page()->runJavaScript("document.body.innerHTML", [](const QVariant &result) {
            QString content = result.toString();
            qDebug() << "[ServerConfigTab] Page content loaded:" << content;
        });
        
        // 获取页面标题验证
        m_webView->page()->runJavaScript("document.title", [](const QVariant &result) {
            qDebug() << "[ServerConfigTab] Page title from JS:" << result.toString();
        });
        
        // 简单的测试JavaScript执行
        m_webView->page()->runJavaScript("'Hello from JavaScript: ' + (new Date()).toLocaleString()", [](const QVariant &result) {
            qDebug() << "[ServerConfigTab] JavaScript execution result:" << result.toString();
        });
        
        // 页面加载完成后获取系统信息
        qDebug() << "[ServerConfigTab] Fetching system info...";
        fetchSystemInfo();
    } else {
        qWarning() << "[ServerConfigTab] Failed to load server config page";
        
        // 尝试获取页面内容，查看是否有错误信息
        m_webView->page()->runJavaScript("document.body.innerHTML", [](const QVariant &result) {
            qWarning() << "[ServerConfigTab] Page content on error:" << result.toString();
        });
        
        // 尝试获取浏览器错误信息
        m_webView->page()->runJavaScript("window.navigator.userAgent", [](const QVariant &result) {
            qDebug() << "[ServerConfigTab] User Agent:" << result.toString();
        });
    }
}

void ServerConfigTab::fetchSystemInfo()
{
    qDebug() << "[ServerConfigTab] Fetching system info...";
    
    // 由于网络请求可能失败，我们先使用模拟数据
    QJsonObject mockData;
    mockData["serverIp"] = "192.168.1.100";
    mockData["os"] = "Ubuntu 20.04 LTS";
    mockData["platform"] = "x86_64";
    mockData["uptime"] = "2 days, 12 hours, 34 minutes";
    mockData["cpuUsage"] = 45;
    mockData["memoryUsage"] = 68;
    mockData["diskUsage"] = 72;
    
    qDebug() << "[ServerConfigTab] Using mock system info data:" << mockData;
    updateCharts(mockData);
    
    // 同时尝试真实的网络请求
    m_networkManager->get("/system/info", [this](const QJsonObject &response) {
        qDebug() << "[ServerConfigTab] Network response received:" << response;
        if (response["code"].toInt() == 0) {
            QJsonObject data = response["data"].toObject();
            qDebug() << "[ServerConfigTab] Real system info received:" << data;
            updateCharts(data);
        } else {
            qWarning() << "[ServerConfigTab] Failed to fetch real system info:" << response["msg"].toString();
        }
    }, [](const QString &error) {
        qWarning() << "[ServerConfigTab] Network error fetching system info:" << error;
    });
}

void ServerConfigTab::updateCharts(const QJsonObject &data)
{
    if (!m_webView || !m_webView->page()) {
        qWarning() << "[ServerConfigTab] WebView not available for updating charts";
        return;
    }
    
    QString jsonData = QJsonDocument(data).toJson(QJsonDocument::Compact);
    QString script = QString("updateServerCharts(%1);").arg(jsonData);
    
    qDebug() << "[ServerConfigTab] Running JavaScript to update charts:" << script.left(100) << "...";
    
    m_webView->page()->runJavaScript(script, [](const QVariant &result) {
        qDebug() << "[ServerConfigTab] JavaScript execution result:" << result;
    });
}

void ServerConfigTab::applyTheme()
{
    ThemeManager *theme = ThemeManager::instance();
    
    QString styleSheet = QString(
        "QWidget { "
        "    background-color: %1; "
        "    color: %2; "
        "    font-family: %3; "
        "}"
    ).arg(theme->colors().BACKGROUND)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY);
    
    setStyleSheet(styleSheet);
    
    if (m_titleLabel) {
        m_titleLabel->setStyleSheet(QString(
            "QLabel { "
            "    font-size: %1px; "
            "    font-weight: bold; "
            "    color: %2; "
            "    padding: 10px 0; "
            "}"
        ).arg(ThemeManager::Typography::FONT_SIZE_XXL)
         .arg(theme->colors().TEXT_PRIMARY));
    }
    
    if (m_webView) {
        m_webView->setStyleSheet(QString(
            "QWebEngineView { "
            "    border: 1px solid %1; "
            "    border-radius: 8px; "
            "    background-color: %2; "
            "}"
        ).arg(theme->colors().BORDER)
         .arg(theme->colors().BACKGROUND));
    }
}