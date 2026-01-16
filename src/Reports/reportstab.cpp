#include "reportstab.h"
#include "../Network/networkmanager.h"
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
#include <QStandardPaths>
#include <QCoreApplication>

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
    
    // 连接主题变化信号
    connect(ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &ReportsTab::applyTheme);
}

ReportsTab::~ReportsTab()
{
}

void ReportsTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);
    
    // 工具栏 - 现代化控制面板
    setupToolbar();
    mainLayout->addWidget(createModernCard("", createToolbarWidget(), false));
    
    // 关键指标卡片 - 仪表盘风格
    setupKeyMetrics();
    mainLayout->addWidget(createModernCard("关键指标", createKeyMetricsWidget()));
    
    // 趋势图表 - 图表卡片
    setupTrendCharts();
    mainLayout->addWidget(createModernCard("趋势分析", createTrendChartsWidget()));
    
    // 排行榜（并排显示）- 数据表格卡片
    setupTopRankings();
    QHBoxLayout *rankingsLayout = new QHBoxLayout();
    rankingsLayout->setSpacing(20);
    rankingsLayout->addWidget(createModernCard("热门页面", createTopPagesWidget()));
    rankingsLayout->addWidget(createModernCard("热门事件", createTopEventsWidget()));
    rankingsLayout->addWidget(createModernCard("活跃用户", createTopUsersWidget()));
    mainLayout->addLayout(rankingsLayout);
    
    // 实时统计 - 实时数据卡片
    setupRealTimeStats();
    mainLayout->addWidget(createModernCard("实时数据", createRealTimeStatsWidget()));
    
    // 应用主题
    applyTheme();
}

void ReportsTab::setupToolbar()
{
    // 时间范围选择
    // 设置默认时间范围为包含实际数据的时间范围
    m_startDateEdit = new QDateEdit(QDate(2025, 12, 31));
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDisplayFormat("yyyy-MM-dd");
    m_startDateEdit->setObjectName("dateEdit");
    connect(m_startDateEdit, &QDateEdit::dateChanged, this, &ReportsTab::onDateRangeChanged);
    
    m_endDateEdit = new QDateEdit(QDate(2026, 1, 1));
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("yyyy-MM-dd");
    m_endDateEdit->setObjectName("dateEdit");
    connect(m_endDateEdit, &QDateEdit::dateChanged, this, &ReportsTab::onDateRangeChanged);
    
    // 报表类型选择
    m_reportTypeCombo = new QComboBox();
    m_reportTypeCombo->setObjectName("comboBox");
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
    m_chartTypeCombo->setObjectName("comboBox");
    m_chartTypeCombo->addItem("折线图", "line");
    m_chartTypeCombo->addItem("柱状图", "bar");
    // 连接图表类型改变信号，重新渲染图表
    connect(m_chartTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ReportsTab::onChartTypeChanged);
    
    // 操作按钮
    m_refreshButton = new QPushButton("刷新");
    m_refreshButton->setObjectName("primaryButton");
    connect(m_refreshButton, &QPushButton::clicked, this, &ReportsTab::refreshReports);
    
    m_exportButton = new QPushButton("导出");
    m_exportButton->setObjectName("secondaryButton");
    connect(m_exportButton, &QPushButton::clicked, this, &ReportsTab::exportReport);
    
    m_exportCSVButton = new QPushButton("导出CSV");
    m_exportCSVButton->setObjectName("secondaryButton");
    connect(m_exportCSVButton, &QPushButton::clicked, this, &ReportsTab::exportToCSV);
    
    m_exportExcelButton = new QPushButton("导出Excel");
    m_exportExcelButton->setObjectName("secondaryButton");
    connect(m_exportExcelButton, &QPushButton::clicked, this, &ReportsTab::exportToExcel);
    
    m_exportPDFButton = new QPushButton("导出PDF");
    m_exportPDFButton->setObjectName("secondaryButton");
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
    
    // 设置对象名称用于主题样式
    m_dauLabel->setObjectName("metricLabel");
    m_mauLabel->setObjectName("metricLabel");
    m_retentionLabel->setObjectName("metricLabel");
    m_conversionLabel->setObjectName("metricLabel");
    m_onlineUsersLabel->setObjectName("metricLabel");
    m_todayEventsLabel->setObjectName("metricLabel");
    m_totalEventsLabel->setObjectName("metricLabel");
}

void ReportsTab::setupTrendCharts()
{
#ifdef WEBENGINE_AVAILABLE
    m_trendChartView = new QWebEngineView();
    m_trendChartView->setMinimumHeight(450);
    m_trendChartView->setObjectName("chartView");
    
    m_activityChartView = new QWebEngineView();
    m_activityChartView->setMinimumHeight(350);
    m_activityChartView->setObjectName("chartView");
#else
    // 创建更友好的趋势图表替代界面
    m_trendChartView = new QLabel("当前平台不支持 WebEngine 组件\n趋势图表功能已禁用");
    m_trendChartView->setAlignment(Qt::AlignCenter);
    m_trendChartView->setStyleSheet("QLabel { color: #666; font-size: 12px; line-height: 1.5; font-size: 16px; font-weight: bold; color: #333; margin: 10px; }");
    m_trendChartView->setMinimumHeight(450);
    m_trendChartView->setObjectName("chartView");
    
    // 创建更友好的活动图表替代界面
    m_activityChartView = new QLabel("当前平台不支持 WebEngine 组件\n活动图表功能已禁用");
    m_activityChartView->setAlignment(Qt::AlignCenter);
    m_activityChartView->setStyleSheet("QLabel { color: #666; font-size: 12px; line-height: 1.5; font-size: 16px; font-weight: bold; color: #333; margin: 10px; }");
    m_activityChartView->setMinimumHeight(350);
    m_activityChartView->setObjectName("chartView");
#endif
}

void ReportsTab::setupTopRankings()
{
    // 热门页面表格
    m_topPagesTable = new QTableWidget();
    m_topPagesTable->setObjectName("dataTable");
    m_topPagesTable->setColumnCount(3);
    m_topPagesTable->setHorizontalHeaderLabels(QStringList() << "排名" << "页面名称" << "访问次数");
    m_topPagesTable->horizontalHeader()->setStretchLastSection(true);
    m_topPagesTable->verticalHeader()->setVisible(false);
    m_topPagesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_topPagesTable->setAlternatingRowColors(true);
    m_topPagesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_topPagesTable->setMinimumHeight(300);
    
    // 热门事件表格
    m_topEventsTable = new QTableWidget();
    m_topEventsTable->setObjectName("dataTable");
    m_topEventsTable->setColumnCount(3);
    m_topEventsTable->setHorizontalHeaderLabels(QStringList() << "排名" << "事件名称" << "触发次数");
    m_topEventsTable->horizontalHeader()->setStretchLastSection(true);
    m_topEventsTable->verticalHeader()->setVisible(false);
    m_topEventsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_topEventsTable->setAlternatingRowColors(true);
    m_topEventsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_topEventsTable->setMinimumHeight(300);
    
    // 活跃用户表格
    m_topUsersTable = new QTableWidget();
    m_topUsersTable->setObjectName("dataTable");
    m_topUsersTable->setColumnCount(3);
    m_topUsersTable->setHorizontalHeaderLabels(QStringList() << "排名" << "用户ID" << "活跃度");
    m_topUsersTable->horizontalHeader()->setStretchLastSection(true);
    m_topUsersTable->verticalHeader()->setVisible(false);
    m_topUsersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_topUsersTable->setAlternatingRowColors(true);
    m_topUsersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_topUsersTable->setMinimumHeight(300);
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
        [](const QString &error) {
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
        [](const QString &error) {
            qWarning() << "加载事件统计失败:" << error;
        });
}

void ReportsTab::loadRetentionStats()
{
    NetworkManager *networkManager = new NetworkManager(this);
    
    networkManager->getRetentionStats(
        [this](const QJsonObject &response) {
           // // LOG_DEBUG() << "留存率API返回数据:" << QJsonDocument(response).toJson(QJsonDocument::Compact);
            QJsonObject data = response["data"].toObject();
          //  // LOG_DEBUG() << "留存率data数据:" << QJsonDocument(data).toJson(QJsonDocument::Compact);
            
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
        [](const QString &error) {
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
        [](const QString &error) {
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
        [](const QString &error) {
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
        [](const QString &error) {
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
        [](const QString &error) {
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
        [](const QString &error) {
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
        [](const QString &error) {
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
        [](const QString &error) {
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
            <script src="qrc:/src/ECharts/echarts.min.js"></script>
        </head>
        <body style="margin:0;padding:0;background:#ffffff;">
            <div id="chart" style="width:100%%;height:450px;"></div>
            <script>
                var chart = echarts.init(document.getElementById('chart'));
                var option = {
                    title: { 
                        text: '%2',
                        left: 'center',
                        top: 20,
                        textStyle: {
                            fontSize: 16,
                            fontWeight: '600',
                            color: '#2c3e50'
                        }
                    },
                    tooltip: { 
                        trigger: 'axis',
                        backgroundColor: 'rgba(255, 255, 255, 0.95)',
                        borderColor: '#e9ecef',
                        borderWidth: 1,
                        textStyle: {
                            color: '#495057'
                        }
                    },
                    legend: { 
                        data: ['数值'], 
                        top: 50,
                        textStyle: {
                            fontSize: 12,
                            color: '#495057'
                        }
                    },
                    grid: {
                        left: '3%%',
                        right: '4%%',
                        bottom: '3%%',
                        top: '15%%',
                        containLabel: true
                    },
                    xAxis: { 
                        type: 'category', 
                        data: [%3],
                        axisLine: {
                            lineStyle: {
                                color: '#e9ecef'
                            }
                        },
                        axisLabel: {
                            color: '#6c757d',
                            fontSize: 11
                        }
                    },
                    yAxis: { 
                        type: 'value',
                        axisLine: {
                            lineStyle: {
                                color: '#e9ecef'
                            }
                        },
                        axisLabel: {
                            color: '#6c757d',
                            fontSize: 11
                        },
                        splitLine: {
                            lineStyle: {
                                color: '#f1f3f4',
                                type: 'dashed'
                            }
                        }
                    },
                    series: [{
                        name: '数值',
                        type: '%4',
                        data: [%5],
                        smooth: true,
                        itemStyle: {
                            color: '#007bff',
                            borderRadius: 4
                        },
                        areaStyle: { 
                            opacity: 0.1,
                            color: {
                                type: 'linear',
                                x: 0,
                                y: 0,
                                x2: 0,
                                y2: 1,
                                colorStops: [{
                                    offset: 0, color: '#007bff'
                                }, {
                                    offset: 1, color: 'rgba(0, 123, 255, 0)'
                                }]
                            }
                        },
                        lineStyle: {
                            width: 3,
                            color: '#007bff'
                        }
                    }]
                };
                chart.setOption(option);
                
                // 响应式调整
                window.addEventListener('resize', function() {
                    chart.resize();
                });
            </script>
        </body>
        </html>
    )").arg(title).arg(xAxisData).arg(chartType).arg(seriesData);
    
#ifdef WEBENGINE_AVAILABLE
    m_trendChartView->setHtml(html);
#else
    QLabel* chartLabel = qobject_cast<QLabel*>(m_trendChartView);
    if (chartLabel) {
        chartLabel->setText("WebEngine 不可用\n趋势图表已禁用");
    }
#endif
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
   // qDebug() << "开始CSV导出...";
    
    // 设置默认文件名和路径
    QString defaultFileName = QString("report_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + defaultFileName;
    
   // qDebug() << "默认文件路径:" << defaultPath;
    
    QString fileName = QFileDialog::getSaveFileName(
        this, 
        "导出报表", 
        defaultPath,
        "CSV文件 (*.csv);;所有文件 (*.*)",
        nullptr,
        QFileDialog::DontUseNativeDialog  // 使用Qt自己的对话框，避免系统对话框问题
    );
    
   // qDebug() << "用户选择的文件名:" << fileName;
    
    if (fileName.isEmpty()) {
      //  qDebug() << "用户取消了文件选择";
        return;
    }
    
  //  qDebug() << "开始写入文件:" << fileName;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
   //     qDebug() << "无法打开文件:" << fileName << "错误:" << file.errorString();
        QMessageBox::warning(this, "错误", "无法打开文件: " + fileName + "\n错误: " + file.errorString());
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
    
    // 检查表格是否存在并且有效
    if (!m_topPagesTable) {
     //   qDebug() << "热门页面表格为空指针";
        QMessageBox::warning(this, "错误", "热门页面表格未初始化");
        file.close();
        return;
    }
    
    int pagesRowCount = m_topPagesTable->rowCount();
  //  qDebug() << "热门页面表格行数:" << pagesRowCount;
    
    // 写入热门页面
    out << "热门页面\n";
    out << "排名,页面名称,访问次数\n";
    for (int i = 0; i < pagesRowCount && i < 100; ++i) { // 限制最大100行，防止无限循环
        QTableWidgetItem *nameItem = m_topPagesTable->item(i, 1);
        QTableWidgetItem *countItem = m_topPagesTable->item(i, 2);
        out << i + 1 << ","
            << (nameItem ? nameItem->text() : "无数据") << ","
            << (countItem ? countItem->text() : "0") << "\n";
    }
    out << "\n";
    
    // 检查事件表格
    if (!m_topEventsTable) {
    //    qDebug() << "热门事件表格为空指针";
        QMessageBox::warning(this, "错误", "热门事件表格未初始化");
        file.close();
        return;
    }
    
    int eventsRowCount = m_topEventsTable->rowCount();
  //  qDebug() << "热门事件表格行数:" << eventsRowCount;
    
    // 写入热门事件
    out << "热门事件\n";
    out << "排名,事件名称,触发次数\n";
    for (int i = 0; i < eventsRowCount && i < 100; ++i) { // 限制最大100行，防止无限循环
        QTableWidgetItem *nameItem = m_topEventsTable->item(i, 1);
        QTableWidgetItem *countItem = m_topEventsTable->item(i, 2);
        out << i + 1 << ","
            << (nameItem ? nameItem->text() : "无数据") << ","
            << (countItem ? countItem->text() : "0") << "\n";
    }
    
    file.close();
  //  qDebug() << "CSV导出完成，文件保存到:" << fileName;
    QMessageBox::information(this, "成功", "报表导出成功!\n文件保存到: " + fileName);
}

void ReportsTab::exportToExcel()
{
 //   qDebug() << "开始Excel导出...";
    
    // 设置默认文件名和路径
    QString defaultFileName = QString("report_%1.xlsx").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + defaultFileName;
    
    QString fileName = QFileDialog::getSaveFileName(
        this, 
        "导出报表为Excel", 
        defaultPath,
        "Excel文件 (*.xlsx);;CSV文件 (*.csv);;所有文件 (*.*)",
        nullptr,
        QFileDialog::DontUseNativeDialog
    );
    
    if (fileName.isEmpty()) {
    //    qDebug() << "用户取消了Excel文件选择";
        return;
    }
    
  //  qDebug() << "开始写入Excel文件:" << fileName;
    
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
    
    // 安全检查和写入热门页面
    if (m_topPagesTable) {
        int pagesRowCount = m_topPagesTable->rowCount();
     //   qDebug() << "Excel导出 - 热门页面表格行数:" << pagesRowCount;
        
        out << "热门页面\n";
        out << "排名,页面名称,访问次数\n";
        for (int i = 0; i < pagesRowCount && i < 100; ++i) {
            QTableWidgetItem *nameItem = m_topPagesTable->item(i, 1);
            QTableWidgetItem *countItem = m_topPagesTable->item(i, 2);
            out << i + 1 << ","
                << (nameItem ? nameItem->text() : "无数据") << ","
                << (countItem ? countItem->text() : "0") << "\n";
        }
        out << "\n";
    }
    
    // 安全检查和写入热门事件
    if (m_topEventsTable) {
        int eventsRowCount = m_topEventsTable->rowCount();
    //    qDebug() << "Excel导出 - 热门事件表格行数:" << eventsRowCount;
        
        out << "热门事件\n";
        out << "排名,事件名称,触发次数\n";
        for (int i = 0; i < eventsRowCount && i < 100; ++i) {
            QTableWidgetItem *nameItem = m_topEventsTable->item(i, 1);
            QTableWidgetItem *countItem = m_topEventsTable->item(i, 2);
            out << i + 1 << ","
                << (nameItem ? nameItem->text() : "无数据") << ","
                << (countItem ? countItem->text() : "0") << "\n";
        }
    }
    
    file.close();
  //  qDebug() << "Excel导出完成，文件保存到:" << fileName;
    QMessageBox::information(this, "成功", "报表导出成功!\n文件保存到: " + fileName + "\n\n注意：文件采用CSV格式保存为.xlsx扩展名，可被Excel打开。");
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
                body { font-family: "Helvetica Neue", "Helvetica", "Arial"; margin: 20px; }
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
    if (m_topPagesTable) {
        int pagesRowCount = m_topPagesTable->rowCount();
     //   qDebug() << "PDF导出 - 热门页面表格行数:" << pagesRowCount;
        
        for (int i = 0; i < pagesRowCount && i < 100; ++i) {
            QTableWidgetItem *nameItem = m_topPagesTable->item(i, 1);
            QTableWidgetItem *countItem = m_topPagesTable->item(i, 2);
            html += QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
                .arg(i + 1)
                .arg(nameItem ? nameItem->text() : "无数据")
                .arg(countItem ? countItem->text() : "0");
        }
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
    if (m_topEventsTable) {
        int eventsRowCount = m_topEventsTable->rowCount();
     //   qDebug() << "PDF导出 - 热门事件表格行数:" << eventsRowCount;
        
        for (int i = 0; i < eventsRowCount && i < 100; ++i) {
            QTableWidgetItem *nameItem = m_topEventsTable->item(i, 1);
            QTableWidgetItem *countItem = m_topEventsTable->item(i, 2);
            html += QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>")
                .arg(i + 1)
                .arg(nameItem ? nameItem->text() : "无数据")
                .arg(countItem ? countItem->text() : "0");
        }
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

void ReportsTab::applyTheme()
{
    const auto& colors = ThemeManager::instance()->colors();
    
    // 直接为按钮设置样式，避免选择器问题
    QString primaryButtonStyle = QString(R"(
        background-color: %1;
        color: white;
        border: none;
        border-radius: %2px;
        padding: 10px 16px;
        font-weight: 500;
        font-size: %3px;
        min-width: 70px;
    )").arg(colors.PRIMARY)
       .arg(ThemeManager::BorderRadius::MD)
       .arg(ThemeManager::Typography::FONT_SIZE_SM);
    
    QString secondaryButtonStyle = QString(R"(
        background-color: %1;
        color: white;
        border: none;
        border-radius: %2px;
        padding: 10px 16px;
        font-weight: 500;
        font-size: %3px;
        min-width: 70px;
    )").arg(colors.GRAY_600)
       .arg(ThemeManager::BorderRadius::MD)
       .arg(ThemeManager::Typography::FONT_SIZE_SM);
    
    // 直接设置按钮样式
    if (m_refreshButton) m_refreshButton->setStyleSheet(primaryButtonStyle);
    if (m_exportButton) m_exportButton->setStyleSheet(secondaryButtonStyle);
    if (m_exportCSVButton) m_exportCSVButton->setStyleSheet(secondaryButtonStyle);
    if (m_exportExcelButton) m_exportExcelButton->setStyleSheet(secondaryButtonStyle);
    if (m_exportPDFButton) m_exportPDFButton->setStyleSheet(secondaryButtonStyle);
    
    // 应用输入控件样式
    QString inputStyle = QString(R"(
        QComboBox#comboBox, QDateEdit#dateEdit {
            border: 1px solid %1;
            border-radius: %2px;
            padding: 8px 12px;
            background-color: %3;
            font-size: %4px;
            min-width: 120px;
            color: %5;
        }
        QComboBox#comboBox:hover, QDateEdit#dateEdit:hover {
            border-color: %6;
        }
    )").arg(colors.BORDER)
       .arg(ThemeManager::BorderRadius::SM)
       .arg(colors.SURFACE)
       .arg(ThemeManager::Typography::FONT_SIZE_SM)
       .arg(colors.TEXT_PRIMARY)
       .arg(colors.PRIMARY);
    
    // 应用表格样式
    QString tableStyle = QString(R"(
        QTableWidget#dataTable {
            border: 1px solid %1;
            border-radius: %2px;
            background-color: %3;
            gridline-color: %4;
            font-size: %5px;
            color: %6;
        }
        QTableWidget#dataTable::item {
            padding: 12px 8px;
            border-bottom: 1px solid %7;
        }
        QTableWidget#dataTable::item:selected {
            background-color: %8;
            color: %9;
        }
        QHeaderView::section {
            background-color: %10;
            padding: 12px 8px;
            border: none;
            border-bottom: 2px solid %11;
            font-weight: 600;
            color: %12;
        }
    )").arg(colors.BORDER)
       .arg(ThemeManager::BorderRadius::MD)
       .arg(colors.CARD)
       .arg(colors.DIVIDER)
       .arg(ThemeManager::Typography::FONT_SIZE_SM)
       .arg(colors.TEXT_PRIMARY)
       .arg(colors.DIVIDER)
       .arg(colors.PRIMARY_LIGHT)
       .arg(colors.PRIMARY)
       .arg(colors.SURFACE)
       .arg(colors.BORDER)
       .arg(colors.TEXT_SECONDARY);
    
    // 应用指标标签样式
    QString metricLabelStyle = QString(R"(
        QLabel#metricLabel {
            padding: 20px;
            border-radius: %1px;
            font-size: %2px;
            font-weight: 600;
            color: %3;
            background-color: %4;
            border: 1px solid %5;
            margin: 4px;
            min-height: 60px;
            border-left: 4px solid %6;
        }
    )").arg(ThemeManager::BorderRadius::LG)
       .arg(ThemeManager::Typography::FONT_SIZE_SM)
       .arg(colors.TEXT_PRIMARY)
       .arg(colors.CARD)
       .arg(colors.BORDER)
       .arg(colors.PRIMARY);
    
    // 应用图表视图样式
    QString chartViewStyle = QString(R"(
        QWebEngineView#chartView {
            border: none;
            border-radius: %1px;
            background-color: %2;
        }
    )").arg(ThemeManager::BorderRadius::MD)
       .arg(colors.CARD);
    
    // 设置主窗口样式，包含所有组件样式（不包括按钮样式，因为按钮使用直接设置）
    QString mainStyle = QString(R"(
        QWidget {
            background-color: %1;
            font-family: %2;
            color: %3;
        }
    )").arg(colors.BACKGROUND)
       .arg(ThemeManager::Typography::FONT_FAMILY)
       .arg(colors.TEXT_PRIMARY);
    
    // 组合所有样式并应用（不包括按钮样式）
    QString combinedStyle = mainStyle + inputStyle + tableStyle + metricLabelStyle + chartViewStyle;
    
    setStyleSheet(combinedStyle);
}

QGroupBox *ReportsTab::createGroupBox(const QString &title, QWidget *content)
{
    QGroupBox *groupBox = new QGroupBox(title);
    QVBoxLayout *layout = new QVBoxLayout(groupBox);
    layout->addWidget(content);
    return groupBox;
}

// 新增现代卡片创建方法
QWidget *ReportsTab::createModernCard(const QString &title, QWidget *content, bool showTitle)
{
    const auto& colors = ThemeManager::instance()->colors();
    
    QWidget *card = new QWidget();
    card->setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border-radius: %2px;
            border: 1px solid %3;
        }
    )").arg(colors.CARD)
       .arg(ThemeManager::BorderRadius::LG)
       .arg(colors.BORDER));
    
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);
    
    if (showTitle && !title.isEmpty()) {
        QLabel *titleLabel = new QLabel(title);
        titleLabel->setStyleSheet(QString(R"(
            QLabel {
                font-weight: 600;
                font-size: %1px;
                color: %2;
                padding: 16px 20px 8px 20px;
                background-color: transparent;
                border: none;
            }
        )").arg(ThemeManager::Typography::FONT_SIZE_LG)
           .arg(colors.TEXT_PRIMARY));
        cardLayout->addWidget(titleLabel);
    }
    
    cardLayout->addWidget(content);
    return card;
}

// 新增分隔符创建方法
QWidget *ReportsTab::createSeparator()
{
    const auto& colors = ThemeManager::instance()->colors();
    
    QWidget *separator = new QWidget();
    separator->setFixedWidth(1);
    separator->setStyleSheet(QString("QWidget { background-color: %1; }").arg(colors.BORDER));
    return separator;
}

QWidget *ReportsTab::createToolbarWidget()
{
    const auto& colors = ThemeManager::instance()->colors();
    
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(16);
    
    // 时间选择区域
    QHBoxLayout *dateLayout = new QHBoxLayout();
    dateLayout->setSpacing(8);
    
    QLabel *startLabel = new QLabel("开始:");
    startLabel->setStyleSheet(QString("QLabel { color: %1; font-weight: 500; }").arg(colors.TEXT_SECONDARY));
    QLabel *endLabel = new QLabel("至:");
    endLabel->setStyleSheet(QString("QLabel { color: %1; font-weight: 500; }").arg(colors.TEXT_SECONDARY));
    
    dateLayout->addWidget(startLabel);
    dateLayout->addWidget(m_startDateEdit);
    dateLayout->addWidget(endLabel);
    dateLayout->addWidget(m_endDateEdit);
    
    // 筛选选择区域
    QHBoxLayout *filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(8);
    
    QLabel *reportLabel = new QLabel("报表:");
    reportLabel->setStyleSheet(QString("QLabel { color: %1; font-weight: 500; }").arg(colors.TEXT_SECONDARY));
    QLabel *chartLabel = new QLabel("图表:");
    chartLabel->setStyleSheet(QString("QLabel { color: %1; font-weight: 500; }").arg(colors.TEXT_SECONDARY));
    
    filterLayout->addWidget(reportLabel);
    filterLayout->addWidget(m_reportTypeCombo);
    filterLayout->addWidget(chartLabel);
    filterLayout->addWidget(m_chartTypeCombo);
    
    // 操作按钮区域
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(8);
    
    actionLayout->addWidget(m_refreshButton);
    actionLayout->addWidget(m_exportButton);
    actionLayout->addWidget(m_exportCSVButton);
    actionLayout->addWidget(m_exportExcelButton);
    actionLayout->addWidget(m_exportPDFButton);
    
    // 组合所有布局
    layout->addLayout(dateLayout);
    layout->addWidget(createSeparator());
    layout->addLayout(filterLayout);
    layout->addWidget(createSeparator());
    layout->addLayout(actionLayout);
    layout->addStretch();
    
    widget->setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border-radius: %2px;
            border: 1px solid %3;
        }
    )").arg(colors.CARD)
       .arg(ThemeManager::BorderRadius::LG)
       .arg(colors.BORDER));
    
    return widget;
}

QWidget *ReportsTab::createKeyMetricsWidget()
{
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(16);
    
    layout->addWidget(m_dauLabel, 0, 0);
    layout->addWidget(m_mauLabel, 0, 1);
    layout->addWidget(m_retentionLabel, 0, 2);
    layout->addWidget(m_conversionLabel, 1, 0);
    layout->addWidget(m_onlineUsersLabel, 1, 1);
    layout->addWidget(m_todayEventsLabel, 1, 2);
    layout->addWidget(m_totalEventsLabel, 2, 0, 1, 3);
    
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
    const auto& colors = ThemeManager::instance()->colors();
    
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(20);
    
    // 设置实时数据样式
    QString realtimeStyle = QString(R"(
        QLabel {
            padding: 16px 20px;
            border-radius: 10px;
            font-size: %1px;
            font-weight: 600;
            color: white;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, 
                stop:0 %2, stop:1 %3);
            min-width: 150px;
        }
    )").arg(ThemeManager::Typography::FONT_SIZE_SM)
       .arg(colors.SUCCESS)
       .arg(colors.INFO);
    
    m_onlineUsersLabel->setStyleSheet(realtimeStyle);
    m_todayEventsLabel->setStyleSheet(realtimeStyle + QString(R"(
        QLabel {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, 
                stop:0 %1, stop:1 %2);
        }
    )").arg(colors.PRIMARY).arg(colors.INFO));
    
    m_onlineUsersLabel->setAlignment(Qt::AlignCenter);
    m_todayEventsLabel->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(m_onlineUsersLabel);
    layout->addWidget(m_todayEventsLabel);
    layout->addStretch();
    
    return widget;
}

