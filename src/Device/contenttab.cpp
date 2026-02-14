#include "contenttab.h"
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

ContentTab::ContentTab(QWidget *parent)
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
    
    connect(m_refreshTimer, &QTimer::timeout, this, &ContentTab::fetchSystemInfo);
    m_refreshTimer->start(5000); // 每5秒刷新一次
    
    fetchSystemInfo();
    
    QTimer::singleShot(0, this, [this]() {
        applyTheme();
    });
}

ContentTab::~ContentTab()
{
    // 查找并等待所有子线程完成
    const QList<QThread*> childThreads = findChildren<QThread*>();
    for (QThread *thread : childThreads) {
        if (thread && thread->isRunning()) {
            thread->quit();
            if (!thread->wait(3000)) {
                qWarning() << "[ContentTab] Thread did not finish in time, terminating...";
                thread->terminate();
                thread->wait(1000);
            }
        }
    }
}

void ContentTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    // 标题和刷新按钮
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("电脑本机配置监控", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    
    QPushButton *refreshBtn = new QPushButton("刷新数据", this);
    refreshBtn->setStyleSheet("padding: 8px 16px; background-color: #3498db; color: white; border: none; border-radius: 4px;");
    connect(refreshBtn, &QPushButton::clicked, this, &ContentTab::refreshSystemInfo);
    headerLayout->addWidget(refreshBtn);
    mainLayout->addLayout(headerLayout);
    
    // 系统信息区域
    QWidget *infoWidget = new QWidget(this);
    infoWidget->setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px; padding: 15px;");
    QHBoxLayout *infoLayout = new QHBoxLayout(infoWidget);
    
    m_serverIpLabel = new QLabel("IP地址: 未知", this);
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

void ContentTab::fetchSystemInfo()
{
    // 创建默认信息对象
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
    
#ifdef Q_OS_WIN
    defaultInfo["platform"] = "windows";
#elif defined(Q_OS_LINUX)
    defaultInfo["platform"] = "linux";
#elif defined(Q_OS_MACOS)
    defaultInfo["platform"] = "darwin";
#else
    defaultInfo["platform"] = "unknown";
#endif
    
    defaultInfo["load_1"] = 0.0;
    defaultInfo["load_5"] = 0.0;
    defaultInfo["load_15"] = 0.0;
    defaultInfo["network_rx_mb"] = 0.0;
    defaultInfo["network_tx_mb"] = 0.0;
    defaultInfo["total_rx_mb"] = 0.0;
    defaultInfo["total_tx_mb"] = 0.0;
    
    updateCharts(defaultInfo);
    
    // 创建线程异步获取系统信息
    QThread *thread = new QThread(this);
    QObject *worker = new QObject();
    
    QPointer<ContentTab> safeThis = this;
    
    QObject::connect(thread, &QThread::started, worker, [safeThis]() {
        if (!safeThis) {
            QThread::currentThread()->quit();
            return;
        }
        QJsonObject systemInfo;
        
        systemInfo["os_info"] = QSysInfo::productType() + " " + QSysInfo::productVersion();
        
#ifdef Q_OS_WIN
        systemInfo["platform"] = "windows";
#elif defined(Q_OS_LINUX)
        systemInfo["platform"] = "linux";
#elif defined(Q_OS_MACOS)
        systemInfo["platform"] = "darwin";
#else
        systemInfo["platform"] = "unknown";
#endif
        
        // 获取网络接口信息
        QString ipAddress = "未知";
        const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
        for (const QNetworkInterface &interface : interfaces) {
            if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
                interface.flags().testFlag(QNetworkInterface::IsRunning) &&
                !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
                const QList<QNetworkAddressEntry> entries = interface.addressEntries();
                for (const QNetworkAddressEntry &entry : entries) {
                    QHostAddress addr = entry.ip();
                    if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
                        ipAddress = addr.toString();
                        break;
                    }
                }
                if (ipAddress != "未知") break;
            }
        }
        systemInfo["ip_address"] = ipAddress;
        
        // 获取磁盘信息
        try {
            QStorageInfo storage = QStorageInfo::root();
            if (storage.isValid() && storage.isReady()) {
                qint64 totalBytes = storage.bytesTotal();
                qint64 availableBytes = storage.bytesAvailable();
                qint64 usedBytes = totalBytes - availableBytes;
                
                systemInfo["disk_total"] = totalBytes / (1024.0 * 1024.0 * 1024.0);
                systemInfo["disk_used"] = usedBytes / (1024.0 * 1024.0 * 1024.0);
                if (totalBytes > 0) {
                    systemInfo["disk_usage"] = (usedBytes * 100.0) / totalBytes;
                } else {
                    systemInfo["disk_usage"] = 0.0;
                }
            } else {
                systemInfo["disk_total"] = 0.0;
                systemInfo["disk_used"] = 0.0;
                systemInfo["disk_usage"] = 0.0;
            }
        } catch (...) {
            systemInfo["disk_total"] = 0.0;
            systemInfo["disk_used"] = 0.0;
            systemInfo["disk_usage"] = 0.0;
        }
        
        // 平台特定的系统信息获取
#ifdef Q_OS_WIN
        // Windows系统信息获取
        QProcess process;
        process.start("wmic", QStringList() << "cpu" << "get" << "loadpercentage" << "/value");
        process.waitForFinished();
        QString cpuOutput = process.readAllStandardOutput().trimmed();
        systemInfo["cpu_usage"] = cpuOutput.toDouble();
        
        process.start("wmic", QStringList() << "OS" << "get" << "TotalVisibleMemorySize,FreePhysicalMemory" << "/value");
        process.waitForFinished();
        QString memOutput = process.readAllStandardOutput().trimmed();
        QStringList memLines = memOutput.split("\n");
        if (memLines.size() >= 2) {
            qint64 totalMem = memLines[0].split("=")[1].trimmed().toLongLong();
            qint64 freeMem = memLines[1].split("=")[1].trimmed().toLongLong();
            qint64 usedMem = totalMem - freeMem;
            systemInfo["mem_total"] = totalMem / (1024.0 * 1024.0 * 1024.0);
            systemInfo["mem_used"] = usedMem / (1024.0 * 1024.0 * 1024.0);
            systemInfo["mem_usage"] = (usedMem * 100.0) / totalMem;
        }
#elif defined(Q_OS_LINUX)
        // Linux系统信息获取
        QProcess process;
        process.start("cat", QStringList() << "/proc/meminfo");
        process.waitForFinished();
        QString memOutput = process.readAllStandardOutput();
        QStringList memLines = memOutput.split("\n");
        qint64 totalMem = 0, freeMem = 0, buffers = 0, cached = 0;
        for (const QString &line : memLines) {
            if (line.startsWith("MemTotal:")) {
                totalMem = line.split(QRegularExpression("\\s+"))[1].toLongLong();
            } else if (line.startsWith("MemFree:")) {
                freeMem = line.split(QRegularExpression("\\s+"))[1].toLongLong();
            } else if (line.startsWith("Buffers:")) {
                buffers = line.split(QRegularExpression("\\s+"))[1].toLongLong();
            } else if (line.startsWith("Cached:")) {
                cached = line.split(QRegularExpression("\\s+"))[1].toLongLong();
            }
        }
        qint64 usedMem = totalMem - freeMem - buffers - cached;
        systemInfo["mem_total"] = totalMem / (1024.0 * 1024.0);
        systemInfo["mem_used"] = usedMem / (1024.0 * 1024.0);
        systemInfo["mem_usage"] = (usedMem * 100.0) / totalMem;
        
        process.start("cat", QStringList() << "/proc/loadavg");
        process.waitForFinished();
        QString loadOutput = process.readAllStandardOutput();
        QStringList loadValues = loadOutput.split(QRegularExpression("\\s+"));
        if (loadValues.size() >= 3) {
            systemInfo["load_1"] = loadValues[0].toDouble();
            systemInfo["load_5"] = loadValues[1].toDouble();
            systemInfo["load_15"] = loadValues[2].toDouble();
        }
        
        process.start("cat", QStringList() << "/proc/stat");
        process.waitForFinished();
        QString cpuOutput = process.readAllStandardOutput();
        QStringList cpuLines = cpuOutput.split("\n");
        if (!cpuLines.isEmpty()) {
            QStringList cpuValues = cpuLines[0].split(QRegularExpression("\\s+"));
            if (cpuValues.size() >= 8) {
                qint64 user = cpuValues[1].toLongLong();
                qint64 nice = cpuValues[2].toLongLong();
                qint64 system = cpuValues[3].toLongLong();
                qint64 idle = cpuValues[4].toLongLong();
                qint64 total = user + nice + system + idle;
                systemInfo["cpu_usage"] = ((total - idle) * 100.0) / total;
            }
        }
#elif defined(Q_OS_MACOS)
        // macOS系统信息获取
        QProcess process;
        process.start("top", QStringList() << "-l" << "1" << "-n" << "0");
        process.waitForFinished();
        QString topOutput = process.readAllStandardOutput();
        QRegularExpression cpuRegex("CPU usage: (\\d+\\.\\d+)%");
        QRegularExpressionMatch cpuMatch = cpuRegex.match(topOutput);
        if (cpuMatch.hasMatch()) {
            systemInfo["cpu_usage"] = cpuMatch.captured(1).toDouble();
        }
        
        process.start("vm_stat");
        process.waitForFinished();
        QString vmOutput = process.readAllStandardOutput();
        QRegularExpression memRegex("Pages free: (\\d+)\\.");
        QRegularExpressionMatch memMatch = memRegex.match(vmOutput);
        if (memMatch.hasMatch()) {
            qint64 freePages = memMatch.captured(1).toLongLong();
            qint64 pageSize = 4096; // macOS页面大小
            qint64 freeMem = freePages * pageSize;
            process.start("sysctl", QStringList() << "-n" << "hw.memsize");
            process.waitForFinished();
            QString memSizeOutput = process.readAllStandardOutput().trimmed();
            qint64 totalMem = memSizeOutput.toLongLong();
            qint64 usedMem = totalMem - freeMem;
            systemInfo["mem_total"] = totalMem / (1024.0 * 1024.0 * 1024.0);
            systemInfo["mem_used"] = usedMem / (1024.0 * 1024.0 * 1024.0);
            systemInfo["mem_usage"] = (usedMem * 100.0) / totalMem;
        }
        
        process.start("uptime");
        process.waitForFinished();
        QString uptimeOutput = process.readAllStandardOutput();
        QStringList uptimeParts = uptimeOutput.split(QRegularExpression("\\s+"));
        if (uptimeParts.size() >= 3) {
            systemInfo["load_1"] = uptimeParts[0].toDouble();
            systemInfo["load_5"] = uptimeParts[1].toDouble();
            systemInfo["load_15"] = uptimeParts[2].toDouble();
        }
#else
        // 其他系统，使用默认值
        systemInfo["cpu_usage"] = 0.0;
        systemInfo["mem_total"] = 0.0;
        systemInfo["mem_used"] = 0.0;
        systemInfo["mem_usage"] = 0.0;
        systemInfo["load_1"] = 0.0;
        systemInfo["load_5"] = 0.0;
        systemInfo["load_15"] = 0.0;
        systemInfo["uptime_days"] = 0.0;
        systemInfo["network_rx_mb"] = 0.0;
        systemInfo["network_tx_mb"] = 0.0;
        systemInfo["total_rx_mb"] = 0.0;
        systemInfo["total_tx_mb"] = 0.0;
#endif
        
        if (safeThis) {
            QMetaObject::invokeMethod(safeThis, "updateCharts", Qt::QueuedConnection, Q_ARG(QJsonObject, systemInfo));
        }
        
        QThread::currentThread()->quit();
    });
    
    QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    
    worker->moveToThread(thread);
    thread->start();
}

void ContentTab::updateCharts(const QJsonObject &data)
{
    // 更新系统信息
    if (m_serverIpLabel) {
        m_serverIpLabel->setText("IP地址: " + (data["ip_address"].toString().isEmpty() ? "未知" : data["ip_address"].toString()));
    }
    if (m_osInfoLabel) {
        m_osInfoLabel->setText("操作系统: " + (data["os_info"].toString().isEmpty() ? "未知" : data["os_info"].toString()));
    }
    if (m_uptimeLabel) {
        QString uptime = data["uptime_days"].toDouble() > 0 ? 
            QString::number(data["uptime_days"].toDouble(), 'f', 1) + "天" : 
            (data["uptime"].toString().isEmpty() ? "未知" : data["uptime"].toString());
        m_uptimeLabel->setText("运行时间: " + uptime);
    }
    
    // 更新 CPU Gauge
    double cpuUsage = data["cpu_usage"].toDouble();
    double cpuIdle = 100 - cpuUsage;
    QStringList cpuDetails;
    cpuDetails << QString("CPU使用率: %1%").arg(cpuUsage, 0, 'f', 1)
               << QString("已用: %1%").arg(cpuUsage, 0, 'f', 1)
               << QString("空闲: %1%").arg(cpuIdle, 0, 'f', 1);
    updateGauge(m_cpuGauge, cpuUsage, cpuDetails);
    
    // 更新内存 Gauge
    double memoryUsage = data["mem_usage"].toDouble();
    double memTotal = data["mem_total"].toDouble();
    double memUsed = data["mem_used"].toDouble();
    double memAvailable = memTotal - memUsed;
    QStringList memDetails;
    memDetails << QString("总计: %1 GB").arg(memTotal, 0, 'f', 2)
               << QString("已用: %1 GB").arg(memUsed, 0, 'f', 2)
               << QString("可用: %1 GB").arg(memAvailable, 0, 'f', 2);
    updateGauge(m_memoryGauge, memoryUsage, memDetails);
    
    // 更新磁盘 Gauge
    double diskUsage = data["disk_usage"].toDouble();
    double diskTotal = data["disk_total"].toDouble();
    double diskUsed = data["disk_used"].toDouble();
    double diskAvailable = diskTotal - diskUsed;
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

void ContentTab::updateGauge(GaugeWidget *gauge, double value, const QStringList &details)
{
    if (gauge) {
        gauge->setValue(value);
        gauge->setDetails(details);
    }
}

void ContentTab::updateBarChart(QChart *chart, QBarSeries *series, const QList<double> &data, const QStringList &categories, const QString &title)
{
    if (!chart || !series) return;
    
    series->clear();
    
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
    barSet->setColor(colors[0]);
    
    series->append(barSet);
    
    QList<QAbstractAxis*> verticalAxes = chart->axes(Qt::Vertical);
    if (!verticalAxes.isEmpty()) {
        QValueAxis *axisY = qobject_cast<QValueAxis*>(verticalAxes.first());
        if (axisY) {
            double maxValue = *std::max_element(data.begin(), data.end());
            axisY->setMax(maxValue * 1.2);
        }
    }
}

void ContentTab::refreshSystemInfo()
{
    fetchSystemInfo();
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
    ).arg(theme->colors().BACKGROUND,
          theme->colors().TEXT_PRIMARY,
          ThemeManager::Typography::FONT_FAMILY);
    
    setStyleSheet(styleSheet);
}
