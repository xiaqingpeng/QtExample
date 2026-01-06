#include "contenttab.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QWebEngineSettings>
#include <QProcess>
#include <QRegularExpression>
#include "theme_manager.h"
#include "serverconfigtab.h"

ContentTab::ContentTab(QWidget *parent)
    : QWidget(parent)
    , m_webView(nullptr)
    , m_channel(nullptr)
    , m_bridge(nullptr)
    , m_networkManager(new NetworkManager(this))
    , m_titleLabel(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    // 创建WebView用于显示图表
    qDebug() << "[ContentTab] Creating WebView";
    m_webView = new QWebEngineView(this);
    m_webView->setObjectName("webView");
    
    // 配置WebEngineView设置
    QWebEngineSettings *settings = m_webView->settings();
    qDebug() << "[ContentTab] Setting up WebEngine settings";
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebEngineSettings::ErrorPageEnabled, true);
    
    // 检查设置是否生效
    qDebug() << "[ContentTab] JavaScriptEnabled:" << settings->testAttribute(QWebEngineSettings::JavascriptEnabled);
    qDebug() << "[ContentTab] LocalContentCanAccessRemoteUrls:" << settings->testAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls);
    qDebug() << "[ContentTab] LocalContentCanAccessFileUrls:" << settings->testAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls);
    
    // 连接所有相关信号，用于调试
    connect(m_webView->page(), &QWebEnginePage::loadStarted, this, [this]() {
        qDebug() << "[ContentTab] Page load started:" << m_webView->url().toString();
    });
    
    connect(m_webView->page(), &QWebEnginePage::loadProgress, this, [](int progress) {
        qDebug() << "[ContentTab] Page load progress:" << progress << "%";
    });
    
    connect(m_webView, &QWebEngineView::loadFinished, this, &ContentTab::onPageLoaded);
    
    // 连接加载完成信号 (Qt 6)
    connect(m_webView->page(), &QWebEnginePage::loadFinished, this, [this](bool ok) {
        if (!ok) {
            qWarning() << "[ContentTab] Page load failed";
        } else {
            qDebug() << "[ContentTab] Page load succeeded";
        }
    });
    
    // 创建WebChannel用于Qt与JS通信
    qDebug() << "[ContentTab] Creating WebChannel";
    m_channel = new QWebChannel(this);
    m_bridge = new ServerConfigBridge(this);
    m_channel->registerObject("qtBridge", m_bridge);
    
    mainLayout->addWidget(m_webView);
    
    // 使用包含WebChannel初始化的HTML内容
    qDebug() << "[ContentTab] Loading HTML content with WebChannel support";
    QString htmlContent = R"HTML(
<html>
<head>
    <title>服务器配置</title>
    <script src="qrc:/qtwebchannel/qwebchannel.js"></script>
    <script src="qrc:/src/ECharts/echarts.min.js"></script>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body {
            margin: 10px;
            font-family: Arial, sans-serif;
            background-color: #f5f5f5;
            min-height: 0;
        }
        h1 {
            text-align: center;
            color: #333;
            margin: 10px 0;
            font-size: 24px;
        }
        .main-container {
            display: grid;
            grid-template-rows: auto auto;
            gap: 10px;
            max-height: calc(100vh - 40px);
            box-sizing: border-box;
        }
        .system-info {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 10px;
            padding: 15px;
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 8px;
            margin-bottom: 10px;
        }
        .info-item {
            display: flex;
            flex-direction: column;
        }
        .info-label {
            font-weight: bold;
            color: #555;
            font-size: 14px;
            margin-bottom: 3px;
        }
        .info-value {
            color: #333;
            font-size: 16px;
        }
        .charts-container {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 10px;
            overflow: hidden;
        }
        .chart-container {
            width: 100%;
            height: 280px;
            border: 1px solid #ddd;
            border-radius: 8px;
            background-color: white;
        }
        /* 响应式设计 */
        @media (max-width: 768px) {
            .main-container {
                max-height: none;
            }
            .system-info {
                grid-template-columns: 1fr;
            }
            .charts-container {
                grid-template-columns: 1fr;
            }
            .chart-container {
                height: 250px;
            }
        }
    </style>
    <script>
        var qtBridge;
        var cpuChart = null;
        var memoryChart = null;
        var diskChart = null;
        var chartsInitialized = false;
        
        // 检查ECharts库是否加载
        if (typeof echarts === 'undefined') {
            console.error('ECharts library not loaded!');
        }
        
        // 初始化WebChannel
        if (typeof qt !== 'undefined' && qt.webChannelTransport) {
            new QWebChannel(qt.webChannelTransport, function(channel) {
                qtBridge = channel.objects.qtBridge;
                console.log("WebChannel initialized");
            });
        } else {
            console.error('Qt WebChannel not available!');
        }
        
        // 页面加载完成后初始化图表
        window.onload = function() {
            console.log('Page loaded, initializing charts...');
            initCharts();
        };
        
        // 初始化图表配置
        function initCharts() {
            if (typeof echarts === 'undefined') {
                console.error('Cannot initialize charts: ECharts not loaded');
                return;
            }
            
            try {
                // 检查DOM元素是否存在
                var cpuElement = document.getElementById('cpuChart');
                var memoryElement = document.getElementById('memoryChart');
                var diskElement = document.getElementById('diskChart');
                
                if (!cpuElement || !memoryElement || !diskElement) {
                    console.error('Chart container elements not found');
                    return;
                }
                
                // 初始化图表
                cpuChart = echarts.init(cpuElement);
                memoryChart = echarts.init(memoryElement);
                diskChart = echarts.init(diskElement);
                
                console.log('Charts initialized successfully');
                
                // 设置初始图表配置
                setInitialChartOptions();
                chartsInitialized = true;
            } catch (error) {
                console.error('Error initializing charts:', error);
            }
        }
        
        // 设置初始图表选项
        function setInitialChartOptions() {
            // CPU使用率图表
            if (cpuChart) {
                cpuChart.setOption({
                    title: {
                        text: 'CPU使用率',
                        left: 'center'
                    },
                    tooltip: {
                        trigger: 'item',
                        formatter: '{a} <br/>{b}: {c}%'
                    },
                    series: [{
                        name: 'CPU',
                        type: 'gauge',
                        detail: { formatter: '{value}%' },
                        data: [{ value: 0, name: '使用率' }]
                    }]
                });
            }
            
            // 内存使用率图表
            if (memoryChart) {
                memoryChart.setOption({
                    title: {
                        text: '内存使用率',
                        left: 'center'
                    },
                    tooltip: {
                        trigger: 'item',
                        formatter: '{a} <br/>{b}: {c}%'
                    },
                    series: [{
                        name: '内存',
                        type: 'gauge',
                        detail: { formatter: '{value}%' },
                        data: [{ value: 0, name: '使用率' }]
                    }]
                });
            }
            
            // 磁盘使用率图表
            if (diskChart) {
                diskChart.setOption({
                    title: {
                        text: '磁盘使用率',
                        left: 'center'
                    },
                    tooltip: {
                        trigger: 'item',
                        formatter: '{a} <br/>{b}: {c}%'
                    },
                    series: [{
                        name: '磁盘',
                        type: 'gauge',
                        detail: { formatter: '{value}%' },
                        data: [{ value: 0, name: '使用率' }]
                    }]
                });
            }
        }
        
        // 更新服务器监控图表
        function updateServerCharts(data) {
            console.log("Received data for charts:", data);
            
            // 更新系统信息
            if (document.getElementById('serverIp')) {
                document.getElementById('serverIp').textContent = data.ip_address || '未知';
            }
            if (document.getElementById('osInfo')) {
                document.getElementById('osInfo').textContent = data.os_info || data.os || '未知';
            }
            if (document.getElementById('uptime')) {
                document.getElementById('uptime').textContent = data.uptime_days ? data.uptime_days + '天' : data.uptime || '未知';
            }
            
            // 确保图表已初始化
            if (!chartsInitialized) {
                console.log('Charts not initialized yet, trying to initialize now...');
                initCharts();
                if (!chartsInitialized) {
                    console.error('Cannot update charts: charts not initialized');
                    return;
                }
            }
            
            // 更新CPU使用率图表
            var cpuUsage = data.cpu_usage || data.cpuUsage || 0;
            if (cpuChart) {
                cpuChart.setOption({
                    series: [{
                        data: [{ value: cpuUsage, name: '使用率' }]
                    }]
                });
                console.log('CPU chart updated with value:', cpuUsage);
            } else {
                console.error('CPU chart object is null');
            }
            
            // 更新内存使用率图表
            var memoryUsage = data.mem_usage || data.memoryUsage || 0;
            if (memoryChart) {
                memoryChart.setOption({
                    series: [{
                        data: [{ value: memoryUsage, name: '使用率' }]
                    }]
                });
                console.log('Memory chart updated with value:', memoryUsage);
            } else {
                console.error('Memory chart object is null');
            }
            
            // 更新磁盘使用率图表
            var diskUsage = data.disk_usage || data.diskUsage || 0;
            if (diskChart) {
                diskChart.setOption({
                    series: [{
                        data: [{ value: diskUsage, name: '使用率' }]
                    }]
                });
                console.log('Disk chart updated with value:', diskUsage);
            } else {
                console.error('Disk chart object is null');
            }
        }
    </script>
</head>
<body>
    <h1>电脑本机配置监控</h1>
    
    <div class="main-container">
        <div class="system-info">
            <div class="info-item">
                <div class="info-label">服务器IP</div>
                <div id="serverIp" class="info-value">未知</div>
            </div>
            <div class="info-item">
                <div class="info-label">操作系统</div>
                <div id="osInfo" class="info-value">未知</div>
            </div>
            <div class="info-item">
                <div class="info-label">运行时间</div>
                <div id="uptime" class="info-value">未知</div>
            </div>
        </div>
        
        <div class="charts-container">
            <div class="chart-container" id="cpuChart"></div>
            <div class="chart-container" id="memoryChart"></div>
            <div class="chart-container" id="diskChart"></div>
        </div>
    </div>
</body>
</html>
)HTML";
    
    // 加载HTML内容
    qDebug() << "[ContentTab] HTML内容长度:" << htmlContent.length();
    qDebug() << "[ContentTab] 开始加载HTML内容...";
    m_webView->setHtml(htmlContent, QUrl("qrc:/html/serverconfig.html"));
    
    // 立即检查WebEngineView状态
    qDebug() << "[ContentTab] WebView页面地址:" << m_webView->url().toString();
    qDebug() << "[ContentTab] WebView标题:" << m_webView->title();
    
    // WebChannel将在页面加载完成后设置
    
    // 应用主题
    applyTheme();
    
    qDebug() << "[ContentTab] Constructor finished";
}

ContentTab::~ContentTab()
{
}

void ContentTab::onPageLoaded(bool ok){
    qDebug() << "[ContentTab] ********************页面加载完成回调********************";
    qDebug() << "[ContentTab] 加载状态:" << ok;
    qDebug() << "[ContentTab] 页面标题:" << m_webView->title();
    qDebug() << "[ContentTab] 页面URL:" << m_webView->url().toString();
    qDebug() << "[ContentTab] WebView是否可见:" << m_webView->isVisible();
    qDebug() << "[ContentTab] WebView尺寸:" << m_webView->size();
    
    // 检查WebChannel状态
    if (m_webView->page()->webChannel()) {
        qDebug() << "[ContentTab] WebChannel is set up after page load";
    } else {
        qWarning() << "[ContentTab] WebChannel is NOT set up after page load";
        // 再次尝试设置WebChannel
        m_webView->page()->setWebChannel(m_channel);
        if (m_webView->page()->webChannel()) {
            qDebug() << "[ContentTab] WebChannel set up successfully after retry";
        } else {
            qCritical() << "[ContentTab] Failed to set up WebChannel after retry";
        }
    }
    
    if (ok) {
        qDebug() << "[ContentTab] Server config page loaded successfully";
        
        // 获取页面内容验证
        m_webView->page()->runJavaScript("document.body.innerHTML", [](const QVariant &result) {
            QString content = result.toString();
            qDebug() << "[ContentTab] Page content loaded:" << content;
        });
        
        // 获取页面标题验证
        m_webView->page()->runJavaScript("document.title", [](const QVariant &result) {
            qDebug() << "[ContentTab] Page title from JS:" << result.toString();
        });
        
        // 简单的测试JavaScript执行
        m_webView->page()->runJavaScript("'Hello from JavaScript: ' + (new Date()).toLocaleString()", [](const QVariant &result) {
            qDebug() << "[ContentTab] JavaScript execution result:" << result.toString();
        });
        
        // 页面加载完成后获取系统信息
        qDebug() << "[ContentTab] Fetching system info...";
        fetchSystemInfo();
    } else {
        qWarning() << "[ContentTab] Failed to load server config page";
        
        // 尝试获取页面内容，查看是否有错误信息
        m_webView->page()->runJavaScript("document.body.innerHTML", [](const QVariant &result) {
            qWarning() << "[ContentTab] Page content on error:" << result.toString();
        });
        
        // 尝试获取浏览器错误信息
        m_webView->page()->runJavaScript("window.navigator.userAgent", [](const QVariant &result) {
            qDebug() << "[ContentTab] User Agent:" << result.toString();
        });
    }
}

// 获取本地系统信息
void ContentTab::fetchSystemInfo()
{
    qDebug() << "[ContentTab] Fetching local system info...";
    
    QJsonObject systemInfo;
    
    // 获取CPU使用率
    QProcess cpuProcess;
    cpuProcess.start("sh", QStringList() << "-c" << "top -l 1 | grep 'CPU usage' | awk '{print $3}' | sed 's/%//'");
    cpuProcess.waitForFinished();
    QString cpuUsage = cpuProcess.readAllStandardOutput().trimmed();
    systemInfo["cpu_usage"] = cpuUsage.toDouble();
    
    // 获取内存信息
    QProcess memProcess;
    memProcess.start("sh", QStringList() << "-c" << "top -l 1 | grep 'PhysMem' | awk '{print $2, $4, $6}'");
    memProcess.waitForFinished();
    QString memInfo = memProcess.readAllStandardOutput().trimmed();
    QStringList memParts = memInfo.split(" ");
    if (memParts.size() >= 3) {
        double memUsed = memParts[0].replace("M", "").toDouble();
        double memFree = memParts[2].replace("M", "").toDouble();
        double memTotal = memUsed + memFree;
        systemInfo["mem_total"] = memTotal;
        systemInfo["mem_used"] = memUsed;
        systemInfo["mem_usage"] = (memUsed / memTotal) * 100;
    }
    
    // 获取磁盘信息
    QProcess diskProcess;
    diskProcess.start("sh", QStringList() << "-c" << "df -h / | tail -1 | awk '{print $2, $3, $5}'");
    diskProcess.waitForFinished();
    QString diskInfo = diskProcess.readAllStandardOutput().trimmed();
    QStringList diskParts = diskInfo.split(" ");
    if (diskParts.size() >= 3) {
        systemInfo["disk_total"] = diskParts[0].replace("G", "").toDouble();
        systemInfo["disk_used"] = diskParts[1].replace("G", "").toDouble();
        systemInfo["disk_usage"] = diskParts[2].replace("%", "").toDouble();
    }
    
    // 获取负载信息
    QProcess loadProcess;
    loadProcess.start("sh", QStringList() << "-c" << "uptime | awk -F'load averages: ' '{print $2}'");
    loadProcess.waitForFinished();
    QString loadInfo = loadProcess.readAllStandardOutput().trimmed();
    QStringList loadParts = loadInfo.split(", ");
    if (loadParts.size() >= 3) {
        systemInfo["load_1"] = loadParts[0].toDouble();
        systemInfo["load_5"] = loadParts[1].toDouble();
        systemInfo["load_15"] = loadParts[2].toDouble();
    }
    
    // 获取运行时间
    QProcess uptimeProcess;
    uptimeProcess.start("sh", QStringList() << "-c" << "uptime | awk -F'up ' '{print $2}' | awk -F', ' '{print $1}'");
    uptimeProcess.waitForFinished();
    QString uptimeInfo = uptimeProcess.readAllStandardOutput().trimmed();
    // 简单处理运行时间（这里只取天数部分）
    QRegularExpression regex("(\\d+) days?");
    QRegularExpressionMatch match = regex.match(uptimeInfo);
    if (match.hasMatch()) {
        systemInfo["uptime_days"] = match.captured(1).toDouble();
    } else {
        systemInfo["uptime_days"] = 0.0;
    }
    
    // 获取网络信息
    QProcess ipProcess;
    ipProcess.start("sh", QStringList() << "-c" << "ifconfig en0 | grep 'inet ' | awk '{print $2}'");
    ipProcess.waitForFinished();
    QString ipAddress = ipProcess.readAllStandardOutput().trimmed();
    systemInfo["ip_address"] = ipAddress;
    
    // 获取操作系统信息
    QProcess osProcess;
    osProcess.start("sh", QStringList() << "-c" << "sw_vers -productName && sw_vers -productVersion");
    osProcess.waitForFinished();
    QString osInfo = osProcess.readAllStandardOutput().trimmed();
    QStringList osParts = osInfo.split("\n");
    if (osParts.size() >= 2) {
        systemInfo["os_info"] = osParts[0] + " " + osParts[1];
    }
    
    // 平台信息
    systemInfo["platform"] = "darwin";
    
    qDebug() << "[ContentTab] Local system info:" << systemInfo;
    updateCharts(systemInfo);
}

void ContentTab::updateCharts(const QJsonObject &data)
{
    if (!m_webView || !m_webView->page()) {
        qWarning() << "[ContentTab] WebView not available for updating charts";
        return;
    }
    
    QString jsonData = QJsonDocument(data).toJson(QJsonDocument::Compact);
    QString script = QString("updateServerCharts(%1);").arg(jsonData);
    
    qDebug() << "[ContentTab] Running JavaScript to update charts:" << script.left(100) << "...";
    
    m_webView->page()->runJavaScript(script, [](const QVariant &result) {
        qDebug() << "[ContentTab] JavaScript execution result:" << result;
    });
}

void ContentTab::applyTheme()
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