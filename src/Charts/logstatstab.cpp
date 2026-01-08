#include "logstatstab.h"
#include "common.h"
#include "../Analytics/analytics.h"
#include "../Styles/theme_manager.h"
#include <QElapsedTimer>
#include <QHeaderView>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDebug>

// 自定义日志宏：打印文件名、行号和日志信息
// 日志宏已禁用

LogStatsTab::LogStatsTab(QWidget *parent)
    : QWidget(parent)
    , m_networkManager(new NetworkManager(this))
    , m_currentPage(1)
    , m_pageSize(10)
    , m_totalPages(1)
    , m_totalRecords(0)
{
    setupUI();
}

LogStatsTab::~LogStatsTab()
{
}

void LogStatsTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // 创建筛选控件
    setupFilterControls();
    mainLayout->addLayout(m_filterLayout);

    // 创建表格
    setupTable();
    mainLayout->addWidget(m_tableWidget);

    // 创建分页控件
    setupPaginationControls();
    mainLayout->addLayout(m_paginationLayout);

    // 应用主题
    applyTheme();
    
    // 连接主题变化信号
    connect(ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &LogStatsTab::applyTheme);

    // 初始化当前高亮状态
    m_currentHighlightedDays = 0; // 默认选择"今天"
    
    // 初始化默认选择"今天"
    onTimeShortcutClicked(0);
}

void LogStatsTab::setupFilterControls()
{
    m_filterLayout = new QHBoxLayout();

    // Method选择下拉框
    m_methodCombo = new QComboBox(this);
    m_methodCombo->setObjectName("methodCombo");
    m_methodCombo->addItem("All", "");  // All表示不筛选
    m_methodCombo->addItem("POST", "POST");
    m_methodCombo->addItem("GET", "GET");
    m_methodCombo->setCurrentIndex(0);  // 默认选择All
    m_methodCombo->setMinimumWidth(120);
    m_methodCombo->setMinimumHeight(36);
    
    QLabel *methodLabel = new QLabel("Method:");
    methodLabel->setObjectName("filterLabel");
    m_filterLayout->addWidget(methodLabel);
    m_filterLayout->addWidget(m_methodCombo);

    // Platform选择下拉框
    m_platformCombo = new QComboBox(this);
    m_platformCombo->setObjectName("platformCombo");
    m_platformCombo->addItem("All", "");  // All表示不筛选
    m_platformCombo->addItem("Web", "Web");
    m_platformCombo->addItem("Android", "Android");
    m_platformCombo->addItem("iOS", "iOS");
    m_platformCombo->addItem("Windows", "Windows");
    m_platformCombo->addItem("Mac", "Mac");
    m_platformCombo->setCurrentIndex(0);  // 默认选择All
    m_platformCombo->setMinimumWidth(120);
    m_platformCombo->setMinimumHeight(36);
    
    QLabel *platformLabel = new QLabel("Platform:");
    platformLabel->setObjectName("filterLabel");
    m_filterLayout->addWidget(platformLabel);
    m_filterLayout->addWidget(m_platformCombo);

    // 创建时间筛选控件
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

    m_filterLayout->addLayout(timeFilterLayout);

    // 创建快捷时间按钮
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

    m_filterLayout->addLayout(shortcutLayout);

    // 连接快捷按钮信号
    connect(m_btnToday, &QPushButton::clicked, [this]() { onTimeShortcutClicked(0); });
    connect(m_btnYesterday, &QPushButton::clicked, [this]() { onTimeShortcutClicked(1); });
    connect(m_btnLast7Days, &QPushButton::clicked, [this]() { onTimeShortcutClicked(7); });
    connect(m_btnLast30Days, &QPushButton::clicked, [this]() { onTimeShortcutClicked(30); });
    connect(m_btnClearTime, &QPushButton::clicked, [this]() { onTimeShortcutClicked(-1); });

    // 连接时间选择器变化信号
    connect(m_startTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &LogStatsTab::onTimeFilterChanged);
    connect(m_endTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &LogStatsTab::onTimeFilterChanged);

    // 连接筛选条件改变信号
    connect(m_methodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogStatsTab::onFilterChanged);
    connect(m_platformCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogStatsTab::onFilterChanged);
}

void LogStatsTab::setupTable()
{
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setObjectName("logStatsTable");
    m_tableWidget->setColumnCount(7);
    m_tableWidget->setHorizontalHeaderLabels({"ID", "Path", "Method", "IP", "Request Time", "Duration (ms)", "Platform"});

    // 设置表头属性
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

void LogStatsTab::setupPaginationControls()
{
    m_paginationLayout = new QHBoxLayout();

    // 上一页按钮
    m_btnPrevious = new QPushButton("上一页", this);
    m_btnPrevious->setObjectName("paginationButton");
    m_btnPrevious->setMinimumHeight(36);
    connect(m_btnPrevious, &QPushButton::clicked, this, &LogStatsTab::onPreviousPage);

    // 页码选择器
    m_pageCombo = new QComboBox(this);
    m_pageCombo->setObjectName("pageCombo");
    m_pageCombo->setMinimumWidth(80);
    m_pageCombo->setMinimumHeight(36);
    connect(m_pageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogStatsTab::onGoToPage);

    // 页码信息标签
    m_pageInfoLabel = new QLabel(this);
    m_pageInfoLabel->setObjectName("pageInfoLabel");

    // 下一页按钮
    m_btnNext = new QPushButton("下一页", this);
    m_btnNext->setObjectName("paginationButton");
    m_btnNext->setMinimumHeight(36);
    connect(m_btnNext, &QPushButton::clicked, this, &LogStatsTab::onNextPage);

    m_paginationLayout->addWidget(m_btnPrevious);
    m_paginationLayout->addWidget(m_pageCombo);
    m_paginationLayout->addWidget(m_pageInfoLabel);
    m_paginationLayout->addWidget(m_btnNext);
    m_paginationLayout->addStretch();
}

void LogStatsTab::onFilterChanged()
{
    // 当筛选条件改变时，重置到第一页并重新获取数据
    m_currentPage = 1;
    // 筛选条件已改变，重新获取数据
    fetchLogData();
}

void LogStatsTab::onTimeFilterChanged()
{
    // 当时间筛选条件改变时，重置到第一页并重新获取数据
    m_currentPage = 1;
    // 时间筛选条件已改变，重新获取数据
    fetchLogData();
}

void LogStatsTab::onTimeShortcutClicked(int days)
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
        // 选择今天
        // 高亮"今天"按钮
        updateButtonHighlight(0);
    } else if (days == 1) {
        // 昨天
        QDateTime yesterday = now.addDays(-1);
        m_startTimeEdit->setDateTime(QDateTime(yesterday.date(), QTime(0, 0, 0)));
        m_endTimeEdit->setDateTime(QDateTime(yesterday.date(), QTime(23, 59, 59)));
        // 选择昨天
        // 高亮"昨天"按钮
        updateButtonHighlight(1);
    } else if (days > 0) {
        // 最近N天：从N天前的00:00:00到今天的23:59:59
        QDateTime startTime = QDateTime(now.addDays(-days).date(), QTime(0, 0, 0));
        QDateTime endTime = QDateTime(now.date(), QTime(23, 59, 59));
        m_startTimeEdit->setDateTime(startTime);
        m_endTimeEdit->setDateTime(endTime);
        // 选择最近几天
        // 高亮对应的按钮
        updateButtonHighlight(days);
    } else if (days == -1) {
        // 清除时间筛选
        m_startTimeEdit->clear();
        m_endTimeEdit->clear();
        // 清除时间筛选
    }

    // 恢复信号发射
    m_startTimeEdit->blockSignals(false);
    m_endTimeEdit->blockSignals(false);

    // 手动触发一次数据获取
    m_currentPage = 1;
    fetchLogData();
}

void LogStatsTab::fetchLogData()
{
    // 开始性能计时
    QElapsedTimer timer;
    timer.start();
    
    // 构建API URL
    QString apiUrl = "http://120.48.95.51:7001/system/logs/stats";

    // 构建查询参数
    QUrlQuery queryParams;

    // 添加分页参数（使用pageNum而不是page）
    queryParams.addQueryItem("pageNum", QString::number(m_currentPage));
    queryParams.addQueryItem("pageSize", QString::number(m_pageSize));

    // 添加筛选条件
    QString method = m_methodCombo->currentData().toString();
    if (!method.isEmpty()) {
        queryParams.addQueryItem("method", method);
    }

    QString platform = m_platformCombo->currentData().toString();
    if (!platform.isEmpty()) {
        queryParams.addQueryItem("platform", platform);
    }

    // 添加时间范围参数（使用ISO格式）
    if (m_startTimeEdit->dateTime().isValid()) {
        QString startTime = m_startTimeEdit->dateTime().toString(Qt::ISODate);
        if (!startTime.endsWith("Z")) {
            startTime += "Z";
        }
        queryParams.addQueryItem("startTime", startTime);
    }
    if (m_endTimeEdit->dateTime().isValid()) {
        QString endTime = m_endTimeEdit->dateTime().toString(Qt::ISODate);
        if (!endTime.endsWith("Z")) {
            endTime += "Z";
        }
        queryParams.addQueryItem("endTime", endTime);
    }

    // // LOG_DEBUG() << "Fetching log data from:" << apiUrl;
    // // LOG_DEBUG() << "Query params:" << queryParams.toString();

    // 使用NetworkManager发送GET请求
    m_networkManager->get(apiUrl,
        [this, timer](const QJsonObject &rootObj) {
            // 记录API请求性能
            qint64 responseTime = timer.elapsed();
            Analytics::SDK::instance()->trackPerformance("api_response_time", responseTime, {
                {"page", "logstats_page"},
                {"api", "system_logs_stats"},
                {"status", "success"}
            });
            
            // 成功回调
            // // LOG_DEBUG() << "Log data received:" << rootObj;

            // 解析响应数据
            int code = rootObj["code"].toInt();
            if (code != 0) {
                QString msg = rootObj["msg"].toString();
                QMessageBox::warning(this, "错误", "获取日志数据失败: " + msg);
                return;
            }

            // 获取总记录数
            m_totalRecords = rootObj["total"].toInt(0);
            m_totalPages = (m_totalRecords + m_pageSize - 1) / m_pageSize;

            // 清空表格
            m_tableWidget->setRowCount(0);

            // 解析日志数据
            QJsonArray rows = rootObj["rows"].toArray();
            for (int i = 0; i < rows.size(); ++i) {
                QJsonObject logObj = rows[i].toObject();

                int row = m_tableWidget->rowCount();
                m_tableWidget->insertRow(row);

                // ID
                m_tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(logObj["id"].toInt())));

                // Path
                m_tableWidget->setItem(row, 1, new QTableWidgetItem(logObj["path"].toString()));

                // Method
                m_tableWidget->setItem(row, 2, new QTableWidgetItem(logObj["method"].toString()));

                // IP
                m_tableWidget->setItem(row, 3, new QTableWidgetItem(logObj["ip"].toString()));

                // Request Time
                QString requestTime = logObj["requestTime"].toString();
                // 转换为北京时间格式: yyyy-MM-dd HH:mm:ss
                QDateTime utcTime = QDateTime::fromString(requestTime, Qt::ISODate);
                QString displayTime;
                if (utcTime.isValid()) {
                    QDateTime beijingTime = utcTime.toOffsetFromUtc(8 * 3600);
                    displayTime = beijingTime.toString("yyyy-MM-dd HH:mm:ss");
                } else {
                    displayTime = requestTime;
                }
                m_tableWidget->setItem(row, 4, new QTableWidgetItem(displayTime));

                // Duration
                m_tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(logObj["durationMs"].toInt())));

                // Platform
                m_tableWidget->setItem(row, 6, new QTableWidgetItem(logObj["platform"].toString()));
            }

            // 更新分页信息
            updatePaginationInfo();
        },
        [this, timer](const QString &errorMsg) {
            // 记录API请求失败性能
            qint64 responseTime = timer.elapsed();
            Analytics::SDK::instance()->trackPerformance("api_response_time", responseTime, {
                {"page", "logstats_page"},
                {"api", "system_logs_stats"},
                {"status", "failed"}
            });
            
            // 错误回调
            // 获取日志数据失败
            QMessageBox::warning(this, "错误", "获取日志数据失败: " + errorMsg);
        },
        queryParams
    );
}

void LogStatsTab::onPreviousPage()
{
    if (m_currentPage > 1) {
        m_currentPage--;
        fetchLogData();
    }
}

void LogStatsTab::onNextPage()
{
    if (m_currentPage < m_totalPages) {
        m_currentPage++;
        fetchLogData();
    }
}

void LogStatsTab::onGoToPage()
{
    int page = m_pageCombo->currentText().toInt();
    if (page >= 1 && page <= m_totalPages) {
        m_currentPage = page;
        fetchLogData();
    }
}

void LogStatsTab::updateButtonHighlight(int days)
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

    QString checkedStyle = QString(
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
        if (m_btnToday) m_btnToday->setStyleSheet(m_currentHighlightedDays == 0 ? checkedStyle : normalStyle);
        if (m_btnYesterday) m_btnYesterday->setStyleSheet(m_currentHighlightedDays == 1 ? checkedStyle : normalStyle);
        if (m_btnLast7Days) m_btnLast7Days->setStyleSheet(m_currentHighlightedDays == 7 ? checkedStyle : normalStyle);
        if (m_btnLast30Days) m_btnLast30Days->setStyleSheet(m_currentHighlightedDays == 30 ? checkedStyle : normalStyle);
        if (m_btnClearTime) m_btnClearTime->setStyleSheet(m_currentHighlightedDays == -1 ? checkedStyle : normalStyle);
        return;
    }

    // 重置所有按钮样式
    if (m_btnToday) m_btnToday->setStyleSheet(normalStyle);
    if (m_btnYesterday) m_btnYesterday->setStyleSheet(normalStyle);
    if (m_btnLast7Days) m_btnLast7Days->setStyleSheet(normalStyle);
    if (m_btnLast30Days) m_btnLast30Days->setStyleSheet(normalStyle);
    if (m_btnClearTime) m_btnClearTime->setStyleSheet(normalStyle);

    // 根据days参数高亮对应按钮
    if (days == 0) {
        if (m_btnToday) m_btnToday->setStyleSheet(checkedStyle);
        m_currentHighlightedDays = 0;
    } else if (days == 1) {
        if (m_btnYesterday) m_btnYesterday->setStyleSheet(checkedStyle);
        m_currentHighlightedDays = 1;
    } else if (days == 7) {
        if (m_btnLast7Days) m_btnLast7Days->setStyleSheet(checkedStyle);
        m_currentHighlightedDays = 7;
    } else if (days == 30) {
        if (m_btnLast30Days) m_btnLast30Days->setStyleSheet(checkedStyle);
        m_currentHighlightedDays = 30;
    } else if (days == -1) {
        if (m_btnClearTime) m_btnClearTime->setStyleSheet(checkedStyle);
        m_currentHighlightedDays = -1;
    } else {
        m_currentHighlightedDays = days;
    }
}

void LogStatsTab::applyTheme()
{
    ThemeManager *theme = ThemeManager::instance();
    
    // 应用主窗口样式
    this->setStyleSheet(QString(
        "QWidget { "
        "    background-color: %1; "
        "    color: %2; "
        "    font-family: %3; "
        "}"
        "QLabel#filterLabel { "
        "    color: %4; "
        "    font-size: %5px; "
        "    font-weight: 500; "
        "}"
        "QLabel#pageInfoLabel { "
        "    color: %6; "
        "    font-size: %7px; "
        "    padding: 0 10px; "
        "}"
    ).arg(theme->colors().BACKGROUND)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_SIZE_SM)
     .arg(theme->colors().TEXT_SECONDARY)
     .arg(ThemeManager::Typography::FONT_SIZE_SM));
    
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
    if (m_pageCombo) m_pageCombo->setStyleSheet(comboBoxStyle);
    
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
    
    // 应用表格样式
    if (m_tableWidget) {
        m_tableWidget->setStyleSheet(QString(
            "QTableWidget#logStatsTable { "
            "    background-color: %1; "
            "    border: 1px solid %2; "
            "    border-radius: %3px; "
            "    gridline-color: %4; "
            "    selection-background-color: %5; "
            "    selection-color: %6; "
            "}"
            "QTableWidget#logStatsTable::item { "
            "    padding: 8px; "
            "    border-bottom: 1px solid %7; "
            "}"
            "QHeaderView::section { "
            "    background-color: %8; "
            "    color: %9; "
            "    padding: 10px; "
            "    border: none; "
            "    border-bottom: 2px solid %10; "
            "    font-weight: 600; "
            "}"
        ).arg(theme->colors().SURFACE)
         .arg(theme->colors().BORDER)
         .arg(ThemeManager::BorderRadius::MD)
         .arg(theme->colors().BORDER)
         .arg(theme->colors().PRIMARY)
         .arg(theme->colors().SURFACE)
         .arg(theme->colors().BORDER)
         .arg(theme->colors().GRAY_100)
         .arg(theme->colors().TEXT_PRIMARY)
         .arg(theme->colors().PRIMARY));
    }
    
    // 应用分页按钮样式
    QString paginationButtonStyle = QString(
        "QPushButton#paginationButton { "
        "    background-color: %1; "
        "    border: 1px solid %2; "
        "    border-radius: %3px; "
        "    padding: 8px 16px; "
        "    font-size: %4px; "
        "    color: %5; "
        "    font-family: %6; "
        "}"
        "QPushButton#paginationButton:hover { "
        "    background-color: %7; "
        "    border: 1px solid %8; "
        "}"
        "QPushButton#paginationButton:disabled { "
        "    background-color: %9; "
        "    color: %10; "
        "    border: 1px solid %11; "
        "}"
    ).arg(theme->colors().SURFACE)
     .arg(theme->colors().BORDER)
     .arg(ThemeManager::BorderRadius::SM)
     .arg(ThemeManager::Typography::FONT_SIZE_SM)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(theme->colors().GRAY_100)
     .arg(theme->colors().PRIMARY)
     .arg(theme->colors().GRAY_100)
     .arg(theme->colors().TEXT_DISABLED)
     .arg(theme->colors().BORDER);
    
    if (m_btnPrevious) m_btnPrevious->setStyleSheet(paginationButtonStyle);
    if (m_btnNext) m_btnNext->setStyleSheet(paginationButtonStyle);
    
    // 更新快捷按钮样式
    updateButtonHighlight(-2); // 使用特殊值来触发样式更新而不改变高亮状态
}

void LogStatsTab::updatePaginationInfo()
{
    // 更新页码选择器
    m_pageCombo->blockSignals(true);  // 阻止信号，避免重复触发
    m_pageCombo->clear();
    for (int i = 1; i <= m_totalPages; ++i) {
        m_pageCombo->addItem(QString::number(i));
    }
    m_pageCombo->setCurrentIndex(m_currentPage - 1);
    m_pageCombo->blockSignals(false);  // 恢复信号

    // 更新页码信息标签
    m_pageInfoLabel->setText(QString("共 %1 条记录，第 %2/%3 页").arg(m_totalRecords).arg(m_currentPage).arg(m_totalPages));

    // 更新按钮状态
    m_btnPrevious->setEnabled(m_currentPage > 1);
    m_btnNext->setEnabled(m_currentPage < m_totalPages);
}
