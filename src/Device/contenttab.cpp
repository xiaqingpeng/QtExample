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
    , m_pageLoaded(false)
    , m_refreshTimer(new QTimer(this))
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
    
    // 连接刷新信号
    connect(m_bridge, &ServerConfigBridge::refreshRequested, this, &ContentTab::refreshSystemInfo);
    
    m_channel->registerObject("qtBridge", m_bridge);
    
    // 在页面加载前设置WebChannel到WebEnginePage上
    m_webView->page()->setWebChannel(m_channel);
    
    mainLayout->addWidget(m_webView);
    
    // 连接定时器信号到槽函数
    connect(m_refreshTimer, &QTimer::timeout, this, &ContentTab::fetchSystemInfo);
    
    // 设置定时器周期为5秒（5000毫秒）
    m_refreshTimer->start(5000);
    
    // 立即调用一次fetchSystemInfo，获取初始数据
    fetchSystemInfo();
    
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
            margin: 20px;
            font-family: Arial, sans-serif;
            background-color: #f5f5f5;
        }
        h1 {
            text-align: center;
            color: #333;
            margin: 10px 0;
            font-size: 24px;
        }
        h2 {
            color: #333;
            margin: 10px 0;
            font-size: 20px;
        }
        .loading-container {
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            padding: 40px;
            text-align: center;
        }
        .loading-spinner {
            border: 4px solid rgba(0, 0, 0, 0.1);
            border-left: 4px solid #3498db;
            border-radius: 50%;
            width: 40px;
            height: 40px;
            animation: spin 1s linear infinite;
            margin-bottom: 20px;
        }
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        .loading-text {
            color: #666;
            font-size: 16px;
        }
        .main-container {
            display: grid;
            grid-template-rows: auto auto;
            gap: 20px;
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
            grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
            gap: 20px;
        }
        .chart-container {
            width: 100%;
            height: 300px;
            margin: 20px 0;
            border: 1px solid #ddd;
            border-radius: 8px;
            background-color: white;
            overflow: visible;
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
                height: 220px;
            }
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
        var dataLoaded = false;
        
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
            
            // 如果数据还没加载完成，显示加载状态
            if (!dataLoaded) {
                showLoadingState();
            }
        };
        
        // 显示加载状态
        function showLoadingState() {
            var systemInfo = document.querySelector('.system-info');
            var chartsContainer = document.querySelector('.charts-container');
            
            if (systemInfo) {
                systemInfo.innerHTML = `
                    <div class="loading-container">
                        <div class="loading-spinner"></div>
                        <div class="loading-text">正在获取系统信息...</div>
                    </div>
                `;
            }
            
            if (chartsContainer) {
                chartsContainer.style.display = 'none';
            }
        }
        
        // 隐藏加载状态
        function hideLoadingState() {
            var loadingContainer = document.querySelector('.system-info .loading-container');
            if (loadingContainer) {
                loadingContainer.style.display = 'none';
            }
            
            var chartsContainer = document.querySelector('.charts-container');
            if (chartsContainer) {
                chartsContainer.style.display = 'grid';
            }
        }
        
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
                        type: 'value',
                        min: 0
                    },
                    series: [{
                        name: '系统负载',
                        type: 'bar',
                        data: [0, 0, 0],
                        itemStyle: {
                            color: function(params) {
                                const colors = ['#FF6384', '#36A2EB', '#FFCE56'];
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
                        text: '网络流量',
                        left: 'center'
                    },
                    tooltip: {
                        trigger: 'axis',
                        formatter: '{b}: {c} MB'
                    },
                    xAxis: {
                        type: 'category',
                        data: ['下行', '上行', '总接收', '总发送']
                    },
                    yAxis: {
                        type: 'value',
                        min: 0,
                        axisLabel: {
                            formatter: '{value} MB'
                        }
                    },
                    series: [{
                        name: '网络流量',
                        type: 'bar',
                        data: [0, 0, 0, 0],
                        itemStyle: {
                            color: function(params) {
                                const colors = ['#4BC0C0', '#FF9F40', '#9966FF', '#FF6384'];
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
            
            // 标记数据已加载
            dataLoaded = true;
            
            // 隐藏加载状态
            hideLoadingState();
            
            // 更新系统信息显示
            var systemInfo = document.querySelector('.system-info');
            if (systemInfo) {
                // 清空现有内容，避免重复显示
                systemInfo.innerHTML = `
                    <div class="info-row">
                        <div class="info-label">IP地址:</div>
                        <div id="serverIp" class="info-value">${data.ip_address || '未知'}</div>
                    </div>
                    <div class="info-row">
                        <div class="info-label">操作系统:</div>
                        <div id="osInfo" class="info-value">${data.os_info || data.os || '未知'}</div>
                    </div>
                    <div class="info-row">
                        <div class="info-label">运行时间:</div>
                        <div id="uptime" class="info-value">${data.uptime_days ? data.uptime_days + '天' : data.uptime || '未知'}</div>
                    </div>
                `;
            }
            else {
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
            var cpuIdle = 100 - cpuUsage;
            if (cpuChart) {
                cpuChart.setOption({
                    series: [{
                        data: [{ value: cpuUsage, name: '使用率' }],
                        // 添加CPU使用率、已用和空闲数据标注
                        markPoint: {
                            data: [
                                { name: 'CPU使用率', value: cpuUsage + '%', x: '50%', y: '90%' },
                                { name: '已用', value: cpuUsage + '%', x: '30%', y: '90%' },
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
                console.log('CPU chart updated with value:', cpuUsage);
            } else {
                console.error('CPU chart object is null');
            }
            
            // 更新内存使用率图表
            var memoryUsage = data.mem_usage || data.memoryUsage || 0;
            var memTotal = data.mem_total || data.memoryTotal || 0;
            var memUsed = data.mem_used || data.memoryUsed || 0;
            var memAvailable = data.mem_available || (memTotal - memUsed);
            
            // 如果没有详细的内存信息，计算近似值
            if (memTotal === 0 && memoryUsage > 0) {
                memTotal = 8; // 默认值(GB)
                memUsed = (memoryUsage / 100.0) * memTotal;
                memAvailable = memTotal - memUsed;
            }
            
            if (memoryChart) {
                memoryChart.setOption({
                    series: [{
                        data: [{ value: memoryUsage, name: '使用率' }],
                        // 添加内存总计、已用、可用数据标注
                        markPoint: {
                            data: [
                                { name: '总计', value: memTotal + 'GB', x: '30%', y: '90%' },
                                { name: '已用', value: memUsed.toFixed(2) + 'GB', x: '50%', y: '90%' },
                                { name: '可用', value: memAvailable.toFixed(2) + 'GB', x: '70%', y: '90%' }
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
                console.log('Memory chart updated with values - Total:', memTotal, 'GB, Used:', memUsed, 'GB, Available:', memAvailable, 'GB');
            } else {
                console.error('Memory chart object is null');
            }
            
            // 更新磁盘使用率图表
            var diskUsage = data.disk_usage || data.diskUsage || 0;
            var diskTotal = data.disk_total || data.diskTotal || 0;
            var diskUsed = data.disk_used || data.diskUsed || 0;
            var diskAvailable = data.diskAvailable || (diskTotal - diskUsed);
            
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
                
                // 计算合适的Y轴最大值，确保所有数据都能在正方向正确显示
                var maxDataValue = Math.max(...networkData);
                var yAxisMax = maxDataValue > 0 ? Math.ceil(maxDataValue * 1.2) : 10; // 确保至少有一个合理的最大值
                
                networkChart.setOption({
                    yAxis: {
                        max: yAxisMax,
                        axisLabel: {
                            formatter: '{value} MB',
                            showMaxLabel: true // 确保最大值标签显示
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
        <h1>电脑本机配置监控</h1>
        <button id="refreshBtn" onclick="refreshSystemInfo()" style="padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px; cursor: pointer;">
            刷新数据
        </button>
    </div>
    
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
            <div class="chart-container" id="loadChart"></div>
            <div class="chart-container" id="networkChart"></div>
        </div>
    </div>
    
    <script>
        // 刷新系统信息数据
        function refreshSystemInfo() {
            console.log('刷新数据按钮被点击');
            if (qtBridge) {
                qtBridge.refreshSystemInfo();
            } else {
                console.error('Qt Bridge not available!');
            }
        }
    </script>
</body>
</html>
)HTML";
    
    // 提前开始获取系统信息，而不是等页面加载完成
    qDebug() << "[ContentTab] 提前开始获取系统信息...";
    fetchSystemInfo();
    
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
            m_pageLoaded = true;
            qDebug() << "[ContentTab] Server config page loaded successfully";
            
            // 检查是否有待处理的数据
            if (!m_pendingData.isEmpty()) {
                qDebug() << "[ContentTab] Processing cached data after page load";
                updateCharts(m_pendingData);
                m_pendingData = QJsonObject(); // 清空缓存
            }
        
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
        
        // 如果系统信息已经获取完成，直接更新图表
        // 否则，等待fetchSystemInfo完成后自动更新
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

void ContentTab::refreshSystemInfo()
{
    qDebug() << "[ContentTab] 刷新系统信息数据";
    fetchSystemInfo();
}

// 获取本地系统信息
void ContentTab::fetchSystemInfo()
{
    qDebug() << "[ContentTab] Fetching system info...";
    
    // 创建一个临时的系统信息对象，用于显示默认值
    QJsonObject defaultInfo;
    defaultInfo["cpu_usage"] = 0.0;
    defaultInfo["mem_total"] = 0.0;
    defaultInfo["mem_used"] = 0.0;
    defaultInfo["mem_usage"] = 0.0;
    defaultInfo["disk_total"] = 0.0;
    defaultInfo["disk_used"] = 0.0;
    defaultInfo["disk_usage"] = 0.0;
    defaultInfo["ip_address"] = "获取中...";
    defaultInfo["os_info"] = "获取中...";
    defaultInfo["uptime_days"] = 0.0;
    defaultInfo["platform"] = "darwin";
    // 添加系统负载和网络流量的默认值
    defaultInfo["load_1"] = 0.0;
    defaultInfo["load_5"] = 0.0;
    defaultInfo["load_15"] = 0.0;
    defaultInfo["network_rx_mb"] = 0.0;
    defaultInfo["network_tx_mb"] = 0.0;
    defaultInfo["total_rx_mb"] = 0.0;
    defaultInfo["total_tx_mb"] = 0.0;
    
    // 先更新一次默认数据，避免页面完全空白
    updateCharts(defaultInfo);
    
    // 创建一个线程来异步获取系统信息
    QThread *thread = new QThread(this);
    QObject *worker = new QObject();
    
    // 在线程中执行获取系统信息的操作
    QObject::connect(thread, &QThread::started, worker, [worker, this]() {
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
        
        // 设置网络流量默认值
        systemInfo["network_rx_mb"] = 0.0;
        systemInfo["network_tx_mb"] = 0.0;
        systemInfo["total_rx_mb"] = 0.0;
        systemInfo["total_tx_mb"] = 0.0;
        
        // 将结果发送回主线程
        QMetaObject::invokeMethod(this, "updateCharts", Qt::QueuedConnection, Q_ARG(QJsonObject, systemInfo));
        
        // 完成后退出线程
        QThread::currentThread()->quit();
    });
    
    // 连接线程完成信号，自动清理资源
    QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    
    worker->moveToThread(thread);
    thread->start();
    
    // 同时尝试从网络获取系统信息
    if (m_networkManager) {
        qDebug() << "[ContentTab] Attempting to fetch system info from network...";
        m_networkManager->get("/system/info", [this](const QJsonObject &response) {
            qDebug() << "[ContentTab] Network response received:" << response;
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
                
                // 转换字段名称以匹配本地系统信息格式
                if (data.contains("cpuUsage")) {
                    data["cpu_usage"] = data["cpuUsage"];
                }
                if (data.contains("memoryUsage")) {
                    data["mem_usage"] = data["memoryUsage"];
                }
                if (data.contains("memoryTotal")) {
                    data["mem_total"] = data["memoryTotal"];
                }
                if (data.contains("memoryUsed")) {
                    data["mem_used"] = data["memoryUsed"];
                }
                if (data.contains("diskUsage")) {
                    data["disk_usage"] = data["diskUsage"];
                }
                if (data.contains("diskTotal")) {
                    data["disk_total"] = data["diskTotal"];
                }
                if (data.contains("diskUsed")) {
                    data["disk_used"] = data["diskUsed"];
                }
                if (data.contains("serverIp")) {
                    data["ip_address"] = data["serverIp"];
                }
                if (data.contains("os")) {
                    data["os_info"] = data["os"];
                }
                
                qDebug() << "[ContentTab] Network system info received with processed data:" << data;
                updateCharts(data);
            } else {
                qWarning() << "[ContentTab] Failed to fetch real system info:" << response["msg"].toString();
            }
        }, [](const QString &error) {
            qWarning() << "[ContentTab] Network error fetching system info:" << error;
        });
    } else {
        qWarning() << "[ContentTab] NetworkManager not available for fetching system info";
    }
}

void ContentTab::updateCharts(const QJsonObject &data)
{
    if (!m_pageLoaded) {
        // 页面未加载完成，缓存数据
        m_pendingData = data;
        qDebug() << "[ContentTab] Page not loaded yet, caching data:" << QJsonDocument(data).toJson(QJsonDocument::Compact).left(100) << "...";
        return;
    }
    
    if (!m_webView || !m_webView->page()) {
        qWarning() << "[ContentTab] WebView not available for updating charts";
        return;
    }
    
    QString jsonData = QJsonDocument(data).toJson(QJsonDocument::Compact);
    QString script = QString("updateServerCharts(%1);").arg(jsonData);
    
    qDebug() << "[ContentTab] Running JavaScript to update charts:" << script.left(100) << "...";
    
    m_webView->page()->runJavaScript(script);
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