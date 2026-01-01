#include "reportstab.h"
#include "../networkmanager.h"
#include "common.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QPrinter>
#include <QTextDocument>
#include <QPageSetupDialog>
#include <QHeaderView>
#include <QTimer>
#include <QDebug>

ReportsTab::ReportsTab(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    // 加载初始数据
    refreshReports();
    // 设置定时刷新（每30秒）
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ReportsTab::refreshReports);
    timer->start(30000);
}

ReportsTab::~ReportsTab()
{
}

void ReportsTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // 工具栏
    setupToolbar();
    mainLayout->addWidget(createGroupBox("操作", createToolbarWidget()));
    
    // 关键指标卡片
    setupKeyMetrics();
    mainLayout->addWidget(createGroupBox("关键指标", createKeyMetricsWidget()));
    
    // 趋势图表
    setupTrendCharts();
    mainLayout->addWidget(createGroupBox("趋势分析", createTrendChartsWidget()));
    
    // 排行榜（并排显示）
    setupTopRankings();
    QHBoxLayout *rankingsLayout = new QHBoxLayout();
    rankingsLayout->addWidget(createGroupBox("热门页面", createTopPagesWidget()));
    rankingsLayout->addWidget(createGroupBox("热门事件", createTopEventsWidget()));
    rankingsLayout->addWidget(createGroupBox("活跃用户", createTopUsersWidget()));
    mainLayout->addLayout(rankingsLayout);
    
    // 实时统计
    setupRealTimeStats();
    mainLayout->addWidget(createGroupBox("实时数据", createRealTimeStatsWidget()));
}

void ReportsTab::setupToolbar()
{
    // 时间范围选择
    // 设置默认时间范围为包含实际数据的时间范围
    m_startDateEdit = new QDateEdit(QDate(2025, 12, 31));
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDisplayFormat("yyyy-MM-dd");
    connect(m_startDateEdit, &QDateEdit::dateChanged, this, &ReportsTab::onDateRangeChanged);
    
    m_endDateEdit = new QDateEdit(QDate(2026, 1, 1));
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("yyyy-MM-dd");
    connect(m_endDateEdit, &QDateEdit::dateChanged, this, &ReportsTab::onDateRangeChanged);
    
    // 报表类型选择
    m_reportTypeCombo = new QComboBox();
    m_reportTypeCombo->addItem("事件总数", "events");
    m_reportTypeCombo->addItem("日活跃用户", "activity");
    m_reportTypeCombo->addItem("页面访问量", "pageview");
    m_reportTypeCombo->addItem("唯一用户数", "unique_users");
    m_reportTypeCombo->addItem("留存率", "retention");
    m_reportTypeCombo->addItem("性能指标", "performance");
    connect(m_reportTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ReportsTab::onReportTypeChanged);
    
    // 图表类型选择
    m_chartTypeCombo = new QComboBox();
    m_chartTypeCombo->addItem("折线图", "line");
    m_chartTypeCombo->addItem("柱状图", "bar");
    // 连接图表类型改变信号，重新渲染图表
    connect(m_chartTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ReportsTab::onChartTypeChanged);
    
    // 操作按钮
    m_refreshButton = new QPushButton("刷新");
    connect(m_refreshButton, &QPushButton::clicked, this, &ReportsTab::refreshReports);
    
    m_exportButton = new QPushButton("导出");
    connect(m_exportButton, &QPushButton::clicked, this, &ReportsTab::exportReport);
    
    m_exportCSVButton = new QPushButton("导出CSV");
    connect(m_exportCSVButton, &QPushButton::clicked, this, &ReportsTab::exportToCSV);
    
    m_exportExcelButton = new QPushButton("导出Excel");
    connect(m_exportExcelButton, &QPushButton::clicked, this, &ReportsTab::exportToExcel);
    
    m_exportPDFButton = new QPushButton("导出PDF");
    connect(m_exportPDFButton, &QPushButton::clicked, this, &ReportsTab::exportToPDF);
}

void ReportsTab::setupKeyMetrics()
{
    m_dauLabel = new QLabel("DAU: -");
    m_mauLabel = new QLabel("MAU: -");
    m_retentionLabel = new QLabel("留存率: -");
    m_conversionLabel = new QLabel("转化率: -");
    m_onlineUsersLabel = new QLabel("在线用户: -");
    m_todayEventsLabel = new QLabel("今日事件: -");
    m_totalEventsLabel = new QLabel("总事件数: -");
    
    // 设置样式
    QString metricStyle = "QLabel { padding: 15px; border-radius: 10px; font-size: 16px; font-weight: bold; background-color: #f8f9fa; }";
    m_dauLabel->setStyleSheet(metricStyle);
    m_mauLabel->setStyleSheet(metricStyle);
    m_retentionLabel->setStyleSheet(metricStyle);
    m_conversionLabel->setStyleSheet(metricStyle);
    m_onlineUsersLabel->setStyleSheet(metricStyle);
    m_todayEventsLabel->setStyleSheet(metricStyle);
    m_totalEventsLabel->setStyleSheet(metricStyle);
    
    QGridLayout *metricsLayout = new QGridLayout();
    metricsLayout->addWidget(m_dauLabel, 0, 0);
    metricsLayout->addWidget(m_mauLabel, 0, 1);
    metricsLayout->addWidget(m_retentionLabel, 0, 2);
    metricsLayout->addWidget(m_conversionLabel, 1, 0);
    metricsLayout->addWidget(m_onlineUsersLabel, 1, 1);
    metricsLayout->addWidget(m_todayEventsLabel, 1, 2);
    metricsLayout->addWidget(m_totalEventsLabel, 2, 0, 1, 3); // 总事件数占满第三行
}

void ReportsTab::setupTrendCharts()
{
    m_trendChartView = new QWebEngineView();
    m_trendChartView->setMinimumHeight(400);
    
    m_activityChartView = new QWebEngineView();
    m_activityChartView->setMinimumHeight(300);
}

void ReportsTab::setupTopRankings()
{
    // 热门页面表格
    m_topPagesTable = new QTableWidget();
    m_topPagesTable->setColumnCount(3);
    m_topPagesTable->setHorizontalHeaderLabels(QStringList() << "排名" << "页面名称" << "访问次数");
    m_topPagesTable->horizontalHeader()->setStretchLastSection(true);
    m_topPagesTable->verticalHeader()->setVisible(false);
    m_topPagesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_topPagesTable->setAlternatingRowColors(true);
    
    // 热门事件表格
    m_topEventsTable = new QTableWidget();
    m_topEventsTable->setColumnCount(3);
    m_topEventsTable->setHorizontalHeaderLabels(QStringList() << "排名" << "事件名称" << "触发次数");
    m_topEventsTable->horizontalHeader()->setStretchLastSection(true);
    m_topEventsTable->verticalHeader()->setVisible(false);
    m_topEventsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_topEventsTable->setAlternatingRowColors(true);
    
    // 活跃用户表格
    m_topUsersTable = new QTableWidget();
    m_topUsersTable->setColumnCount(3);
    m_topUsersTable->setHorizontalHeaderLabels(QStringList() << "排名" << "用户ID" << "活跃度");
    m_topUsersTable->horizontalHeader()->setStretchLastSection(true);
    m_topUsersTable->verticalHeader()->setVisible(false);
    m_topUsersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_topUsersTable->setAlternatingRowColors(true);
}

void ReportsTab::setupRealTimeStats()
{
    // 实时数据标签已在setupKeyMetrics中创建
}

void ReportsTab::refreshReports()
{
    loadActivityStats();
    loadRetentionStats();
    loadPageViewStats();
    loadEventStats();
    loadTrendAnalysis();
    loadTopPages();
    loadTopEvents();
    loadTopUsers();
    loadRealTimeStats();
}

void ReportsTab::loadActivityStats()
{
    NetworkManager *networkManager = new NetworkManager(this);
    QString startDate = m_startDateEdit->date().toString("yyyy-MM-dd");
    QString endDate = m_endDateEdit->date().toString("yyyy-MM-dd");
    
    networkManager->getActivityStats(startDate, endDate,
        [this](const QJsonObject &response) {
            updateKeyMetricsDisplay(response["data"].toObject());
        },
        [this](const QString &error) {
            qWarning() << "加载活跃度统计失败:" << error;
        });
    
    // 加载事件统计以获取总事件数
    networkManager = new NetworkManager(this);
    networkManager->getEventStats(startDate, endDate,
        [this](const QJsonObject &response) {
            QJsonArray eventStats = response["data"].toArray();
            int totalEvents = 0;
            for (const QJsonValue &value : eventStats) {
                totalEvents += value.toObject()["count"].toInt();
            }
            // 更新总事件数标签
            m_totalEventsLabel->setText(QString("总事件数: %1").arg(totalEvents));
        },
        [this](const QString &error) {
            qWarning() << "加载事件统计失败:" << error;
        });
}

void ReportsTab::loadRetentionStats()
{
    NetworkManager *networkManager = new NetworkManager(this);
    
    networkManager->getRetentionStats(
        [this](const QJsonObject &response) {
           // LOG_DEBUG() << "留存率API返回数据:" << QJsonDocument(response).toJson(QJsonDocument::Compact);
            QJsonObject data = response["data"].toObject();
          //  LOG_DEBUG() << "留存率data数据:" << QJsonDocument(data).toJson(QJsonDocument::Compact);
            
            // API返回的是day1Retention、day7Retention、day30Retention三个字段
            // 优先显示7日留存率
            double retention = 0.0;
            QString retentionKey;
            
            if (data.contains("day7Retention")) {
                retentionKey = "day7Retention";
            } else if (data.contains("day1Retention")) {
                retentionKey = "day1Retention";
            } else if (data.contains("day30Retention")) {
                retentionKey = "day30Retention";
            }
            
            if (!retentionKey.isEmpty()) {
                // API返回的是字符串类型，需要先转换为QString，再转换为double
                retention = data[retentionKey].toString().toDouble();
            }
            
            m_retentionLabel->setText(QString("留存率: %1%").arg(retention, 0, 'f', 2));
        },
        [this](const QString &error) {
            qWarning() << "加载留存率统计失败:" << error;
        });
}

void ReportsTab::loadPageViewStats()
{
    NetworkManager *networkManager = new NetworkManager(this);
    QString startDate = m_startDateEdit->date().toString("yyyy-MM-dd");
    QString endDate = m_endDateEdit->date().toString("yyyy-MM-dd");
    
    networkManager->getPageViewStats(startDate, endDate,
        [this](const QJsonObject &response) {
            QJsonArray stats = response["data"].toArray();
            updateTopPagesTable(stats);
        },
        [this](const QString &error) {
            qWarning() << "加载页面访问统计失败:" << error;
        });
}

void ReportsTab::loadEventStats()
{
    NetworkManager *networkManager = new NetworkManager(this);
    QString startDate = m_startDateEdit->date().toString("yyyy-MM-dd");
    QString endDate = m_endDateEdit->date().toString("yyyy-MM-dd");
    
    networkManager->getEventStats(startDate, endDate,
        [this](const QJsonObject &response) {
            QJsonArray stats = response["data"].toArray();
            updateTopEventsTable(stats);
        },
        [this](const QString &error) {
            qWarning() << "加载事件统计失败:" << error;
        });
}

void ReportsTab::loadTrendAnalysis()
{
    NetworkManager *networkManager = new NetworkManager(this);
    QString startDate = m_startDateEdit->date().toString("yyyy-MM-dd");
    QString endDate = m_endDateEdit->date().toString("yyyy-MM-dd");
    QString reportType = m_reportTypeCombo->currentData().toString();
    QString title = m_reportTypeCombo->currentText();
    
    // 将前端报表类型映射到后端metric类型
    // 根据API文档，metric参数支持：events, dau, page_views, unique_users, retention, performance
    QString metric;
    if (reportType == "events") {
        metric = "events";  // 事件总数趋势
    } else if (reportType == "activity") {
        metric = "dau";  // 日活跃用户数趋势
    } else if (reportType == "pageview") {
        metric = "page_views";  // 页面访问量趋势
    } else if (reportType == "unique_users") {
        metric = "unique_users";  // 唯一用户数趋势
    } else if (reportType == "retention") {
        metric = "retention";  // 留存率趋势
    } else if (reportType == "performance") {
        metric = "performance";  // 性能指标趋势
    } else {
        metric = "events";  // 默认为事件总数
    }
    
    // 调用统一的trends接口
    networkManager->getTrendAnalysis(metric, startDate, endDate,
        [this, title, metric](const QJsonObject &response) {
            QJsonArray trendData = response["data"].toArray();
            
            // 根据不同的metric类型，提取相应的数值字段
            QJsonArray chartData;
            for (const QJsonValue &value : trendData) {
                QJsonObject item = value.toObject();
                QJsonObject chartItem;
                
                // 提取时间
                QString timeBucket = item["timeBucket"].toString();
                chartItem["time_bucket"] = timeBucket.left(10);
                
                // 根据metric类型提取对应的数值字段
                if (metric == "events") {
                    // 事件总数趋势：使用count字段
                    chartItem["count"] = item["count"].toInt();
                } else if (metric == "dau") {
                    // 日活跃用户数趋势：使用dau字段
                    chartItem["count"] = item["dau"].toInt();
                } else if (metric == "page_views") {
                    // 页面访问量趋势：使用pageViews字段
                    chartItem["count"] = item["pageViews"].toInt();
                } else if (metric == "unique_users") {
                    // 唯一用户数趋势：使用uniqueUsers字段
                    chartItem["count"] = item["uniqueUsers"].toInt();
                } else if (metric == "retention") {
                    // 留存率趋势：优先使用次日留存率，其次使用7日留存率
                    double retentionValue = 0.0;
                    if (item.contains("day1RetentionRate")) {
                        // 先转换为QString，再转换为double，确保字符串类型数值能正确转换
                        retentionValue = item["day1RetentionRate"].toString().toDouble();
                    } else if (item.contains("day7RetentionRate")) {
                        // 先转换为QString，再转换为double，确保字符串类型数值能正确转换
                        retentionValue = item["day7RetentionRate"].toString().toDouble();
                    }
                    // 保留小数精度，乘以100转换为百分比数值用于图表显示
                    // 直接赋值，QJsonObject会自动将double转换为QJsonValue::Double类型
                    chartItem["count"] = retentionValue;
                } else if (metric == "performance") {
                    // 性能指标趋势：使用avgDuration字段
                    chartItem["count"] = static_cast<int>(item["avgDuration"].toString().toDouble());
                } else {
                    // 默认：使用count字段
                    chartItem["count"] = item["count"].toInt();
                }
                
                chartData.append(chartItem);
            }
            
            updateTrendChart(chartData, title);
        },
        [this](const QString &error) {
            qWarning() << "加载趋势分析失败:" << error;
        });
}

void ReportsTab::loadTopPages()
{
    NetworkManager *networkManager = new NetworkManager(this);
    QString startDate = m_startDateEdit->date().toString("yyyy-MM-dd");
    QString endDate = m_endDateEdit->date().toString("yyyy-MM-dd");
    
    networkManager->getTopPages(startDate, endDate, 10,
        [this](const QJsonObject &response) {
            QJsonArray pages = response["data"].toArray();
            updateTopPagesTable(pages);
        },
        [this](const QString &error) {
            qWarning() << "加载热门页面失败:" << error;
        });
}

void ReportsTab::loadTopEvents()
{
    NetworkManager *networkManager = new NetworkManager(this);
    QString startDate = m_startDateEdit->date().toString("yyyy-MM-dd");
    QString endDate = m_endDateEdit->date().toString("yyyy-MM-dd");
    
    networkManager->getTopEvents(startDate, endDate, 10,
        [this](const QJsonObject &response) {
            QJsonArray events = response["data"].toArray();
            updateTopEventsTable(events);
        },
        [this](const QString &error) {
            qWarning() << "加载热门事件失败:" << error;
        });
}

void ReportsTab::loadTopUsers()
{
    NetworkManager *networkManager = new NetworkManager(this);
    
    networkManager->getTopUsers(1, 10,
        [this](const QJsonObject &response) {
            // 处理活跃用户API响应
    QJsonObject data = response["data"].toObject();
    
    // 尝试获取users或list字段
    QJsonArray users;
    if (data.contains("list")) {
        users = data["list"].toArray();
    } else if (data.contains("users")) {
        users = data["users"].toArray();
    } else {
        qWarning() << "data对象中既没有list字段也没有users字段";
    }
            
            // 检查用户对象是否为空
            bool hasEmptyUsers = false;
            for (int i = 0; i < users.size(); ++i) {
                QJsonObject user = users[i].toObject();
                if (user.isEmpty()) {
                    hasEmptyUsers = true;
                    break;
                }
            }
            
            if (hasEmptyUsers) {
                qWarning() << "警告：检测到空的用户对象，后端数据异常！";
                qWarning() << "可能原因：";
                qWarning() << "1. user_profiles表缺少数据";
                qWarning() << "2. 后端getUserList方法未正确填充用户数据";
                qWarning() << "3. 数据库查询返回了空记录";
            }
            
            // 处理用户数据
            
            updateTopUsersTable(users);
        },
        [this](const QString &error) {
            qWarning() << "加载用户列表失败:" << error;
        });
}

void ReportsTab::loadRealTimeStats()
{
    NetworkManager *networkManager = new NetworkManager(this);
    
    networkManager->getRealTimeStats(
        [this](const QJsonObject &response) {
            updateRealTimeStats(response["data"].toArray());
        },
        [this](const QString &error) {
            qWarning() << "加载实时统计失败:" << error;
        });
}

void ReportsTab::updateKeyMetricsDisplay(const QJsonObject &metrics)
{
    // 更新关键指标显示
    QJsonArray dauStats = metrics["dauStats"].toArray();
    QJsonArray mauStats = metrics["mauStats"].toArray();
    
    int latestDau = 0;
    int latestMau = 0;
    
    if (!dauStats.isEmpty()) {
        latestDau = dauStats.last().toObject()["dau"].toInt();
        m_dauLabel->setText(QString("DAU: %1").arg(latestDau));
    }
    
    if (!mauStats.isEmpty()) {
        latestMau = mauStats.last().toObject()["mau"].toInt();
        m_mauLabel->setText(QString("MAU: %1").arg(latestMau));
    }
    
    // 计算转化率：DAU / MAU
    double conversionRate = 0.0;
    if (latestMau > 0) {
        conversionRate = (static_cast<double>(latestDau) / latestMau) * 100.0;
    }
    m_conversionLabel->setText(QString("转化率: %1%").arg(conversionRate, 0, 'f', 2));
}

void ReportsTab::updateTrendChart(const QJsonArray &trendData, const QString &title)
{
    QString chartType = m_chartTypeCombo->currentData().toString();
    renderTrendChart(trendData, title, chartType);
}

void ReportsTab::updateTopPagesTable(const QJsonArray &pages)
{
    m_topPagesTable->setRowCount(pages.size());
    for (int i = 0; i < pages.size(); ++i) {
        QJsonObject page = pages[i].toObject();
        m_topPagesTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        m_topPagesTable->setItem(i, 1, new QTableWidgetItem(page["pageUrl"].toString()));
        m_topPagesTable->setItem(i, 2, new QTableWidgetItem(QString::number(page["pv"].toInt())));
    }
}

void ReportsTab::updateTopEventsTable(const QJsonArray &events)
{
    m_topEventsTable->setRowCount(events.size());
    for (int i = 0; i < events.size(); ++i) {
        QJsonObject event = events[i].toObject();
        m_topEventsTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        m_topEventsTable->setItem(i, 1, new QTableWidgetItem(event["eventName"].toString()));
        m_topEventsTable->setItem(i, 2, new QTableWidgetItem(QString::number(event["count"].toInt())));
    }
}

void ReportsTab::updateTopUsersTable(const QJsonArray &users)
{
    // 更新活跃用户表格
    m_topUsersTable->setRowCount(users.size());
    
    for (int i = 0; i < users.size(); ++i) {
        QJsonObject user = users[i].toObject();
        
        // 使用正确的驼峰命名字段名
        QString userId = user["userId"].toString();
        QString activityLevel = user["activityLevel"].toString();
        
        // 如果用户对象为空，显示提示信息
        if (user.isEmpty()) {
            userId = "数据缺失";
            activityLevel = "数据缺失";
        }
        
        m_topUsersTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        m_topUsersTable->setItem(i, 1, new QTableWidgetItem(userId));
        m_topUsersTable->setItem(i, 2, new QTableWidgetItem(activityLevel));
    }
}

void ReportsTab::updateRealTimeStats(const QJsonArray &stats)
{
    int todayEvents = 0;
    int uniqueUsers = 0;
    
    if (!stats.isEmpty()) {
        // 获取第一条数据（今日的统计）
        QJsonObject todayData = stats.first().toObject();
        todayEvents = todayData["count"].toInt();
        uniqueUsers = todayData["uniqueUsers"].toInt();
    }
    
    // 在线用户数使用独立用户数作为近似值
    m_onlineUsersLabel->setText(QString("在线用户: %1").arg(uniqueUsers));
    m_todayEventsLabel->setText(QString("今日事件: %1").arg(todayEvents));
}

void ReportsTab::renderTrendChart(const QJsonArray &trendData, const QString &title, const QString &chartType)
{
    QString xAxisData;
    QString seriesData;
    
    for (const QJsonValue &value : trendData) {
        QJsonObject item = value.toObject();
        QString timeBucket = item["time_bucket"].toString();
        
        // 检查count字段的类型
        QJsonValue countValue = item["count"];
        double count = countValue.toDouble();
        
        xAxisData += QString("'%1',").arg(timeBucket.left(10)); // 只取日期部分
        seriesData += QString("%1,").arg(count);
    }
    
    if (!xAxisData.isEmpty()) xAxisData.chop(1);
    if (!seriesData.isEmpty()) seriesData.chop(1);
    
    QString html = QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <script src="qrc:/echarts/ECharts/echarts.min.js"></script>
        </head>
        <body style="margin:0;padding:0;">
            <div id="chart" style="width:100%%;height:400px;"></div>
            <script>
                var chart = echarts.init(document.getElementById('chart'));
                var option = {
                    title: { text: '%1', left: 'center' },
                    tooltip: { trigger: 'axis' },
                    legend: { data: ['数值'], top: 30 },
                    xAxis: { type: 'category', data: [%2] },
                    yAxis: { type: 'value' },
                    series: [{
                        name: '数值',
                        type: '%3',
                        data: [%4],
                        smooth: true,
                        areaStyle: { opacity: 0.3 }
                    }]
                };
                chart.setOption(option);
            </script>
        </body>
        </html>
    )").arg(title).arg(xAxisData).arg(chartType).arg(seriesData);
    
    m_trendChartView->setHtml(html);
}

void ReportsTab::onReportTypeChanged(int index)
{
    Q_UNUSED(index);
    loadTrendAnalysis();
}

void ReportsTab::onChartTypeChanged(int index)
{
    Q_UNUSED(index);
    // 重新加载趋势分析数据，使用新的图表类型渲染
    loadTrendAnalysis();
}

void ReportsTab::onDateRangeChanged()
{
    refreshReports();
}

void ReportsTab::exportReport()
{
    // 默认导出为CSV
    exportToCSV();
}

void ReportsTab::exportToCSV()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出报表", 
        QString("report_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")), 
        "CSV文件 (*.csv)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + fileName);
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // 写入关键指标
    out << "报表导出时间," << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    out << "时间范围," << m_startDateEdit->date().toString("yyyy-MM-dd") << " 至 " 
        << m_endDateEdit->date().toString("yyyy-MM-dd") << "\n";
    out << "\n";
    
    // 写入关键指标
    out << "关键指标\n";
    out << "指标名称,数值\n";
    out << "DAU," << m_dauLabel->text().replace("DAU: ", "") << "\n";
    out << "MAU," << m_mauLabel->text().replace("MAU: ", "") << "\n";
    out << "留存率," << m_retentionLabel->text().replace("留存率: ", "") << "\n";
    out << "在线用户," << m_onlineUsersLabel->text().replace("在线用户: ", "") << "\n";
    out << "今日事件," << m_todayEventsLabel->text().replace("今日事件: ", "") << "\n";
    out << "\n";
    
    // 写入热门页面
    out << "热门页面\n";
    out << "排名,页面名称,访问次数\n";
    for (int i = 0; i < m_topPagesTable->rowCount(); ++i) {
        out << i + 1 << ","
            << m_topPagesTable->item(i, 1)->text() << ","
            << m_topPagesTable->item(i, 2)->text() << "\n";
    }
    out << "\n";
    
    // 写入热门事件
    out << "热门事件\n";
    out << "排名,事件名称,触发次数\n";
    for (int i = 0; i < m_topEventsTable->rowCount(); ++i) {
        out << i + 1 << ","
            << m_topEventsTable->item(i, 1)->text() << ","
            << m_topEventsTable->item(i, 2)->text() << "\n";
    }
    
    file.close();
    QMessageBox::information(this, "成功", "报表导出成功!");
}

void ReportsTab::exportToExcel()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出报表为Excel", 
        QString("report_%1.xlsx").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")), 
        "Excel文件 (*.xlsx)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // 使用CSV格式但保存为.xlsx文件（可以被Excel打开）
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + fileName);
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // 写入UTF-8 BOM以确保Excel正确显示中文
    out << "\xEF\xBB\xBF";
    
    // 写入标题行
    out << "报表导出时间," << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    out << "时间范围," << m_startDateEdit->date().toString("yyyy-MM-dd") << " 至 " 
        << m_endDateEdit->date().toString("yyyy-MM-dd") << "\n";
    out << "\n";
    
    // 写入关键指标
    out << "关键指标\n";
    out << "指标名称,数值\n";
    out << "DAU," << m_dauLabel->text().replace("DAU: ", "") << "\n";
    out << "MAU," << m_mauLabel->text().replace("MAU: ", "") << "\n";
    out << "留存率," << m_retentionLabel->text().replace("留存率: ", "") << "\n";
    out << "在线用户," << m_onlineUsersLabel->text().replace("在线用户: ", "") << "\n";
    out << "今日事件," << m_todayEventsLabel->text().replace("今日事件: ", "") << "\n";
    out << "\n";
    
    // 写入热门页面
    out << "热门页面\n";
    out << "排名,页面名称,访问次数\n";
    for (int i = 0; i < m_topPagesTable->rowCount(); ++i) {
        out << i + 1 << ","
            << m_topPagesTable->item(i, 1)->text() << ","
            << m_topPagesTable->item(i, 2)->text() << "\n";
    }
    out << "\n";
    
    // 写入热门事件
    out << "热门事件\n";
    out << "排名,事件名称,触发次数\n";
    for (int i = 0; i < m_topEventsTable->rowCount(); ++i) {
        out << i + 1 << ","
            << m_topEventsTable->item(i, 1)->text() << ","
            << m_topEventsTable->item(i, 2)->text() << "\n";
    }
    
    file.close();
    QMessageBox::information(this, "成功", "报表导出成功!\n\n注意：文件采用CSV格式保存为.xlsx扩展名，可被Excel打开。\n如需真正的Excel格式，请使用专业库（如QXlsx）。");
}

void ReportsTab::exportToPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出报表为PDF", 
        QString("report_%1.pdf").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")), 
        "PDF文件 (*.pdf)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // 创建HTML内容
    QString html = QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <style>
                body { font-family: Arial, sans-serif; margin: 20px; }
                h1 { color: #333; border-bottom: 2px solid #4CAF50; padding-bottom: 10px; }
                h2 { color: #666; margin-top: 30px; }
                table { border-collapse: collapse; width: 100%; margin-top: 10px; }
                th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
                th { background-color: #4CAF50; color: white; }
                tr:nth-child(even) { background-color: #f2f2f2; }
                .info { background-color: #e7f3fe; padding: 10px; border-left: 6px solid #2196F3; margin-bottom: 20px; }
                .metric { display: inline-block; margin: 10px 20px 10px 0; padding: 10px; background-color: #f9f9f9; border-radius: 5px; }
                .metric-label { font-weight: bold; color: #666; }
                .metric-value { font-size: 18px; color: #4CAF50; }
            </style>
        </head>
        <body>
            <h1>数据分析报表</h1>
            
            <div class="info">
                <strong>导出时间:</strong> %1<br>
                <strong>时间范围:</strong> %2 至 %3
            </div>
            
            <h2>关键指标</h2>
            <div>
                <div class="metric">
                    <span class="metric-label">DAU:</span>
                    <span class="metric-value">%4</span>
                </div>
                <div class="metric">
                    <span class="metric-label">MAU:</span>
                    <span class="metric-value">%5</span>
                </div>
                <div class="metric">
                    <span class="metric-label">留存率:</span>
                    <span class="metric-value">%6</span>
                </div>
                <div class="metric">
                    <span class="metric-label">在线用户:</span>
                    <span class="metric-value">%7</span>
                </div>
                <div class="metric">
                    <span class="metric-label">今日事件:</span>
                    <span class="metric-value">%8</span>
                </div>
            </div>
            
            <h2>热门页面</h2>
            <table>
                <thead>
                    <tr>
                        <th>排名</th>
                        <th>页面名称</th>
                        <th>访问次数</th>
                    </tr>
                </thead>
                <tbody>
    )").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
      .arg(m_startDateEdit->date().toString("yyyy-MM-dd"))
      .arg(m_endDateEdit->date().toString("yyyy-MM-dd"))
      .arg(m_dauLabel->text().replace("DAU: ", ""))
      .arg(m_mauLabel->text().replace("MAU: ", ""))
      .arg(m_retentionLabel->text().replace("留存率: ", ""))
      .arg(m_onlineUsersLabel->text().replace("在线用户: ", ""))
      .arg(m_todayEventsLabel->text().replace("今日事件: ", ""));
    
    // 添加热门页面数据
    for (int i = 0; i < m_topPagesTable->rowCount(); ++i) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
            .arg(i + 1)
            .arg(m_topPagesTable->item(i, 1)->text())
            .arg(m_topPagesTable->item(i, 2)->text());
    }
    
    // 添加热门事件表格
    html += R"(
                </tbody>
            </table>
            
            <h2>热门事件</h2>
            <table>
                <thead>
                    <tr>
                        <th>排名</th>
                        <th>事件名称</th>
                        <th>触发次数</th>
                    </tr>
                </thead>
                <tbody>
    )";
    
    // 添加热门事件数据
    for (int i = 0; i < m_topEventsTable->rowCount(); ++i) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
            .arg(i + 1)
            .arg(m_topEventsTable->item(i, 1)->text())
            .arg(m_topEventsTable->item(i, 2)->text());
    }
    
    html += R"(
                </tbody>
            </table>
        </body>
        </html>
    )";
    
    // 创建PDF文档
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);
    
    // 使用QTextDocument渲染HTML为PDF
    QTextDocument document;
    document.setHtml(html);
    document.print(&printer);
    
    QMessageBox::information(this, "成功", "报表导出成功!");
}

QGroupBox *ReportsTab::createGroupBox(const QString &title, QWidget *content)
{
    QGroupBox *groupBox = new QGroupBox(title);
    QVBoxLayout *layout = new QVBoxLayout(groupBox);
    layout->addWidget(content);
    return groupBox;
}

QWidget *ReportsTab::createToolbarWidget()
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    layout->addWidget(new QLabel("开始日期:"));
    layout->addWidget(m_startDateEdit);
    layout->addWidget(new QLabel("结束日期:"));
    layout->addWidget(m_endDateEdit);
    layout->addWidget(new QLabel("报表类型:"));
    layout->addWidget(m_reportTypeCombo);
    layout->addWidget(new QLabel("图表类型:"));
    layout->addWidget(m_chartTypeCombo);
    layout->addWidget(m_refreshButton);
    layout->addWidget(m_exportButton);
    layout->addWidget(m_exportCSVButton);
    layout->addWidget(m_exportExcelButton);
    layout->addWidget(m_exportPDFButton);
    layout->addStretch();
    
    return widget;
}

QWidget *ReportsTab::createKeyMetricsWidget()
{
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    layout->addWidget(m_dauLabel, 0, 0);
    layout->addWidget(m_mauLabel, 0, 1);
    layout->addWidget(m_retentionLabel, 0, 2);
    layout->addWidget(m_conversionLabel, 1, 0);
    layout->addWidget(m_onlineUsersLabel, 1, 1);
    layout->addWidget(m_todayEventsLabel, 1, 2);
    
    return widget;
}

QWidget *ReportsTab::createTrendChartsWidget()
{
    return m_trendChartView;
}

QWidget *ReportsTab::createTopPagesWidget()
{
    return m_topPagesTable;
}

QWidget *ReportsTab::createTopEventsWidget()
{
    return m_topEventsTable;
}

QWidget *ReportsTab::createTopUsersWidget()
{
    return m_topUsersTable;
}

QWidget *ReportsTab::createRealTimeStatsWidget()
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    layout->addWidget(m_onlineUsersLabel);
    layout->addWidget(m_todayEventsLabel);
    layout->addStretch();
    
    return widget;
}