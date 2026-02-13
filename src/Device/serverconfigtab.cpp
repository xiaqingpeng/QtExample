#include "serverconfigtab.h"
#include <QJsonDocument>
#include <QDebug>
#include <QThread>
#include <QPointer>
#include <QSysInfo>
#include <QStorageInfo>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QDir>
#include <QRegularExpression>
#include <QProcess>
#include <QDateTime>
#include <QPushButton>
#include <QPainter>
#include <algorithm>

ServerConfigTab::ServerConfigTab(QWidget *parent)
    : QWidget(parent)
    , m_apiService(nullptr)
    , m_refreshTimer(new QTimer(this))
    , m_serverIpLabel(nullptr)
    , m_osInfoLabel(nullptr)
    , m_uptimeLabel(nullptr)
    , m_cpuGauge(nullptr)
    , m_memoryGauge(nullptr)
    , m_diskGauge(nullptr)
    , m_loadChartView(nullptr)
    , m_networkChartView(nullptr)
    , m_loadChart(nullptr)
    , m_networkChart(nullptr)
    , m_loadSeries(nullptr)
    , m_networkSeries(nullptr)
{
    m_apiService = new ApiService(this);
    
    setupUI();
    
    connect(m_refreshTimer, &QTimer::timeout, this, &ServerConfigTab::fetchSystemInfo);
    m_refreshTimer->start(5 * 60 * 1000); // 每5分钟刷新一次
    
    fetchSystemInfo();
    
        QTimer::singleShot(0, this, [this]() {
                applyTheme();
    });
}

ServerConfigTab::~ServerConfigTab()
{
}

void ServerConfigTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    // 标题和刷新按钮
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("服务器配置监控", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    
    QPushButton *refreshBtn = new QPushButton("刷新数据", this);
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &ServerConfigTab::refreshSystemInfo);
    headerLayout->addWidget(refreshBtn);
    mainLayout->addLayout(headerLayout);
    
    // 系统信息区域
    QWidget *infoWidget = new QWidget(this);
    infoWidget->setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px; padding: 15px;");
    QHBoxLayout *infoLayout = new QHBoxLayout(infoWidget);
    
    m_serverIpLabel = new QLabel("服务器IP: 未知", this);
    m_osInfoLabel = new QLabel("操作系统: 未知", this);
    m_uptimeLabel = new QLabel("运行时间: 未知", this);
    
    infoLayout->addWidget(m_serverIpLabel);
    infoLayout->addWidget(m_osInfoLabel);
    infoLayout->addWidget(m_uptimeLabel);
    infoLayout->addStretch();
    
    mainLayout->addWidget(infoWidget);
    
    // 图表区域
    QGridLayout *chartsLayout = new QGridLayout();
    chartsLayout->setSpacing(20);
    
    // CPU 使用率 Gauge
    QWidget *cpuWidget = new QWidget(this);
    cpuWidget->setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px; padding: 10px;");
    QVBoxLayout *cpuLayout = new QVBoxLayout(cpuWidget);
    QLabel *cpuTitle = new QLabel("CPU使用率", this);
    cpuTitle->setAlignment(Qt::AlignCenter);
    cpuTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    cpuLayout->addWidget(cpuTitle);
    m_cpuGauge = new GaugeWidget(this);
    m_cpuGauge->setTitle("CPU使用率");
    m_cpuGauge->setUnit("%");
    cpuLayout->addWidget(m_cpuGauge, 1);
    chartsLayout->addWidget(cpuWidget, 0, 0);
    
    // 内存使用率 Gauge
    QWidget *memoryWidget = new QWidget(this);
    memoryWidget->setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px; padding: 10px;");
    QVBoxLayout *memoryLayout = new QVBoxLayout(memoryWidget);
    QLabel *memoryTitle = new QLabel("内存使用率", this);
    memoryTitle->setAlignment(Qt::AlignCenter);
    memoryTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    memoryLayout->addWidget(memoryTitle);
    m_memoryGauge = new GaugeWidget(this);
    m_memoryGauge->setTitle("内存使用率");
    m_memoryGauge->setUnit("%");
    memoryLayout->addWidget(m_memoryGauge, 1);
    chartsLayout->addWidget(memoryWidget, 0, 1);
            
    // 磁盘使用率 Gauge
    QWidget *diskWidget = new QWidget(this);
    diskWidget->setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px; padding: 10px;");
    QVBoxLayout *diskLayout = new QVBoxLayout(diskWidget);
    QLabel *diskTitle = new QLabel("磁盘使用率", this);
    diskTitle->setAlignment(Qt::AlignCenter);
    diskTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    diskLayout->addWidget(diskTitle);
    m_diskGauge = new GaugeWidget(this);
    m_diskGauge->setTitle("磁盘使用率");
    m_diskGauge->setUnit("%");
    diskLayout->addWidget(m_diskGauge, 1);
    chartsLayout->addWidget(diskWidget, 0, 2);
            
    // 系统负载柱状图
    QWidget *loadWidget = new QWidget(this);
    loadWidget->setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px; padding: 10px;");
    QVBoxLayout *loadLayout = new QVBoxLayout(loadWidget);
    QLabel *loadTitle = new QLabel("系统负载", this);
    loadTitle->setAlignment(Qt::AlignCenter);
    loadTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    loadLayout->addWidget(loadTitle);
    
    m_loadChart = new QChart();
    m_loadSeries = new QBarSeries();
    m_loadChart->addSeries(m_loadSeries);
    m_loadChart->setTitle("系统负载");
    m_loadChart->setAnimationOptions(QChart::SeriesAnimations);
    m_loadChart->legend()->setVisible(false);
    
    QStringList loadCategories;
    loadCategories << "1分钟" << "5分钟" << "15分钟";
    QBarCategoryAxis *loadAxisX = new QBarCategoryAxis();
    loadAxisX->append(loadCategories);
    m_loadChart->addAxis(loadAxisX, Qt::AlignBottom);
    m_loadSeries->attachAxis(loadAxisX);
    
    QValueAxis *loadAxisY = new QValueAxis();
    loadAxisY->setMin(0);
    m_loadChart->addAxis(loadAxisY, Qt::AlignLeft);
    m_loadSeries->attachAxis(loadAxisY);
    
    m_loadChartView = new QChartView(m_loadChart, this);
    m_loadChartView->setRenderHint(QPainter::Antialiasing);
    m_loadChartView->setMinimumHeight(250);
    loadLayout->addWidget(m_loadChartView);
    chartsLayout->addWidget(loadWidget, 1, 0, 1, 2);
            
    // 网络流量柱状图
    QWidget *networkWidget = new QWidget(this);
    networkWidget->setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px; padding: 10px;");
    QVBoxLayout *networkLayout = new QVBoxLayout(networkWidget);
    QLabel *networkTitle = new QLabel("网络流量 (MB)", this);
    networkTitle->setAlignment(Qt::AlignCenter);
    networkTitle->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    networkLayout->addWidget(networkTitle);
    
    m_networkChart = new QChart();
    m_networkSeries = new QBarSeries();
    m_networkChart->addSeries(m_networkSeries);
    m_networkChart->setTitle("网络流量");
    m_networkChart->setAnimationOptions(QChart::SeriesAnimations);
    m_networkChart->legend()->setVisible(false);
    
    QStringList networkCategories;
    networkCategories << "下行" << "上行" << "总接收" << "总发送";
    QBarCategoryAxis *networkAxisX = new QBarCategoryAxis();
    networkAxisX->append(networkCategories);
    m_networkChart->addAxis(networkAxisX, Qt::AlignBottom);
    m_networkSeries->attachAxis(networkAxisX);
    
    QValueAxis *networkAxisY = new QValueAxis();
    networkAxisY->setMin(0);
    networkAxisY->setTitleText("流量 (MB)");
    m_networkChart->addAxis(networkAxisY, Qt::AlignLeft);
    m_networkSeries->attachAxis(networkAxisY);
    
    m_networkChartView = new QChartView(m_networkChart, this);
    m_networkChartView->setRenderHint(QPainter::Antialiasing);
    m_networkChartView->setMinimumHeight(250);
    networkLayout->addWidget(m_networkChartView);
    chartsLayout->addWidget(networkWidget, 1, 2);
    
    mainLayout->addLayout(chartsLayout);
    mainLayout->addStretch();
}

void ServerConfigTab::fetchSystemInfo()
{
    // 创建模拟数据（实际应该从API获取）
    QJsonObject mockData;
    mockData["serverIp"] = "192.168.1.100";
    mockData["os"] = "Ubuntu 20.04 LTS";
    mockData["platform"] = "x86_64";
    mockData["uptime"] = "2 days, 12 hours, 34 minutes";
    mockData["cpuUsage"] = 45;
    mockData["cpuCores"] = 8;
    mockData["memoryUsage"] = 68;
    mockData["memoryTotal"] = 16;
    mockData["memoryUsed"] = 10.88;
    mockData["memoryAvailable"] = 5.12;
    mockData["diskUsage"] = 72;
    mockData["diskTotal"] = 500;
    mockData["diskUsed"] = 360;
    mockData["diskAvailable"] = 140;
    mockData["load_1"] = 1.2;
    mockData["load_5"] = 1.5;
    mockData["load_15"] = 1.8;
    mockData["network_rx_mb"] = 128.5;
    mockData["network_tx_mb"] = 64.2;
    mockData["total_rx_mb"] = 1536.8;
    mockData["total_tx_mb"] = 768.4;
    
    updateCharts(mockData);
    
    // 同时尝试真实的网络请求
    if (m_apiService) {
    m_apiService->get("/system/info", [this](const QJsonObject &response) {
        if (response["code"].toInt() == 0) {
            QJsonObject data = response["data"].toObject();
            
                // 处理网络流量数据
            if (data.contains("network_rx_bytes")) {
                qint64 rxBytes = data["network_rx_bytes"].toVariant().toLongLong();
                data["network_rx_mb"] = rxBytes / (1024.0 * 1024.0);
            }
            if (data.contains("network_tx_bytes")) {
                qint64 txBytes = data["network_tx_bytes"].toVariant().toLongLong();
                data["network_tx_mb"] = txBytes / (1024.0 * 1024.0);
            }
            
                // 转换字段名称
                if (data.contains("cpuUsage")) data["cpu_usage"] = data["cpuUsage"];
                if (data.contains("memoryUsage")) data["mem_usage"] = data["memoryUsage"];
                if (data.contains("memoryTotal")) data["mem_total"] = data["memoryTotal"];
                if (data.contains("memoryUsed")) data["mem_used"] = data["memoryUsed"];
                if (data.contains("diskUsage")) data["disk_usage"] = data["diskUsage"];
                if (data.contains("diskTotal")) data["disk_total"] = data["diskTotal"];
                if (data.contains("diskUsed")) data["disk_used"] = data["diskUsed"];
                if (data.contains("serverIp")) data["ip_address"] = data["serverIp"];
                if (data.contains("os")) data["os_info"] = data["os"];
                
                updateCharts(data);
        }
    }, [](const QString &error) {
            qWarning() << "[ServerConfigTab] Network error:" << error;
    });
    }
}

void ServerConfigTab::updateCharts(const QJsonObject &data)
{
    // 更新系统信息
    if (m_serverIpLabel) {
        m_serverIpLabel->setText("服务器IP: " + (data["ip_address"].toString().isEmpty() ? data["serverIp"].toString() : data["ip_address"].toString()));
    }
    if (m_osInfoLabel) {
        m_osInfoLabel->setText("操作系统: " + (data["os_info"].toString().isEmpty() ? data["os"].toString() : data["os_info"].toString()));
    }
    if (m_uptimeLabel) {
        QString uptime = data["uptime_days"].toDouble() > 0 ? 
            QString::number(data["uptime_days"].toDouble(), 'f', 1) + "天" : 
            data["uptime"].toString();
        m_uptimeLabel->setText("运行时间: " + uptime);
    }
    
    // 更新 CPU Gauge
    double cpuUsage = data["cpu_usage"].toDouble() > 0 ? data["cpu_usage"].toDouble() : data["cpuUsage"].toDouble();
    double cpuIdle = 100 - cpuUsage;
    int cpuCores = data["cpuCores"].toInt();
    QStringList cpuDetails;
    cpuDetails << QString("已用: %1%").arg(cpuUsage, 0, 'f', 1)
               << QString("核心数: %1核").arg(cpuCores)
               << QString("空闲: %1%").arg(cpuIdle, 0, 'f', 1);
    updateGauge(m_cpuGauge, cpuUsage, cpuDetails);
    
    // 更新内存 Gauge
    double memoryUsage = data["mem_usage"].toDouble() > 0 ? data["mem_usage"].toDouble() : data["memoryUsage"].toDouble();
    double memTotal = data["mem_total"].toDouble() > 0 ? data["mem_total"].toDouble() : data["memoryTotal"].toDouble();
    double memUsed = data["mem_used"].toDouble() > 0 ? data["mem_used"].toDouble() : data["memoryUsed"].toDouble();
    double memAvailable = data["memoryAvailable"].toDouble() > 0 ? data["memoryAvailable"].toDouble() : (memTotal - memUsed);
    QStringList memDetails;
    memDetails << QString("总计: %1 GB").arg(memTotal, 0, 'f', 2)
               << QString("已用: %1 GB").arg(memUsed, 0, 'f', 2)
               << QString("可用: %1 GB").arg(memAvailable, 0, 'f', 2);
    updateGauge(m_memoryGauge, memoryUsage, memDetails);
    
    // 更新磁盘 Gauge
    double diskUsage = data["disk_usage"].toDouble() > 0 ? data["disk_usage"].toDouble() : data["diskUsage"].toDouble();
    double diskTotal = data["disk_total"].toDouble() > 0 ? data["disk_total"].toDouble() : data["diskTotal"].toDouble();
    double diskUsed = data["disk_used"].toDouble() > 0 ? data["disk_used"].toDouble() : data["diskUsed"].toDouble();
    double diskAvailable = data["diskAvailable"].toDouble() > 0 ? data["diskAvailable"].toDouble() : (diskTotal - diskUsed);
    QStringList diskDetails;
    diskDetails << QString("总计: %1 GB").arg(diskTotal, 0, 'f', 2)
                 << QString("已用: %1 GB").arg(diskUsed, 0, 'f', 2)
                 << QString("可用: %1 GB").arg(diskAvailable, 0, 'f', 2);
    updateGauge(m_diskGauge, diskUsage, diskDetails);
    
    // 更新系统负载图表
    QList<double> loadData;
    loadData << data["load_1"].toDouble()
             << data["load_5"].toDouble()
             << data["load_15"].toDouble();
    QStringList loadCategories;
    loadCategories << "1分钟" << "5分钟" << "15分钟";
    updateBarChart(m_loadChart, m_loadSeries, loadData, loadCategories, "系统负载");
    
    // 更新网络流量图表
    QList<double> networkData;
    networkData << data["network_rx_mb"].toDouble()
                << data["network_tx_mb"].toDouble()
                << data["total_rx_mb"].toDouble()
                << data["total_tx_mb"].toDouble();
    QStringList networkCategories;
    networkCategories << "下行" << "上行" << "总接收" << "总发送";
    updateBarChart(m_networkChart, m_networkSeries, networkData, networkCategories, "网络流量");
        }

void ServerConfigTab::updateGauge(GaugeWidget *gauge, double value, const QStringList &details)
{
    if (gauge) {
        gauge->setValue(value);
        gauge->setDetails(details);
    }
}

void ServerConfigTab::updateBarChart(QChart *chart, QBarSeries *series, const QList<double> &data, const QStringList &categories, const QString &title)
{
    if (!chart || !series) return;
    
    // 清除现有数据
    series->clear();
    
    // 创建新的 BarSet
    QBarSet *barSet = new QBarSet(title);
    QList<QColor> colors;
    if (categories.size() == 3) {
        colors = {QColor(255, 99, 132), QColor(255, 159, 64), QColor(75, 192, 192)};
    } else {
        colors = {QColor(46, 204, 113), QColor(231, 76, 60), QColor(52, 152, 219), QColor(243, 156, 18)};
        }
    
    for (int i = 0; i < data.size(); ++i) {
        *barSet << data[i];
    }
    barSet->setColor(colors[0]); // 设置第一个颜色
    
    series->append(barSet);
    
    // 更新 Y 轴范围
    QList<QAbstractAxis*> verticalAxes = chart->axes(Qt::Vertical);
    if (!verticalAxes.isEmpty()) {
        QValueAxis *axisY = qobject_cast<QValueAxis*>(verticalAxes.first());
        if (axisY) {
            double maxValue = *std::max_element(data.begin(), data.end());
            axisY->setMax(maxValue * 1.2);
        }
    }
}

void ServerConfigTab::refreshSystemInfo()
{
    fetchSystemInfo();
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
    ).arg(theme->colors().BACKGROUND,
          theme->colors().TEXT_PRIMARY,
          ThemeManager::Typography::FONT_FAMILY);
    
    setStyleSheet(styleSheet);
}
