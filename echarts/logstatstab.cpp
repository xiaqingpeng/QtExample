#include "logstatstab.h"
#include "common.h"
#include "../analytics/analytics.h"
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

    // 设置窗口样式
    this->setStyleSheet(
        "QWidget { background-color: #f5f7fa; }"
        "QLabel { color: #333; font-size: 13px; font-weight: 500; }"
    );

    // 创建筛选控件
    setupFilterControls();
    mainLayout->addLayout(m_filterLayout);

    // 创建表格
    setupTable();
    mainLayout->addWidget(m_tableWidget);

    // 创建分页控件
    setupPaginationControls();
    mainLayout->addLayout(m_paginationLayout);

    // 初始化默认选择"今天"
    onTimeShortcutClicked(0);
}

void LogStatsTab::setupFilterControls()
{
    m_filterLayout = new QHBoxLayout();

    // Method选择下拉框
    m_methodCombo = new QComboBox(this);
    m_methodCombo->addItem("All", "");  // All表示不筛选
    m_methodCombo->addItem("POST", "POST");
    m_methodCombo->addItem("GET", "GET");
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
    m_filterLayout->addWidget(new QLabel("Method:"));
    m_filterLayout->addWidget(m_methodCombo);

    // Platform选择下拉框
    m_platformCombo = new QComboBox(this);
    m_platformCombo->addItem("All", "");  // All表示不筛选
    m_platformCombo->addItem("Web", "Web");
    m_platformCombo->addItem("Android", "Android");
    m_platformCombo->addItem("iOS", "iOS");
    m_platformCombo->addItem("Windows", "Windows");
    m_platformCombo->addItem("Mac", "Mac");
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
    m_filterLayout->addWidget(new QLabel("Platform:"));
    m_filterLayout->addWidget(m_platformCombo);

    // 创建时间筛选控件
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

    m_filterLayout->addLayout(timeFilterLayout);

    // 创建快捷时间按钮
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
    m_tableWidget->setColumnCount(7);
    m_tableWidget->setHorizontalHeaderLabels({"ID", "Path", "Method", "IP", "Request Time", "Duration (ms)", "Platform"});

    // 设置表格样式
    m_tableWidget->setStyleSheet(
        "QTableWidget {"
        "    background-color: white;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 8px;"
        "    gridline-color: #e0e0e0;"
        "    selection-background-color: #0078d4;"
        "    selection-color: white;"
        "}"
        "QTableWidget::item {"
        "    padding: 8px;"
        "    border-bottom: 1px solid #e0e0e0;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f8f9fa;"
        "    color: #333;"
        "    padding: 10px;"
        "    border: none;"
        "    border-bottom: 2px solid #0078d4;"
        "    font-weight: 600;"
        "}"
    );

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
    m_btnPrevious->setMinimumHeight(36);
    m_btnPrevious->setStyleSheet(
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
        "QPushButton:disabled {"
        "    background-color: #f5f5f5;"
        "    color: #999;"
        "    border: 1px solid #e0e0e0;"
        "}"
    );
    connect(m_btnPrevious, &QPushButton::clicked, this, &LogStatsTab::onPreviousPage);

    // 页码选择器
    m_pageCombo = new QComboBox(this);
    m_pageCombo->setMinimumWidth(80);
    m_pageCombo->setMinimumHeight(36);
    m_pageCombo->setStyleSheet(
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
    );
    connect(m_pageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogStatsTab::onGoToPage);

    // 页码信息标签
    m_pageInfoLabel = new QLabel(this);
    m_pageInfoLabel->setStyleSheet(
        "QLabel {"
        "    color: #666;"
        "    font-size: 13px;"
        "    padding: 0 10px;"
        "}"
    );

    // 下一页按钮
    m_btnNext = new QPushButton("下一页", this);
    m_btnNext->setMinimumHeight(36);
    m_btnNext->setStyleSheet(
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
        "QPushButton:disabled {"
        "    background-color: #f5f5f5;"
        "    color: #999;"
        "    border: 1px solid #e0e0e0;"
        "}"
    );
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

    QString checkedStyle =
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

    // 重置所有按钮样式
    m_btnToday->setStyleSheet(normalStyle);
    m_btnYesterday->setStyleSheet(normalStyle);
    m_btnLast7Days->setStyleSheet(normalStyle);
    m_btnLast30Days->setStyleSheet(normalStyle);
    m_btnClearTime->setStyleSheet(normalStyle);

    // 根据days参数高亮对应按钮
    if (days == 0) {
        m_btnToday->setStyleSheet(checkedStyle);
    } else if (days == 1) {
        m_btnYesterday->setStyleSheet(checkedStyle);
    } else if (days == 7) {
        m_btnLast7Days->setStyleSheet(checkedStyle);
    } else if (days == 30) {
        m_btnLast30Days->setStyleSheet(checkedStyle);
    }
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
