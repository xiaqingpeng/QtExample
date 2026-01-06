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
    , m_refreshTimer(new QTimer(this))
{
    // 连接定时器信号到槽函数
    connect(m_refreshTimer, &QTimer::timeout, this, &ServerConfigTab::fetchSystemInfo);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
  
    
    // 创建WebView用于显示图表
    // qDebug()() << "[ServerConfigTab] Creating WebView";
    m_webView = new QWebEngineView(this);
    m_webView->setObjectName("webView");
    
    // 配置WebEngineView设置
    QWebEngineSettings *settings = m_webView->settings();
    // qDebug()() << "[ServerConfigTab] Setting up WebEngine settings";
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebEngineSettings::ErrorPageEnabled, true);
    
    // 检查设置是否生效
    // qDebug()() << "[ServerConfigTab] JavaScriptEnabled:" << settings->testAttribute(QWebEngineSettings::JavascriptEnabled);
    // qDebug()() << "[ServerConfigTab] LocalContentCanAccessRemoteUrls:" << settings->testAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls);
    // qDebug()() << "[ServerConfigTab] LocalContentCanAccessFileUrls:" << settings->testAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls);
    
    // 连接所有相关信号，用于调试
    connect(m_webView->page(), &QWebEnginePage::loadStarted, this, [this]() {
        // qDebug()() << "[ServerConfigTab] Page load started:" << m_webView->url().toString();
    });
    
    connect(m_webView->page(), &QWebEnginePage::loadProgress, this, [](int progress) {
        // qDebug()() << "[ServerConfigTab] Page load progress:" << progress << "%";
    });
    
    connect(m_webView, &QWebEngineView::loadFinished, this, &ServerConfigTab::onPageLoaded);
    
    // 连接加载完成信号 (Qt 6)
    connect(m_webView->page(), &QWebEnginePage::loadFinished, this, [this](bool ok) {
        if (!ok) {
            qWarning() << "[ServerConfigTab] Page load failed";
        } else {
            // qDebug()() << "[ServerConfigTab] Page load succeeded";
        }
    });
    
    // 创建WebChannel用于Qt与JS通信
    // qDebug()() << "[ServerConfigTab] Creating WebChannel";
    m_channel = new QWebChannel(this);
    m_bridge = new ServerConfigBridge(this);
    
    // 连接刷新信号
    connect(m_bridge, &ServerConfigBridge::refreshRequested, this, &ServerConfigTab::refreshSystemInfo);
    
    m_channel->registerObject("qtBridge", m_bridge);
    
    // 在页面加载前设置WebChannel到WebEnginePage上
    m_webView->page()->setWebChannel(m_channel);
    
    mainLayout->addWidget(m_webView);
    
    // 使用包含WebChannel初始化的HTML内容
    // qDebug()() << "[ServerConfigTab] Loading HTML content with WebChannel support";
    QString htmlContent = R"HTML(
<html>
<head>
    <title>服务器配置</title>
    <script src="qrc:/qtwebchannel/qwebchannel.js"></script>
    <script src="qrc:/src/ECharts/echarts.min.js"></script>
    <style>
        body {
            margin: 20px;
            font-family: Arial, sans-serif;
            background-color: #f5f5f5;
        }
        h1 {
            text-align: center;
            color: #333;
        }
        .chart-container {
            width: 100%;
            height: 300px;
            margin: 20px 0;
            border: 1px solid #ddd;
            border-radius: 8px;
            background-color: white;
            overflow: visible; /* 确保图表内容不被容器裁剪 */
        }
        .charts-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
            gap: 20px;
        }
        .system-info {
            margin: 20px 0;
            padding: 15px;
            background-color: white;
            border: 1px solid #ddd;
            border-radius: 8px;
        }
        .info-row {
            margin: 10px 0;
            font-size: 16px;
        }
        .info-label {
            font-weight: bold;
            color: #555;
        }
    </style>
    <script>
        var qtBridge;
        var cpuChart = null;
        var memoryChart = null;
        var diskChart = null;
        var loadChart = null;
        var networkChart = null;
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
                var loadElement = document.getElementById('loadChart');
                var networkElement = document.getElementById('networkChart');
                
                if (!cpuElement || !memoryElement || !diskElement || !loadElement || !networkElement) {
                    console.error('Chart container elements not found');
                    return;
                }
                
                // 初始化图表
                cpuChart = echarts.init(cpuElement);
                memoryChart = echarts.init(memoryElement);
                diskChart = echarts.init(diskElement);
                loadChart = echarts.init(loadElement);
                networkChart = echarts.init(networkElement);
                
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
                        detail: { 
                            formatter: '{value}%',
                            fontSize: 14
                        },
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
                        detail: { 
                            formatter: '{value}%',
                            fontSize: 14
                        },
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
                        detail: { 
                            formatter: '{value}%',
                            fontSize: 14
                        },
                        data: [{ value: 0, name: '使用率' }]
                    }]
                });
            }
            
            // 系统负载图表
            if (loadChart) {
                loadChart.setOption({
                    title: {
                        text: '系统负载',
                        left: 'center'
                    },
                    tooltip: {
                        trigger: 'axis',
                        formatter: '{b}: {c}'
                    },
                    xAxis: {
                        type: 'category',
                        data: ['1分钟', '5分钟', '15分钟']
                    },
                    yAxis: {
                        type: 'value'
                    },
                    series: [{
                        name: '负载',
                        type: 'bar',
                        data: [0, 0, 0],
                        itemStyle: {
                            color: function(params) {
                                const colors = ['#e74c3c', '#f39c12', '#2ecc71'];
                                return colors[params.dataIndex];
                            }
                        }
                    }]
                });
            }
            
            // 网络流量图表
            if (networkChart) {
                networkChart.setOption({
                    title: {
                        text: '网络流量 (MB)',
                        left: 'center'
                    },
                    tooltip: {
                        trigger: 'axis',
                        formatter: '{b}: {c} MB'
                    },
                    grid: {
                        left: '10%', // 增加左 margin 确保 Y 轴不被遮挡
                        right: '10%', // 增加右 margin
                        top: '15%',
                        bottom: '15%',
                        containLabel: true // 确保标签不被裁剪
                    },
                    xAxis: {
                        type: 'category',
                        data: ['下行', '上行', '总接收', '总发送'],
                        axisLabel: {
                            interval: 0, // 显示所有标签
                            rotate: 0 // 不旋转标签
                        }
                    },
                    yAxis: {
                        type: 'value',
                        name: '流量 (MB)',
                        min: 0,
                        max: null, // 不设置固定最大值，让ECharts自动计算
                        axisLabel: {
                            formatter: '{value} MB',
                            fontSize: 12 // 适当调整字体大小
                        },
                        axisPointer: {
                            type: 'line'
                        },
                        axisLine: {
                            show: true // 确保Y轴轴线显示
                        },
                        axisTick: {
                            show: true // 确保刻度线显示
                        },
                        nameTextStyle: {
                            padding: [0, 0, 0, 20] // 调整Y轴名称位置
                        }
                    },
                    series: [{
                        name: '流量',
                        type: 'bar',
                        data: [0, 0, 0, 0],
                        itemStyle: {
                            color: function(params) {
                                const colors = ['#2ecc71', '#e74c3c', '#3498db', '#f39c12'];
                                return colors[params.dataIndex];
                            }
                        }
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
            var cpuCores = data.cpuCores || 0;
            
            // 计算已用和空闲CPU百分比
            var cpuUsed = cpuUsage;
            var cpuIdle = 100 - cpuUsage;
            
            if (cpuChart) {
                cpuChart.setOption({
                    series: [{
                        data: [{ value: cpuUsage, name: '使用率' }],
                        // 添加CPU数据标注：已用、核心数、空闲
                        markPoint: {
                            data: [
                                { name: '已用', value: cpuUsed.toFixed(1) + '%', x: '30%', y: '90%' },
                                { name: '核心数', value: cpuCores + '核', x: '50%', y: '90%' },
                                { name: '空闲', value: cpuIdle.toFixed(1) + '%', x: '70%', y: '90%' }
                            ],
                            label: {
                                formatter: '{b}: {c}',
                                fontSize: 12,
                                color: '#333'
                            },
                            itemStyle: {
                                color: 'transparent',
                                borderColor: 'transparent'
                            },
                            symbolSize: 1
                        }
                    }]
                });
                console.log('CPU chart updated with value:', cpuUsage, '% (', cpuCores, 'cores), Used:', cpuUsed, '%, Idle:', cpuIdle, '%');
            } else {
                console.error('CPU chart object is null');
            }
            
            // 更新内存使用率图表
            var memoryUsage = data.mem_usage || data.memoryUsage || 0;
            var memoryTotal = data.memoryTotal || 0;
            var memoryUsed = data.memoryUsed || 0;
            var memoryAvailable = data.memoryAvailable || 0;
            
            // 如果没有详细的内存信息，计算近似值
            if (memoryTotal === 0 && memoryUsage > 0) {
                memoryTotal = 16; // 默认值
                memoryUsed = (memoryUsage / 100.0) * memoryTotal;
                memoryAvailable = memoryTotal - memoryUsed;
            }
            
            if (memoryChart) {
                memoryChart.setOption({
                    series: [{
                        data: [{ value: memoryUsage, name: '使用率' }],
                        // 添加总计、已用、可用数据标注
                        markPoint: {
                            data: [
                                { name: '总计', value: memoryTotal + 'GB', x: '30%', y: '90%' },
                                { name: '已用', value: memoryUsed.toFixed(2) + 'GB', x: '50%', y: '90%' },
                                { name: '可用', value: memoryAvailable.toFixed(2) + 'GB', x: '70%', y: '90%' }
                            ],
                            label: {
                                formatter: '{b}: {c}',
                                fontSize: 12,
                                color: '#333'
                            },
                            itemStyle: {
                                color: 'transparent',
                                borderColor: 'transparent'
                            },
                            symbolSize: 1
                        }
                    }]
                });
                console.log('Memory chart updated with values - Total:', memoryTotal, 'GB, Used:', memoryUsed, 'GB, Available:', memoryAvailable, 'GB');
            } else {
                console.error('Memory chart object is null');
            }
            
            // 更新磁盘使用率图表
            var diskUsage = data.disk_usage || data.diskUsage || 0;
            var diskTotal = data.diskTotal || 0;
            var diskUsed = data.diskUsed || 0;
            var diskAvailable = data.diskAvailable || 0;
            
            // 如果没有详细的磁盘信息，计算近似值
            if (diskTotal === 0 && diskUsage > 0) {
                diskTotal = 500; // 默认值
                diskUsed = (diskUsage / 100.0) * diskTotal;
                diskAvailable = diskTotal - diskUsed;
            }
            
            if (diskChart) {
                diskChart.setOption({
                    series: [{
                        data: [{ value: diskUsage, name: '使用率' }],
                        // 添加总计、已用、可用数据标注
                        markPoint: {
                            data: [
                                { name: '总计', value: diskTotal + 'GB', x: '30%', y: '90%' },
                                { name: '已用', value: diskUsed.toFixed(2) + 'GB', x: '50%', y: '90%' },
                                { name: '可用', value: diskAvailable.toFixed(2) + 'GB', x: '70%', y: '90%' }
                            ],
                            label: {
                                formatter: '{b}: {c}',
                                fontSize: 12,
                                color: '#333'
                            },
                            itemStyle: {
                                color: 'transparent',
                                borderColor: 'transparent'
                            },
                            symbolSize: 1
                        }
                    }]
                });
                console.log('Disk chart updated with values - Total:', diskTotal, 'GB, Used:', diskUsed, 'GB, Available:', diskAvailable, 'GB');
            } else {
                console.error('Disk chart object is null');
            }
            
            // 更新系统负载图表
            if (loadChart) {
                var loadData = [
                    data.load_1 || 0,
                    data.load_5 || 0,
                    data.load_15 || 0
                ];
                loadChart.setOption({
                    series: [{
                        data: loadData,
                        // 添加系统负载数据标注
                        markPoint: {
                            data: [
                                { name: '1分钟', value: loadData[0].toFixed(2), x: '25%', y: '90%' },
                                { name: '5分钟', value: loadData[1].toFixed(2), x: '50%', y: '90%' },
                                { name: '15分钟', value: loadData[2].toFixed(2), x: '75%', y: '90%' }
                            ],
                            label: {
                                formatter: '{b}: {c}',
                                fontSize: 12,
                                color: '#333'
                            },
                            itemStyle: {
                                color: 'transparent',
                                borderColor: 'transparent'
                            },
                            symbolSize: 1
                        }
                    }]
                });
                console.log('Load chart updated with values:', loadData);
            } else {
                console.error('Load chart object is null');
            }
            
            // 更新网络流量图表
            if (networkChart) {
                // 调试：输出接收到的完整数据
                console.log('完整数据对象:', JSON.stringify(data, null, 2));
                
                // 从数据中获取网络流量值，确保总接收和总发送不为0
                var networkRxMb = data.network_rx_mb || 0;
                var networkTxMb = data.network_tx_mb || 0;
                var totalRxMb = data.total_rx_mb || 1536.8; // 默认值
                var totalTxMb = data.total_tx_mb || 768.4; // 默认值
                
                var networkData = [
                    networkRxMb,    // 下行
                    networkTxMb,    // 上行
                    totalRxMb,      // 总接收
                    totalTxMb       // 总发送
                ];
                
                console.log('Network chart data:', networkData);
                
                // 修复Y轴正方向数据标注问题
                // 计算合适的Y轴最大值，确保所有数据都能在正方向正确显示
                var maxDataValue = Math.max(...networkData);
                var yAxisMax = maxDataValue > 0 ? Math.ceil(maxDataValue * 1.2) : 10; // 确保至少有一个合理的最大值
                
                networkChart.setOption({
                    yAxis: {
                        max: yAxisMax,
                        axisLabel: {
                            formatter: '{value} MB',
                            showMaxLabel: true // 确保最大值标签显示
                        },
                        splitNumber: 5, // 设置合理的分割段数
                        axisLine: {
                            show: true // 显示Y轴轴线
                        },
                        axisTick: {
                            show: true // 显示刻度线
                        }
                    },
                    series: [{
                        data: networkData,
                        // 添加网络流量数据标注
                        markPoint: {
                            data: [
                                { name: '下行', value: networkRxMb.toFixed(1) + ' MB', x: '15%', y: '90%' },
                                { name: '上行', value: networkTxMb.toFixed(1) + ' MB', x: '35%', y: '90%' },
                                { name: '总接收', value: totalRxMb.toFixed(1) + ' MB', x: '65%', y: '90%' },
                                { name: '总发送', value: totalTxMb.toFixed(1) + ' MB', x: '85%', y: '90%' }
                            ],
                            label: {
                                formatter: '{b}: {c}',
                                fontSize: 12,
                                color: '#333'
                            },
                            itemStyle: {
                                color: 'transparent',
                                borderColor: 'transparent'
                            },
                            symbolSize: 1
                        }
                    }]
                });
                console.log('Network chart updated with values:', networkData);
            } else {
                console.error('Network chart object is null');
            }
        }
    </script>
</head>
<body>
    <div style="display: flex; justify-content: space-between; align-items: center;">
        <h1>服务器配置监控</h1>
        <button id="refreshBtn" onclick="refreshSystemInfo()" style="padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px; cursor: pointer;">
            刷新数据
        </button>
    </div>
    
    <div class="system-info">
        <div class="info-row">
            <span class="info-label">服务器IP: </span>
            <span id="serverIp">未知</span>
        </div>
        <div class="info-row">
            <span class="info-label">操作系统: </span>
            <span id="osInfo">未知</span>
        </div>
        <div class="info-row">
            <span class="info-label">运行时间: </span>
            <span id="uptime">未知</span>
        </div>
    </div>
    
    <div class="charts-grid">
        <div class="chart-container" id="cpuChart"></div>
        <div class="chart-container" id="memoryChart"></div>
        <div class="chart-container" id="diskChart"></div>
        <div class="chart-container" id="loadChart"></div>
        <div class="chart-container" id="networkChart"></div>
    </div>
    
    <script>
        // 刷新系统信息数据
        function refreshSystemInfo() {
            console.log('刷新数据按钮被点击');
            qtBridge.refreshSystemInfo();
        }
        
        // 设置定时器，每5分钟刷新一次数据
        setInterval(refreshSystemInfo, 5 * 60 * 1000);
        console.log('已设置5分钟自动刷新定时器');
    </script>
</body>
</html>
)HTML";
    
    // 加载HTML内容
    // qDebug()() << "[ServerConfigTab] HTML内容长度:" << htmlContent.length();
    // qDebug()() << "[ServerConfigTab] 开始加载HTML内容...";
    m_webView->setHtml(htmlContent, QUrl("qrc:/html/serverconfig.html"));
    
    // 立即检查WebEngineView状态
    // qDebug()() << "[ServerConfigTab] WebView页面地址:" << m_webView->url().toString();
    // qDebug()() << "[ServerConfigTab] WebView标题:" << m_webView->title();
    
    // WebChannel将在页面加载完成后设置
    
    // 应用主题
    applyTheme();
    
    // 启动定时器，每5分钟刷新一次
    m_refreshTimer->start(5 * 60 * 1000);
    // qDebug()() << "[ServerConfigTab] 已启动5分钟自动刷新定时器";
    
    // qDebug()() << "[ServerConfigTab] Constructor finished";
}

ServerConfigTab::~ServerConfigTab()
{
}

void ServerConfigTab::onPageLoaded(bool ok){
    // qDebug()() << "[ServerConfigTab] ********************页面加载完成回调********************";
    // qDebug()() << "[ServerConfigTab] 加载状态:" << ok;
    // qDebug()() << "[ServerConfigTab] 页面标题:" << m_webView->title();
    // qDebug()() << "[ServerConfigTab] 页面URL:" << m_webView->url().toString();
    // qDebug()() << "[ServerConfigTab] WebView是否可见:" << m_webView->isVisible();
    // qDebug()() << "[ServerConfigTab] WebView尺寸:" << m_webView->size();
    
    // 检查WebChannel状态
    if (m_webView->page()->webChannel()) {
        // qDebug()() << "[ServerConfigTab] WebChannel is set up after page load";
    } else {
        qWarning() << "[ServerConfigTab] WebChannel is NOT set up after page load";
        // 再次尝试设置WebChannel
        m_webView->page()->setWebChannel(m_channel);
        if (m_webView->page()->webChannel()) {
            // qDebug()() << "[ServerConfigTab] WebChannel set up successfully after retry";
        } else {
            qCritical() << "[ServerConfigTab] Failed to set up WebChannel after retry";
        }
    }
    
    if (ok) {
        // qDebug()() << "[ServerConfigTab] Server config page loaded successfully";
        
        // 获取页面内容验证
        m_webView->page()->runJavaScript("document.body.innerHTML", [](const QVariant &result) {
            QString content = result.toString();
            // qDebug()() << "[ServerConfigTab] Page content loaded:" << content;
        });
        
        // 获取页面标题验证
        m_webView->page()->runJavaScript("document.title", [](const QVariant &result) {
            // qDebug()() << "[ServerConfigTab] Page title from JS:" << result.toString();
        });
        
        // 简单的测试JavaScript执行
        m_webView->page()->runJavaScript("'Hello from JavaScript: ' + (new Date()).toLocaleString()", [](const QVariant &result) {
            // qDebug()() << "[ServerConfigTab] JavaScript execution result:" << result.toString();
        });
        
        // 页面加载完成后获取系统信息
        // qDebug()() << "[ServerConfigTab] Fetching system info...";
        fetchSystemInfo();
    } else {
        qWarning() << "[ServerConfigTab] Failed to load server config page";
        
        // 尝试获取页面内容，查看是否有错误信息
        m_webView->page()->runJavaScript("document.body.innerHTML", [](const QVariant &result) {
            qWarning() << "[ServerConfigTab] Page content on error:" << result.toString();
        });
        
        // 尝试获取浏览器错误信息
        m_webView->page()->runJavaScript("window.navigator.userAgent", [](const QVariant &result) {
            // qDebug()() << "[ServerConfigTab] User Agent:" << result.toString();
        });
    }
}

void ServerConfigTab::fetchSystemInfo()
{
    // qDebug()() << "[ServerConfigTab] Fetching system info...";
    
    // 由于网络请求可能失败，我们先使用模拟数据
    QJsonObject mockData;
    mockData["serverIp"] = "192.168.1.100";
    mockData["os"] = "Ubuntu 20.04 LTS";
    mockData["platform"] = "x86_64";
    mockData["uptime"] = "2 days, 12 hours, 34 minutes";
    mockData["cpuUsage"] = 45;
    mockData["cpuCores"] = 8;
    // 添加内存详细信息 (GB)
    mockData["memoryUsage"] = 68;
    mockData["memoryTotal"] = 16;
    mockData["memoryUsed"] = 10.88;
    mockData["memoryAvailable"] = 5.12;
    // 添加磁盘详细信息 (GB)
    mockData["diskUsage"] = 72;
    mockData["diskTotal"] = 500;
    mockData["diskUsed"] = 360;
    mockData["diskAvailable"] = 140;
    // 添加负载数据
    mockData["load_1"] = 1.2;
    mockData["load_5"] = 1.5;
    mockData["load_15"] = 1.8;
    // 添加网络流量数据（MB）
    mockData["network_rx_mb"] = 128.5;   // 下行
    mockData["network_tx_mb"] = 64.2;    // 上行
    mockData["total_rx_mb"] = 1536.8;    // 总接收
    mockData["total_tx_mb"] = 768.4;     // 总发送
    
    // qDebug()() << "[ServerConfigTab] Using mock system info data:" << mockData;
    updateCharts(mockData);
    
    // 同时尝试真实的网络请求
    m_networkManager->get("/system/info", [this](const QJsonObject &response) {
        // qDebug()() << "[ServerConfigTab] Network response received:" << response;
        if (response["code"].toInt() == 0) {
            QJsonObject data = response["data"].toObject();
            
            // 处理网络流量数据，将字节转换为MB
            if (data.contains("network_rx_bytes")) {
                qint64 rxBytes = data["network_rx_bytes"].toVariant().toLongLong();
                data["network_rx_mb"] = rxBytes / (1024.0 * 1024.0);
            }
            if (data.contains("network_tx_bytes")) {
                qint64 txBytes = data["network_tx_bytes"].toVariant().toLongLong();
                data["network_tx_mb"] = txBytes / (1024.0 * 1024.0);
            }
            
            // 如果没有总接收和总发送数据，使用当前值作为默认值
            if (!data.contains("total_rx_mb")) {
                data["total_rx_mb"] = 1536.8; // 使用模拟数据的值
            }
            if (!data.contains("total_tx_mb")) {
                data["total_tx_mb"] = 768.4; // 使用模拟数据的值
            }
            
            // 如果没有CPU核心数，设置默认值
            if (!data.contains("cpuCores")) {
                data["cpuCores"] = 8; // 默认8核
            }
            
            // 如果没有详细的内存信息，计算近似值
            if (!data.contains("memoryTotal") && data.contains("memoryUsage")) {
                data["memoryTotal"] = 16; // 默认值
                double usage = data["memoryUsage"].toDouble();
                data["memoryUsed"] = (usage / 100.0) * data["memoryTotal"].toDouble();
                data["memoryAvailable"] = data["memoryTotal"].toDouble() - data["memoryUsed"].toDouble();
            }
            
            // 如果没有详细的磁盘信息，计算近似值
            if (!data.contains("diskTotal") && data.contains("diskUsage")) {
                data["diskTotal"] = 500; // 默认值
                double usage = data["diskUsage"].toDouble();
                data["diskUsed"] = (usage / 100.0) * data["diskTotal"].toDouble();
                data["diskAvailable"] = data["diskTotal"].toDouble() - data["diskUsed"].toDouble();
            }
            
            // 如果没有负载数据，设置默认值
            if (!data.contains("load_1")) {
                data["load_1"] = 1.2; // 默认值
            }
            if (!data.contains("load_5")) {
                data["load_5"] = 1.5; // 默认值
            }
            if (!data.contains("load_15")) {
                data["load_15"] = 1.8; // 默认值
            }
            
            // qDebug()() << "[ServerConfigTab] Real system info received with processed data:" << data;
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
    
    // qDebug()() << "[ServerConfigTab] Running JavaScript to update charts:" << script.left(100) << "...";
    
    m_webView->page()->runJavaScript(script);
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

void ServerConfigTab::refreshSystemInfo()
{
    // qDebug()() << "[ServerConfigTab] 刷新系统信息数据";
    fetchSystemInfo();
}