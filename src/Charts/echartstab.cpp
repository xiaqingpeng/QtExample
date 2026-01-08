#include "echartstab.h"
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#ifdef WEBENGINE_AVAILABLE
#include <QWebChannel>
#include <QWebEngineView>
#endif
#include <QFile>
#include "networkmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
// #include <QTimer>  // 已禁用定时器功能
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QList>
#include <QDateTimeEdit>
#include <QDate>
#include "echartstab.h"
#include "common.h"
#include "../Analytics/analytics.h"
#include "../Styles/theme_manager.h"

EChartsTab::EChartsTab(QWidget *parent)
    : QMainWindow(parent)
    , m_networkManager(new NetworkManager(this))
{
    // 1. 设置窗口基本属性
    this->setWindowTitle("Qt + ECharts Demo");
    this->resize(800, 600);

    // 2. 创建中心部件和布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("echartsMainWidget");
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);
    this->setCentralWidget(centralWidget);

#ifdef WEBENGINE_AVAILABLE
    // 3. 创建WebEngineView，加载ECharts HTML页面
    m_webView = new QWebEngineView(this);
    m_webView->setObjectName("echartsWebView");
    layout->addWidget(m_webView);

    // 4. 创建桥接对象，实现Qt与JS交互
    m_bridge = new ChartBridge(this);
    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject("qt", m_bridge);  // 注册为window.qt对象（供JS调用）
    m_webView->page()->setWebChannel(channel);

    // 5. 加载本地HTML文件（使用绝对路径确保文件能被找到）
    QString htmlPath = "/Applications/qingpengxia/qt/qt6/example/src/ECharts/chart.html";
    
    // 连接WebView加载完成信号
    connect(m_webView, &QWebEngineView::loadFinished, this, &EChartsTab::onPageLoaded);
    
    m_webView->load(QUrl::fromLocalFile(htmlPath));
#else
    // WebEngine不可用时，创建一个简单的标签作为占位符
    m_webView = new QLabel("WebEngine 不可用 - ECharts功能已禁用", this);
    m_webView->setObjectName("echartsWebView");
    m_webView->setAlignment(Qt::AlignCenter);
    m_webView->setStyleSheet("QLabel { color: #666; font-size: 14px; }");
    layout->addWidget(m_webView);

    // 创建桥接对象但不使用WebChannel
    m_bridge = new ChartBridge(this);
    m_channel = nullptr;
#endif
    QFile file(htmlPath);
    if (!file.exists()) {
        // HTML文件不存在
        return;
    }
    
    // 连接WebView加载完成信号
    connect(m_webView, &QWebEngineView::loadFinished, this, &EChartsTab::onPageLoaded);
    
    m_webView->load(QUrl::fromLocalFile(htmlPath));

    // 6. 创建筛选控件
    QHBoxLayout *filterLayout = new QHBoxLayout();
    
    // Method选择下拉框
    m_methodCombo = new QComboBox(this);
    m_methodCombo->setObjectName("methodCombo");
    m_methodCombo->addItem("All", "");  // All表示不筛选
    m_methodCombo->addItem("Post", "Post");
    m_methodCombo->addItem("Get", "Get");
    m_methodCombo->setCurrentIndex(0);  // 默认选择All
    m_methodCombo->setMinimumWidth(120);
    m_methodCombo->setMinimumHeight(36);
    
    QLabel *methodLabel = new QLabel("Method:");
    methodLabel->setObjectName("filterLabel");
    filterLayout->addWidget(methodLabel);
    filterLayout->addWidget(m_methodCombo);
    
    // Platform选择下拉框
    m_platformCombo = new QComboBox(this);
    m_platformCombo->setObjectName("platformCombo");
    m_platformCombo->addItem("All", "");  // All表示不筛选
    m_platformCombo->addItem("Web", "web");
    m_platformCombo->addItem("Android", "android");
    m_platformCombo->addItem("IOS", "ios");
    m_platformCombo->addItem("Windows", "windows");
    m_platformCombo->addItem("Mac", "mac");
    m_platformCombo->setCurrentIndex(0);  // 默认选择All
    m_platformCombo->setMinimumWidth(120);
    m_platformCombo->setMinimumHeight(36);
    
    QLabel *platformLabel = new QLabel("Platform:");
    platformLabel->setObjectName("filterLabel");
    filterLayout->addWidget(platformLabel);
    filterLayout->addWidget(m_platformCombo);
    
    // 图表类型选择下拉框
    m_chartTypeCombo = new QComboBox(this);
    m_chartTypeCombo->setObjectName("chartTypeCombo");
    m_chartTypeCombo->addItem("柱状图", "bar");
    m_chartTypeCombo->addItem("折线图", "line");
    m_chartTypeCombo->addItem("饼图", "pie");
    m_chartTypeCombo->addItem("雷达图", "radar");
    m_chartTypeCombo->setCurrentIndex(0);  // 默认选择柱状图
    m_chartTypeCombo->setMinimumWidth(120);
    m_chartTypeCombo->setMinimumHeight(36);
    
    QLabel *chartTypeLabel = new QLabel("图表:");
    chartTypeLabel->setObjectName("filterLabel");
    filterLayout->addWidget(chartTypeLabel);
    filterLayout->addWidget(m_chartTypeCombo);
    
    layout->addLayout(filterLayout);
    
    // 6.1 创建时间筛选控件
    QHBoxLayout *timeFilterLayout = new QHBoxLayout();
    
    // 开始时间选择器
    QLabel *startTimeLabel = new QLabel("开始时间:");
    startTimeLabel->setObjectName("filterLabel");
    timeFilterLayout->addWidget(startTimeLabel);
    m_startTimeEdit = new QDateTimeEdit(this);
    m_startTimeEdit->setObjectName("startTimeEdit");
    m_startTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    m_startTimeEdit->setCalendarPopup(true);
    m_startTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(-7));  // 默认7天前
    m_startTimeEdit->setMinimumHeight(36);
    timeFilterLayout->addWidget(m_startTimeEdit);
    
    // 结束时间选择器
    QLabel *endTimeLabel = new QLabel("结束时间:");
    endTimeLabel->setObjectName("filterLabel");
    timeFilterLayout->addWidget(endTimeLabel);
    m_endTimeEdit = new QDateTimeEdit(this);
    m_endTimeEdit->setObjectName("endTimeEdit");
    m_endTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    m_endTimeEdit->setCalendarPopup(true);
    m_endTimeEdit->setDateTime(QDateTime::currentDateTime());  // 默认当前时间
    m_endTimeEdit->setMinimumHeight(36);
    timeFilterLayout->addWidget(m_endTimeEdit);
    
    layout->addLayout(timeFilterLayout);
    
    // 6.2 创建快捷时间按钮
    QHBoxLayout *shortcutLayout = new QHBoxLayout();
    
    m_btnToday = new QPushButton("今天", this);
    m_btnToday->setObjectName("timeShortcutButton");
    m_btnYesterday = new QPushButton("昨天", this);
    m_btnYesterday->setObjectName("timeShortcutButton");
    m_btnLast7Days = new QPushButton("最近7天", this);
    m_btnLast7Days->setObjectName("timeShortcutButton");
    m_btnLast30Days = new QPushButton("最近30天", this);
    m_btnLast30Days->setObjectName("timeShortcutButton");
    m_btnClearTime = new QPushButton("清除时间", this);
    m_btnClearTime->setObjectName("timeShortcutButton");
    
    shortcutLayout->addWidget(m_btnToday);
    shortcutLayout->addWidget(m_btnYesterday);
    shortcutLayout->addWidget(m_btnLast7Days);
    shortcutLayout->addWidget(m_btnLast30Days);
    shortcutLayout->addWidget(m_btnClearTime);
    
    layout->addLayout(shortcutLayout);
    
    // 连接快捷按钮信号
    connect(m_btnToday, &QPushButton::clicked, [this]() { onTimeShortcutClicked(0); });
    connect(m_btnYesterday, &QPushButton::clicked, [this]() { onTimeShortcutClicked(1); });
    connect(m_btnLast7Days, &QPushButton::clicked, [this]() { onTimeShortcutClicked(7); });
    connect(m_btnLast30Days, &QPushButton::clicked, [this]() { onTimeShortcutClicked(30); });
    connect(m_btnClearTime, &QPushButton::clicked, [this]() { onTimeShortcutClicked(-1); });
    
    // 连接时间选择器变化信号
    connect(m_startTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &EChartsTab::onTimeFilterChanged);
    connect(m_endTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &EChartsTab::onTimeFilterChanged);
    
    // 7. 创建API数据获取按钮
    QPushButton *btnFetchApi = new QPushButton("获取API数据", this);
    btnFetchApi->setObjectName("fetchApiButton");
    btnFetchApi->setMinimumHeight(40);
    layout->addWidget(btnFetchApi);
    
    connect(btnFetchApi, &QPushButton::clicked, this, &EChartsTab::fetchApiData);
    
    // 连接筛选条件改变信号
    connect(m_methodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &EChartsTab::onFilterChanged);
    connect(m_platformCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &EChartsTab::onFilterChanged);
    connect(m_chartTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &EChartsTab::onChartTypeChanged);
    
    // 已取消定时器功能，不再自动刷新数据
    // 创建定时器（但不启动，等待首次手动获取数据后才启动）
    // m_apiTimer = new QTimer(this);
    // connect(m_apiTimer, &QTimer::timeout, this, &EChartsTab::fetchApiData);
    
    // 应用主题
    applyTheme();
    
    // 连接主题变化信号
    connect(ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &EChartsTab::applyTheme);
    
    // 初始化当前高亮状态
    m_currentHighlightedDays = 0; // 默认选择"今天"
    
    // 初始化默认选择"今天"
    onTimeShortcutClicked(0);
    
    // 追踪ECharts图表页面浏览事件
    Analytics::SDK::instance()->trackView("echarts_page", {
        {"page_title", "ECharts图表示例"}
    });
}

EChartsTab::~EChartsTab()
{
}

void EChartsTab::onFilterChanged()
{
    // 追踪筛选操作事件
    QString method = m_methodCombo->currentData().toString();
    QString platform = m_platformCombo->currentData().toString();
    Analytics::SDK::instance()->track("chart_filter_changed", {
        {"event_type", "click"},
        {"page", "echarts_page"},
        {"filter_method", method},
        {"filter_platform", platform}
    });
    
    // 当筛选条件改变时，自动重新获取数据
    fetchApiData();
}

void EChartsTab::onTimeFilterChanged()
{
    // 追踪时间筛选操作事件
    QString startTime;
    QString endTime;
    if (m_startTimeEdit->dateTime().isValid()) {
        startTime = m_startTimeEdit->dateTime().toString(Qt::ISODate);
    }
    if (m_endTimeEdit->dateTime().isValid()) {
        endTime = m_endTimeEdit->dateTime().toString(Qt::ISODate);
    }
    Analytics::SDK::instance()->track("chart_time_filter_changed", {
        {"event_type", "click"},
        {"page", "echarts_page"},
        {"start_time", startTime},
        {"end_time", endTime}
    });
    
    // 当时间筛选条件改变时，自动重新获取数据
    fetchApiData();
}

void EChartsTab::onChartTypeChanged(int index)
{
    Q_UNUSED(index);
    // 追踪图表类型切换事件
    QString chartType = m_chartTypeCombo->currentData().toString();
    Analytics::SDK::instance()->track("chart_type_changed", {
        {"event_type", "click"},
        {"page", "echarts_page"},
        {"chart_type", chartType}
    });
    
    // 当图表类型改变时，重新渲染图表
    fetchApiData();
}

void EChartsTab::onTimeShortcutClicked(int days)
{
    QDateTime now = QDateTime::currentDateTime();
    
    // 清除所有按钮的高亮
    updateButtonHighlight(-1);
    
    // 阻止信号发射，避免触发多次请求
    m_startTimeEdit->blockSignals(true);
    m_endTimeEdit->blockSignals(true);
    
    if (days == 0) {
        // 今天
        m_startTimeEdit->setDateTime(QDateTime(now.date(), QTime(0, 0, 0)));
        m_endTimeEdit->setDateTime(QDateTime(now.date(), QTime(23, 59, 59)));
        // 高亮"今天"按钮
        updateButtonHighlight(0);
    } else if (days == 1) {
        // 昨天
        QDateTime yesterday = now.addDays(-1);
        m_startTimeEdit->setDateTime(QDateTime(yesterday.date(), QTime(0, 0, 0)));
        m_endTimeEdit->setDateTime(QDateTime(yesterday.date(), QTime(23, 59, 59)));
        // 高亮"昨天"按钮
        updateButtonHighlight(1);
    } else if (days > 0) {
        // 最近N天：从N天前的00:00:00到今天的23:59:59
        QDateTime startTime = QDateTime(now.addDays(-days).date(), QTime(0, 0, 0));
        QDateTime endTime = QDateTime(now.date(), QTime(23, 59, 59));
        m_startTimeEdit->setDateTime(startTime);
        m_endTimeEdit->setDateTime(endTime);
        // 高亮对应的按钮
        updateButtonHighlight(days);
    } else if (days == -1) {
        // 清除时间
        m_startTimeEdit->clear();
        m_endTimeEdit->clear();
        // 高亮"清除时间"按钮
        updateButtonHighlight(-1);
    }
    
    // 恢复信号发射
    m_startTimeEdit->blockSignals(false);
    m_endTimeEdit->blockSignals(false);
    
    // 手动触发一次数据获取
    fetchApiData();
}

void EChartsTab::updateButtonHighlight(int days)
{
    ThemeManager *theme = ThemeManager::instance();
    
    // 定义按钮样式
    QString normalStyle = QString(
        "QPushButton#timeShortcutButton { "
        "    background-color: %1; "
        "    border: 1px solid %2; "
        "    border-radius: %3px; "
        "    padding: 8px 16px; "
        "    font-size: %4px; "
        "    color: %5; "
        "    font-family: %6; "
        "}"
        "QPushButton#timeShortcutButton:hover { "
        "    background-color: %7; "
        "    border: 1px solid %8; "
        "}"
        "QPushButton#timeShortcutButton:pressed { "
        "    background-color: %9; "
        "}"
    ).arg(theme->colors().SURFACE)
     .arg(theme->colors().BORDER)
     .arg(ThemeManager::BorderRadius::SM)
     .arg(ThemeManager::Typography::FONT_SIZE_SM)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(theme->colors().GRAY_100)
     .arg(theme->colors().PRIMARY)
     .arg(theme->colors().GRAY_200);
    
    QString highlightStyle = QString(
        "QPushButton#timeShortcutButton { "
        "    background-color: %1; "
        "    color: %2; "
        "    border: 1px solid %3; "
        "    border-radius: %4px; "
        "    padding: 8px 16px; "
        "    font-size: %5px; "
        "    font-family: %6; "
        "}"
        "QPushButton#timeShortcutButton:hover { "
        "    background-color: %7; "
        "}"
        "QPushButton#timeShortcutButton:pressed { "
        "    background-color: %8; "
        "}"
    ).arg(theme->colors().PRIMARY)
     .arg(theme->colors().SURFACE)
     .arg(theme->colors().PRIMARY_HOVER)
     .arg(ThemeManager::BorderRadius::SM)
     .arg(ThemeManager::Typography::FONT_SIZE_SM)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(theme->colors().PRIMARY_HOVER)
     .arg(theme->colors().PRIMARY_HOVER);
    
    // 如果是特殊值-2，只更新样式不改变高亮状态
    if (days == -2) {
        // 保持当前高亮状态，只更新样式
        // 重新应用所有按钮的样式
        if (m_btnToday) m_btnToday->setStyleSheet(m_currentHighlightedDays == 0 ? highlightStyle : normalStyle);
        if (m_btnYesterday) m_btnYesterday->setStyleSheet(m_currentHighlightedDays == 1 ? highlightStyle : normalStyle);
        if (m_btnLast7Days) m_btnLast7Days->setStyleSheet(m_currentHighlightedDays == 7 ? highlightStyle : normalStyle);
        if (m_btnLast30Days) m_btnLast30Days->setStyleSheet(m_currentHighlightedDays == 30 ? highlightStyle : normalStyle);
        if (m_btnClearTime) m_btnClearTime->setStyleSheet(m_currentHighlightedDays == -1 ? highlightStyle : normalStyle);
        return;
    }
    
    // 重置所有按钮为普通样式
    if (m_btnToday) m_btnToday->setStyleSheet(normalStyle);
    if (m_btnYesterday) m_btnYesterday->setStyleSheet(normalStyle);
    if (m_btnLast7Days) m_btnLast7Days->setStyleSheet(normalStyle);
    if (m_btnLast30Days) m_btnLast30Days->setStyleSheet(normalStyle);
    if (m_btnClearTime) m_btnClearTime->setStyleSheet(normalStyle);
    
    // 根据days参数高亮对应按钮
    switch (days) {
        case 0:
            if (m_btnToday) m_btnToday->setStyleSheet(highlightStyle);
            m_currentHighlightedDays = 0;
            break;
        case 1:
            if (m_btnYesterday) m_btnYesterday->setStyleSheet(highlightStyle);
            m_currentHighlightedDays = 1;
            break;
        case 7:
            if (m_btnLast7Days) m_btnLast7Days->setStyleSheet(highlightStyle);
            m_currentHighlightedDays = 7;
            break;
        case 30:
            if (m_btnLast30Days) m_btnLast30Days->setStyleSheet(highlightStyle);
            m_currentHighlightedDays = 30;
            break;
        case -1:
            if (m_btnClearTime) m_btnClearTime->setStyleSheet(highlightStyle);
            m_currentHighlightedDays = -1;
            break;
        default:
            m_currentHighlightedDays = days;
            break;
    }
}

void EChartsTab::onPageLoaded(bool ok)
{
    if (ok) {
        // 页面加载完成后，延迟一小段时间确保JavaScript环境准备好
        // QTimer::singleShot(500, this, &EChartsTab::fetchApiData); // 已禁用自动获取数据
        // 改为直接调用，不使用定时器延迟
        fetchApiData();
    } else {
        // WebView页面加载失败
    }
}

void EChartsTab::fetchApiData()
{
    // 开始性能计时
    QElapsedTimer timer;
    timer.start();
    
    // 追踪图表数据查看事件
    Analytics::SDK::instance()->track("chart_data_viewed", {
        {"event_type", "view"},
        {"page", "echarts_page"}
    });
    
    // 已取消定时器自动刷新功能，只在手动操作时获取数据
    // 首次获取数据后启动定时器（每30秒自动刷新）
    // if (!m_apiTimer->isActive()) {
    //     m_apiTimer->start(30000); // 30秒
    // }
    
    // 从下拉框获取筛选条件
    QString method = m_methodCombo->currentData().toString();
    QString platform = m_platformCombo->currentData().toString();
    
    // 从时间选择器获取时间筛选条件
    QString startTime;
    QString endTime;
    if (m_startTimeEdit->dateTime().isValid()) {
        startTime = m_startTimeEdit->dateTime().toString(Qt::ISODate) + "Z";
    }
    if (m_endTimeEdit->dateTime().isValid()) {
        endTime = m_endTimeEdit->dateTime().toString(Qt::ISODate) + "Z";
    }
    
    // 构建带筛选条件的API URL
    QString apiUrl = "http://120.48.95.51:7001/system/logs/stats";
    
    // 使用QUrlQuery构建查询参数
    QUrlQuery queryParams;
    if (!method.isEmpty()) {
        queryParams.addQueryItem("method", method);
    }
    if (!platform.isEmpty()) {
        queryParams.addQueryItem("platform", platform);
    }
    if (!startTime.isEmpty()) {
        queryParams.addQueryItem("startTime", startTime);
    }
    if (!endTime.isEmpty()) {
        queryParams.addQueryItem("endTime", endTime);
    }
    
    // 添加分页参数（获取更多数据）
    queryParams.addQueryItem("pageNum", "1");
    queryParams.addQueryItem("pageSize", "1000");
    
    m_networkManager->get(apiUrl, [this, timer](const QJsonObject &rootObj) {
        // 记录API请求成功性能
        qint64 responseTime = timer.elapsed();
        Analytics::SDK::instance()->trackPerformance("api_response_time", responseTime, {
            {"page", "echarts_page"},
            {"api", "system_logs_stats"},
            {"status", "success"}
        });
        
        if (rootObj["code"].toInt() == 0) {
            QJsonArray rows = rootObj["rows"].toArray();
            
            // 统计各路径的访问次数和响应时间
            QMap<QString, int> pathCounts;
            QMap<QString, QList<int>> pathDurations;
            
            // 输出数据的时间范围
            if (!rows.isEmpty()) {
            }
            
            for (const QJsonValue &rowValue : rows) {
                QJsonObject rowObj = rowValue.toObject();
                QString path = rowObj["path"].toString();
                int duration = rowObj["durationMs"].toInt();
                
                pathCounts[path]++;
                pathDurations[path].append(duration);
            }
            
            // 准备图表数据
            QStringList categories;
            QList<int> counts;
            QList<double> avgDurations;
            
            // 按访问次数排序，取前10个
            QList<QPair<QString, int>> sortedPaths;
            for (auto it = pathCounts.begin(); it != pathCounts.end(); ++it) {
                sortedPaths.append(qMakePair(it.key(), it.value()));
            }
            std::sort(sortedPaths.begin(), sortedPaths.end(), 
                     [](const QPair<QString, int> &a, const QPair<QString, int> &b) {
                         return a.second > b.second;
                     });
            
            int maxItems = qMin(10, sortedPaths.size());
            for (int i = 0; i < maxItems; ++i) {
                QString path = sortedPaths[i].first;
                categories.append(path.length() > 20 ? path.left(20) + "..." : path);
                counts.append(sortedPaths[i].second);
                
                // 计算平均响应时间
                QList<int> durations = pathDurations[path];
                double avgDuration = 0;
                for (int d : durations) {
                    avgDuration += d;
                }
                avgDuration /= durations.size();
                avgDurations.append(avgDuration);
            }
            
            // 获取当前选择的图表类型
            QString chartType = m_chartTypeCombo->currentData().toString();
            
            // 生成JavaScript代码更新图表
            QString jsCode = QString(
                "if (window.updateApiChart) {"
                "    window.updateApiChart(%1, %2, %3, '%4');"
                "} else {"
                "    console.log('updateApiChart函数未找到');"
                "}"
            ).arg(jsonArrayToString(categories))
             .arg(jsonArrayToString(counts))
             .arg(jsonArrayToString(avgDurations))
             .arg(chartType);
            
#ifdef WEBENGINE_AVAILABLE
            m_webView->page()->runJavaScript(jsCode);
#else
            // WebEngine不可用时，更新QLabel显示信息
            QLabel* chartLabel = qobject_cast<QLabel*>(m_webView);
            if (chartLabel) {
                chartLabel->setText("WebEngine 不可用\nECharts功能已禁用\n\n图表数据已更新");
            }
#endif
            
        } else {
            qWarning() << "API返回错误:" << rootObj["msg"].toString();
        }
    }, [this, timer](const QString &errorMsg) {
        // 记录API请求失败性能
        qint64 responseTime = timer.elapsed();
        Analytics::SDK::instance()->trackPerformance("api_response_time", responseTime, {
            {"page", "echarts_page"},
            {"api", "system_logs_stats"},
            {"status", "failed"}
        });
        
        qWarning() << "API请求失败:" << errorMsg;
        // 调用JavaScript显示网络错误提示
        QString jsCode = QString(
            "if (window.showNetworkErrorMessage) {"
            "    window.showNetworkErrorMessage();"
            "} else {"
            "    console.log('showNetworkErrorMessage函数未找到');"
            "}"
        );
        m_webView->page()->runJavaScript(jsCode);
    }, queryParams);
}



QString EChartsTab::jsonArrayToString(const QStringList &list)
{
    QString result = "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) result += ",";
        result += "\"" + list[i] + "\"";
    }
    result += "]";
    return result;
}

QString EChartsTab::jsonArrayToString(const QList<int> &list)
{
    QString result = "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) result += ",";
        result += QString::number(list[i]);
    }
    result += "]";
    return result;
}

QString EChartsTab::jsonArrayToString(const QList<double> &list)
{
    QString result = "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) result += ",";
        result += QString::number(list[i], 'f', 2);
    }
    result += "]";
    return result;
}

void EChartsTab::applyTheme()
{
    ThemeManager *theme = ThemeManager::instance();
    
    // 应用主窗口样式
    this->setStyleSheet(QString(
        "QMainWindow { "
        "    background-color: %1; "
        "    color: %2; "
        "    font-family: %3; "
        "}"
        "QLabel#filterLabel { "
        "    color: %4; "
        "    font-size: %5px; "
        "    font-weight: 500; "
        "}"
    ).arg(theme->colors().BACKGROUND)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_SIZE_SM));
    
    // 应用WebView样式
    if (m_webView) {
#ifdef WEBENGINE_AVAILABLE
        m_webView->setStyleSheet(QString(
            "QWebEngineView#echartsWebView { "
            "    border: 1px solid %1; "
            "    border-radius: %2px; "
            "    background-color: %3; "
            "}"
        ).arg(theme->colors().BORDER)
         .arg(ThemeManager::BorderRadius::MD)
         .arg(theme->colors().SURFACE));
#else
        m_webView->setStyleSheet(QString(
            "QLabel#echartsWebView { "
            "    border: 1px solid %1; "
            "    border-radius: %2px; "
            "    background-color: %3; "
            "    color: %4; "
            "}"
        ).arg(theme->colors().BORDER)
         .arg(ThemeManager::BorderRadius::MD)
         .arg(theme->colors().SURFACE)
         .arg(theme->colors().TEXT_PRIMARY));
#endif
    }
    
    // 应用下拉框样式
    QString comboBoxStyle = QString(
        "QComboBox { "
        "    background-color: %1; "
        "    border: 1px solid %2; "
        "    border-radius: %3px; "
        "    padding: 6px 12px; "
        "    font-size: %4px; "
        "    color: %5; "
        "    font-family: %6; "
        "}"
        "QComboBox:hover { "
        "    border: 1px solid %7; "
        "}"
        "QComboBox:focus { "
        "    border: 1px solid %8; "
        "}"
        "QComboBox::drop-down { "
        "    border: none; "
        "    width: 24px; "
        "}"
        "QComboBox::down-arrow { "
        "    image: none; "
        "    border: none; "
        "}"
        "QComboBox QAbstractItemView { "
        "    background-color: %9; "
        "    border: 1px solid %10; "
        "    border-radius: %11px; "
        "    selection-background-color: %12; "
        "    selection-color: %13; "
        "    outline: none; "
        "}"
    ).arg(theme->colors().SURFACE)
     .arg(theme->colors().BORDER)
     .arg(ThemeManager::BorderRadius::SM)
     .arg(ThemeManager::Typography::FONT_SIZE_SM)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(theme->colors().PRIMARY)
     .arg(theme->colors().PRIMARY)
     .arg(theme->colors().SURFACE)
     .arg(theme->colors().BORDER)
     .arg(ThemeManager::BorderRadius::SM)
     .arg(theme->colors().PRIMARY)
     .arg(theme->colors().SURFACE);
    
    if (m_methodCombo) m_methodCombo->setStyleSheet(comboBoxStyle);
    if (m_platformCombo) m_platformCombo->setStyleSheet(comboBoxStyle);
    if (m_chartTypeCombo) m_chartTypeCombo->setStyleSheet(comboBoxStyle);
    
    // 应用时间选择器样式
    QString dateTimeStyle = QString(
        "QDateTimeEdit { "
        "    background-color: %1; "
        "    border: 1px solid %2; "
        "    border-radius: %3px; "
        "    padding: 6px 12px; "
        "    font-size: %4px; "
        "    color: %5; "
        "    font-family: %6; "
        "}"
        "QDateTimeEdit:hover { "
        "    border: 1px solid %7; "
        "}"
        "QDateTimeEdit:focus { "
        "    border: 1px solid %8; "
        "}"
        "QDateTimeEdit::drop-down { "
        "    border: none; "
        "    width: 24px; "
        "}"
        "QDateTimeEdit::down-arrow { "
        "    image: none; "
        "    border: none; "
        "}"
        "QDateTimeEdit QCalendarWidget { "
        "    background-color: %9; "
        "    border: 1px solid %10; "
        "    border-radius: %11px; "
        "}"
    ).arg(theme->colors().SURFACE)
     .arg(theme->colors().BORDER)
     .arg(ThemeManager::BorderRadius::SM)
     .arg(ThemeManager::Typography::FONT_SIZE_SM)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(theme->colors().PRIMARY)
     .arg(theme->colors().PRIMARY)
     .arg(theme->colors().SURFACE)
     .arg(theme->colors().BORDER)
     .arg(ThemeManager::BorderRadius::SM);
    
    if (m_startTimeEdit) m_startTimeEdit->setStyleSheet(dateTimeStyle);
    if (m_endTimeEdit) m_endTimeEdit->setStyleSheet(dateTimeStyle);
    
    // 应用主按钮样式
    QWidget *fetchButton = this->findChild<QPushButton*>("fetchApiButton");
    if (fetchButton) {
        fetchButton->setStyleSheet(theme->getButtonStyle("primary"));
    }
    
    // 更新快捷按钮样式
    updateButtonHighlight(-2); // 使用特殊值来触发样式更新而不改变高亮状态
}