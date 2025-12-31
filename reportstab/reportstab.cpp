#include "reportstab.h"
#include "../networkmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
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
    m_startDateEdit = new QDateEdit(QDate::currentDate().addDays(-30));
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDisplayFormat("yyyy-MM-dd");
    connect(m_startDateEdit, &QDateEdit::dateChanged, this, &ReportsTab::onDateRangeChanged);
    
    m_endDateEdit = new QDateEdit(QDate::currentDate());
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("yyyy-MM-dd");
    connect(m_endDateEdit, &QDateEdit::dateChanged, this, &ReportsTab::onDateRangeChanged);
    
    // 报表类型选择
    m_reportTypeCombo = new QComboBox();
    m_reportTypeCombo->addItem("用户活跃度", "activity");
    m_reportTypeCombo->addItem("留存率", "retention");
    m_reportTypeCombo->addItem("页面访问", "pageview");
    m_reportTypeCombo->addItem("事件统计", "event");
    connect(m_reportTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ReportsTab::onReportTypeChanged);
    
    // 图表类型选择
    m_chartTypeCombo = new QComboBox();
    m_chartTypeCombo->addItem("折线图", "line");
    m_chartTypeCombo->addItem("柱状图", "bar");
    
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
    
    // 设置样式
    QString metricStyle = "QLabel { padding: 15px; border-radius: 10px; font-size: 16px; font-weight: bold; background-color: #f8f9fa; }";
    m_dauLabel->setStyleSheet(metricStyle);
    m_mauLabel->setStyleSheet(metricStyle);
    m_retentionLabel->setStyleSheet(metricStyle);
    m_conversionLabel->setStyleSheet(metricStyle);
    m_onlineUsersLabel->setStyleSheet(metricStyle);
    m_todayEventsLabel->setStyleSheet(metricStyle);
    
    QGridLayout *metricsLayout = new QGridLayout();
    metricsLayout->addWidget(m_dauLabel, 0, 0);
    metricsLayout->addWidget(m_mauLabel, 0, 1);
    metricsLayout->addWidget(m_retentionLabel, 0, 2);
    metricsLayout->addWidget(m_conversionLabel, 1, 0);
    metricsLayout->addWidget(m_onlineUsersLabel, 1, 1);
    metricsLayout->addWidget(m_todayEventsLabel, 1, 2);
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
}

void ReportsTab::loadRetentionStats()
{
    NetworkManager *networkManager = new NetworkManager(this);
    
    networkManager->getRetentionStats(
        [this](const QJsonObject &response) {
            double retention = response["data"].toObject()["retentionRate"].toDouble();
            m_retentionLabel->setText(QString("留存率: %1%").arg(retention * 100, 0, 'f', 2));
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
    QString metric = m_reportTypeCombo->currentData().toString();
    
    networkManager->getTrendAnalysis(metric, startDate, endDate,
        [this](const QJsonObject &response) {
            QJsonArray trendData = response["data"].toArray();
            QString title = m_reportTypeCombo->currentText();
            updateTrendChart(trendData, title);
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
            QJsonObject data = response["data"].toObject();
            QJsonArray users = data["users"].toArray();
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
            updateRealTimeStats(response["data"].toObject());
        },
        [this](const QString &error) {
            qWarning() << "加载实时统计失败:" << error;
        });
}

void ReportsTab::updateKeyMetricsDisplay(const QJsonObject &metrics)
{
    QJsonArray dauStats = metrics["dauStats"].toArray();
    QJsonArray mauStats = metrics["mauStats"].toArray();
    
    if (!dauStats.isEmpty()) {
        int latestDau = dauStats.last().toObject()["dau"].toInt();
        m_dauLabel->setText(QString("DAU: %1").arg(latestDau));
    }
    
    if (!mauStats.isEmpty()) {
        int latestMau = mauStats.last().toObject()["mau"].toInt();
        m_mauLabel->setText(QString("MAU: %1").arg(latestMau));
    }
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
    m_topUsersTable->setRowCount(users.size());
    for (int i = 0; i < users.size(); ++i) {
        QJsonObject user = users[i].toObject();
        m_topUsersTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        m_topUsersTable->setItem(i, 1, new QTableWidgetItem(user["userId"].toString()));
        m_topUsersTable->setItem(i, 2, new QTableWidgetItem(user["activityLevel"].toString()));
    }
}

void ReportsTab::updateRealTimeStats(const QJsonObject &stats)
{
    int onlineUsers = stats["onlineUsers"].toInt();
    int todayEvents = stats["todayEvents"].toInt();
    
    m_onlineUsersLabel->setText(QString("在线用户: %1").arg(onlineUsers));
    m_todayEventsLabel->setText(QString("今日事件: %1").arg(todayEvents));
}

void ReportsTab::renderTrendChart(const QJsonArray &trendData, const QString &title, const QString &chartType)
{
    QString xAxisData;
    QString seriesData;
    
    for (const QJsonValue &value : trendData) {
        QJsonObject item = value.toObject();
        QString timeBucket = item["time_bucket"].toString();
        int count = item["count"].toInt();
        
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
    // Excel导出需要额外的库支持，这里暂时使用CSV格式
    QMessageBox::information(this, "提示", "Excel导出功能正在开发中，请使用CSV格式导出。");
}

void ReportsTab::exportToPDF()
{
    // PDF导出需要额外的库支持，这里暂时提示用户
    QMessageBox::information(this, "提示", "PDF导出功能正在开发中，请使用CSV格式导出。");
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