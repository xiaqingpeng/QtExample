#include "echartstab.h"
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QWebChannel>
#include <QWebEngineView>
#include <QFile>
#include "networkmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QList>
#include <QDateTimeEdit>
#include <QDate>
#include "echartstab.h"
#include "common.h"
#include "analytics/analytics.h"

EChartsTab::EChartsTab(QWidget *parent)
    : QMainWindow(parent)
    , m_networkManager(new NetworkManager(this))
{
    // 1. 设置窗口基本属性
    this->setWindowTitle("Qt + ECharts Demo");
    this->resize(800, 600);
    
    // 设置窗口样式
    this->setStyleSheet(
        "QMainWindow { background-color: #f5f7fa; }"
        "QLabel { color: #333; font-size: 13px; font-weight: 500; }"
    );

    // 2. 创建中心部件和布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);
    this->setCentralWidget(centralWidget);

    // 3. 创建WebEngineView，加载ECharts HTML页面
    m_webView = new QWebEngineView(this);
    m_webView->setStyleSheet(
        "QWebEngineView { border: 1px solid #e0e0e0; border-radius: 8px; background-color: white; }"
    );
    layout->addWidget(m_webView);

    // 4. 创建桥接对象，实现Qt与JS交互
    m_bridge = new ChartBridge(this);
    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject("qt", m_bridge);  // 注册为window.qt对象（供JS调用）
    m_webView->page()->setWebChannel(channel);

    // 5. 加载本地HTML文件（使用绝对路径确保文件能被找到）
    QString htmlPath = "/Applications/qingpengxia/qt/qt6/example/echarts/ECharts/chart.html";
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
    m_methodCombo->addItem("All", "");  // All表示不筛选
    m_methodCombo->addItem("Post", "Post");
    m_methodCombo->addItem("Get", "Get");
    m_methodCombo->setCurrentIndex(0);  // 默认选择All
    m_methodCombo->setMinimumWidth(120);
    m_methodCombo->setMinimumHeight(36);
    m_methodCombo->setStyleSheet(
        "QComboBox {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    font-size: 13px;"
        "    color: #333;"
        "}"
        "QComboBox:hover { border: 1px solid #0078d4; }"
        "QComboBox:focus { border: 1px solid #0078d4; }"
        "QComboBox::drop-down { border: none; width: 24px; }"
        "QComboBox::down-arrow { image: none; border: none; }"
        "QComboBox QAbstractItemView {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "    selection-background-color: #0078d4;"
        "    selection-color: white;"
        "    outline: none;"
        "}"
    );
    filterLayout->addWidget(new QLabel("Method:"));
    filterLayout->addWidget(m_methodCombo);
    
    // Platform选择下拉框
    m_platformCombo = new QComboBox(this);
    m_platformCombo->addItem("All", "");  // All表示不筛选
    m_platformCombo->addItem("Web", "web");
    m_platformCombo->addItem("Android", "android");
    m_platformCombo->addItem("IOS", "ios");
    m_platformCombo->addItem("Windows", "windows");
    m_platformCombo->addItem("Mac", "mac");
    m_platformCombo->setCurrentIndex(0);  // 默认选择All
    m_platformCombo->setMinimumWidth(120);
    m_platformCombo->setMinimumHeight(36);
    m_platformCombo->setStyleSheet(
        "QComboBox {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    font-size: 13px;"
        "    color: #333;"
        "}"
        "QComboBox:hover { border: 1px solid #0078d4; }"
        "QComboBox:focus { border: 1px solid #0078d4; }"
        "QComboBox::drop-down { border: none; width: 24px; }"
        "QComboBox::down-arrow { image: none; border: none; }"
        "QComboBox QAbstractItemView {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "    selection-background-color: #0078d4;"
        "    selection-color: white;"
        "    outline: none;"
        "}"
    );
    filterLayout->addWidget(new QLabel("Platform:"));
    filterLayout->addWidget(m_platformCombo);
    
    // 图表类型选择下拉框
    m_chartTypeCombo = new QComboBox(this);
    m_chartTypeCombo->addItem("柱状图", "bar");
    m_chartTypeCombo->addItem("折线图", "line");
    m_chartTypeCombo->addItem("饼图", "pie");
    m_chartTypeCombo->addItem("雷达图", "radar");
    m_chartTypeCombo->setCurrentIndex(0);  // 默认选择柱状图
    m_chartTypeCombo->setMinimumWidth(120);
    m_chartTypeCombo->setMinimumHeight(36);
    m_chartTypeCombo->setStyleSheet(
        "QComboBox {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    font-size: 13px;"
        "    color: #333;"
        "}"
        "QComboBox:hover { border: 1px solid #0078d4; }"
        "QComboBox:focus { border: 1px solid #0078d4; }"
        "QComboBox::drop-down { border: none; width: 24px; }"
        "QComboBox::down-arrow { image: none; border: none; }"
        "QComboBox QAbstractItemView {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "    selection-background-color: #0078d4;"
        "    selection-color: white;"
        "    outline: none;"
        "}"
    );
    filterLayout->addWidget(new QLabel("图表:"));
    filterLayout->addWidget(m_chartTypeCombo);
    
    layout->addLayout(filterLayout);
    
    // 6.1 创建时间筛选控件
    QHBoxLayout *timeFilterLayout = new QHBoxLayout();
    
    // 开始时间选择器
    timeFilterLayout->addWidget(new QLabel("开始时间:"));
    m_startTimeEdit = new QDateTimeEdit(this);
    m_startTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    m_startTimeEdit->setCalendarPopup(true);
    m_startTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(-7));  // 默认7天前
    m_startTimeEdit->setMinimumHeight(36);
    m_startTimeEdit->setStyleSheet(
        "QDateTimeEdit {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    font-size: 13px;"
        "    color: #333;"
        "}"
        "QDateTimeEdit:hover { border: 1px solid #0078d4; }"
        "QDateTimeEdit:focus { border: 1px solid #0078d4; }"
        "QDateTimeEdit::drop-down { border: none; width: 24px; }"
        "QDateTimeEdit::down-arrow { image: none; border: none; }"
        "QDateTimeEdit QCalendarWidget {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "}"
    );
    timeFilterLayout->addWidget(m_startTimeEdit);
    
    // 结束时间选择器
    timeFilterLayout->addWidget(new QLabel("结束时间:"));
    m_endTimeEdit = new QDateTimeEdit(this);
    m_endTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    m_endTimeEdit->setCalendarPopup(true);
    m_endTimeEdit->setDateTime(QDateTime::currentDateTime());  // 默认当前时间
    m_endTimeEdit->setMinimumHeight(36);
    m_endTimeEdit->setStyleSheet(
        "QDateTimeEdit {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    font-size: 13px;"
        "    color: #333;"
        "}"
        "QDateTimeEdit:hover { border: 1px solid #0078d4; }"
        "QDateTimeEdit:focus { border: 1px solid #0078d4; }"
        "QDateTimeEdit::drop-down { border: none; width: 24px; }"
        "QDateTimeEdit::down-arrow { image: none; border: none; }"
        "QDateTimeEdit QCalendarWidget {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "}"
    );
    timeFilterLayout->addWidget(m_endTimeEdit);
    
    layout->addLayout(timeFilterLayout);
    
    // 6.2 创建快捷时间按钮
    QHBoxLayout *shortcutLayout = new QHBoxLayout();
    
    m_btnToday = new QPushButton("今天", this);
    m_btnYesterday = new QPushButton("昨天", this);
    m_btnLast7Days = new QPushButton("最近7天", this);
    m_btnLast30Days = new QPushButton("最近30天", this);
    m_btnClearTime = new QPushButton("清除时间", this);
    
    // 设置快捷按钮的初始样式
    QString normalButtonStyle =
        "QPushButton {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "    padding: 8px 16px;"
        "    font-size: 13px;"
        "    color: #333;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f0f0f0;"
        "    border: 1px solid #0078d4;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #e0e0e0;"
        "}"
        "QPushButton:checked {"
        "    background-color: #0078d4;"
        "    color: white;"
        "    border: 1px solid #005a9e;"
        "}";
    
    m_btnToday->setStyleSheet(normalButtonStyle);
    m_btnYesterday->setStyleSheet(normalButtonStyle);
    m_btnLast7Days->setStyleSheet(normalButtonStyle);
    m_btnLast30Days->setStyleSheet(normalButtonStyle);
    m_btnClearTime->setStyleSheet(normalButtonStyle);
    
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
    btnFetchApi->setMinimumHeight(40);
    btnFetchApi->setStyleSheet(
        "QPushButton {"
        "    background-color: #0078d4;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 10px 20px;"
        "    font-size: 14px;"
        "    font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "    background-color: #106ebe;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #005a9e;"
        "}"
    );
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
    // 定义按钮样式
    QString normalStyle =
        "QPushButton {"
        "    background-color: white;"
        "    border: 1px solid #d0d0d0;"
        "    border-radius: 6px;"
        "    padding: 8px 16px;"
        "    font-size: 13px;"
        "    color: #333;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f0f0f0;"
        "    border: 1px solid #0078d4;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #e0e0e0;"
        "}";
    QString highlightStyle =
        "QPushButton {"
        "    background-color: #0078d4;"
        "    color: white;"
        "    border: 1px solid #005a9e;"
        "    border-radius: 6px;"
        "    padding: 8px 16px;"
        "    font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #106ebe;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #005a9e;"
        "}";
    
    // 重置所有按钮为普通样式
    m_btnToday->setStyleSheet(normalStyle);
    m_btnYesterday->setStyleSheet(normalStyle);
    m_btnLast7Days->setStyleSheet(normalStyle);
    m_btnLast30Days->setStyleSheet(normalStyle);
    m_btnClearTime->setStyleSheet(normalStyle);
    
    // 根据days参数高亮对应按钮
    switch (days) {
        case 0:
            m_btnToday->setStyleSheet(highlightStyle);
            break;
        case 1:
            m_btnYesterday->setStyleSheet(highlightStyle);
            break;
        case 7:
            m_btnLast7Days->setStyleSheet(highlightStyle);
            break;
        case 30:
            m_btnLast30Days->setStyleSheet(highlightStyle);
            break;
        case -1:
            m_btnClearTime->setStyleSheet(highlightStyle);
            break;
    }
}

void EChartsTab::onPageLoaded(bool ok)
{
    if (ok) {
        // 页面加载完成后，延迟一小段时间确保JavaScript环境准备好
        QTimer::singleShot(500, this, &EChartsTab::fetchApiData);
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
    
    // 添加查询参数（只添加非空的参数）
    QList<QString> params;
    if (!method.isEmpty()) {
        params.append(QString("method=%1").arg(method));
    }
    if (!platform.isEmpty()) {
        params.append(QString("platform=%1").arg(platform));
    }
    if (!startTime.isEmpty()) {
        params.append(QString("startTime=%1").arg(startTime));
    }
    if (!endTime.isEmpty()) {
        params.append(QString("endTime=%1").arg(endTime));
    }
    
    // 添加分页参数（获取更多数据）
    params.append(QString("pageNum=%1").arg(1));
    params.append(QString("pageSize=%1").arg(1000));
    
    if (!params.isEmpty()) {
        apiUrl += "?" + params.join("&");
    }
    
    m_networkManager->get(apiUrl, [this, timer](const QJsonObject &rootObj) {
        // 记录API请求性能
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
            
            m_webView->page()->runJavaScript(jsCode);
            
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
    });
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