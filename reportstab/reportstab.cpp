#include "reportstab.h"
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QTimer>

ReportsTab::ReportsTab(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ReportsTab::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 工具栏
    toolbarLayout = new QHBoxLayout();
    toolbarLayout->setSpacing(15);

    QLabel *reportTypeLabel = new QLabel("报表类型:");
    reportTypeCombo = new QComboBox();
    reportTypeCombo->addItem("用户活跃度报表", "user_activity");
    reportTypeCombo->addItem("页面访问报表", "page_access");
    reportTypeCombo->addItem("功能使用报表", "feature_usage");
    reportTypeCombo->addItem("用户画像报表", "user_profile");
    reportTypeCombo->setStyleSheet(
        "QComboBox { "
        "    padding: 8px; "
        "    border: 1px solid #ced4da; "
        "    border-radius: 5px; "
        "    background-color: white; "
        "    min-width: 200px; "
        "} "
        "QComboBox::drop-down { "
        "    border: none; "
        "} "
        "QComboBox::down-arrow { "
        "    image: url(:/icons/down-arrow.png); "
        "}"
    );

    QLabel *startDateLabel = new QLabel("开始日期:");
    startDateEdit = new QDateEdit();
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setDate(QDate::currentDate().addDays(-30));
    startDateEdit->setDisplayFormat("yyyy-MM-dd");

    QLabel *endDateLabel = new QLabel("结束日期:");
    endDateEdit = new QDateEdit();
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setDate(QDate::currentDate());
    endDateEdit->setDisplayFormat("yyyy-MM-dd");

    generateButton = new QPushButton("生成报表");
    generateButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #667eea; "
        "    color: white; "
        "    border: none; "
        "    padding: 8px 20px; "
        "    border-radius: 5px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #5568d3; "
        "}"
    );
    connect(generateButton, &QPushButton::clicked, this, &ReportsTab::onGenerateReport);

    exportButton = new QPushButton("导出报表");
    exportButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #28a745; "
        "    color: white; "
        "    border: none; "
        "    padding: 8px 20px; "
        "    border-radius: 5px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #218838; "
        "}"
    );
    connect(exportButton, &QPushButton::clicked, this, &ReportsTab::onExportReport);

    toolbarLayout->addWidget(reportTypeLabel);
    toolbarLayout->addWidget(reportTypeCombo);
    toolbarLayout->addWidget(startDateLabel);
    toolbarLayout->addWidget(startDateEdit);
    toolbarLayout->addWidget(endDateLabel);
    toolbarLayout->addWidget(endDateEdit);
    toolbarLayout->addWidget(generateButton);
    toolbarLayout->addWidget(exportButton);
    toolbarLayout->addStretch();

    mainLayout->addLayout(toolbarLayout);

    // 滚动区域
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea { "
        "    border: none; "
        "    background-color: #f8f9fa; "
        "}"
    );

    scrollWidget = new QWidget();
    scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setSpacing(20);
    scrollLayout->setContentsMargins(10, 10, 10, 10);

    // 设置各个部分
    setupReportSummary();
    setupTrendChart();
    setupDataTable();

    scrollLayout->addStretch();

    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
}

void ReportsTab::setupReportSummary()
{
    summaryGroup = new QGroupBox("报表摘要");
    summaryGroup->setStyleSheet(
        "QGroupBox { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #495057; "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 10px; "
        "    margin-top: 10px; "
        "    padding-top: 15px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 20px; "
        "    padding: 0 10px; "
        "}"
    );

    QHBoxLayout *groupLayout = new QHBoxLayout(summaryGroup);
    groupLayout->setSpacing(20);

    // 总访问次数
    QWidget *totalVisitsWidget = new QWidget();
    QVBoxLayout *totalVisitsLayout = new QVBoxLayout(totalVisitsWidget);
    totalVisitsLayout->setSpacing(5);
    QLabel *totalVisitsTitle = new QLabel("总访问次数");
    totalVisitsTitle->setStyleSheet("font-size: 14px; color: #6c757d;");
    totalVisitsLabel = new QLabel("-");
    totalVisitsLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #667eea;");
    totalVisitsLayout->addWidget(totalVisitsTitle);
    totalVisitsLayout->addWidget(totalVisitsLabel);
    groupLayout->addWidget(totalVisitsWidget);

    // 活跃用户数
    QWidget *activeUsersWidget = new QWidget();
    QVBoxLayout *activeUsersLayout = new QVBoxLayout(activeUsersWidget);
    activeUsersLayout->setSpacing(5);
    QLabel *activeUsersTitle = new QLabel("活跃用户数");
    activeUsersTitle->setStyleSheet("font-size: 14px; color: #6c757d;");
    activeUsersLabel = new QLabel("-");
    activeUsersLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #28a745;");
    activeUsersLayout->addWidget(activeUsersTitle);
    activeUsersLayout->addWidget(activeUsersLabel);
    groupLayout->addWidget(activeUsersWidget);

    // 平均会话时长
    QWidget *avgSessionWidget = new QWidget();
    QVBoxLayout *avgSessionLayout = new QVBoxLayout(avgSessionWidget);
    avgSessionLayout->setSpacing(5);
    QLabel *avgSessionTitle = new QLabel("平均会话时长");
    avgSessionTitle->setStyleSheet("font-size: 14px; color: #6c757d;");
    avgSessionLabel = new QLabel("-");
    avgSessionLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #ffc107;");
    avgSessionLayout->addWidget(avgSessionTitle);
    avgSessionLayout->addWidget(avgSessionLabel);
    groupLayout->addWidget(avgSessionWidget);

    // 转化率
    QWidget *conversionWidget = new QWidget();
    QVBoxLayout *conversionLayout = new QVBoxLayout(conversionWidget);
    conversionLayout->setSpacing(5);
    QLabel *conversionTitle = new QLabel("转化率");
    conversionTitle->setStyleSheet("font-size: 14px; color: #6c757d;");
    conversionLabel = new QLabel("-");
    conversionLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #fd7e14;");
    conversionLayout->addWidget(conversionTitle);
    conversionLayout->addWidget(conversionLabel);
    groupLayout->addWidget(conversionWidget);

    groupLayout->addStretch();

    scrollLayout->addWidget(summaryGroup);
}

void ReportsTab::setupTrendChart()
{
    trendGroup = new QGroupBox("趋势图表");
    trendGroup->setStyleSheet(
        "QGroupBox { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #495057; "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 10px; "
        "    margin-top: 10px; "
        "    padding-top: 15px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 20px; "
        "    padding: 0 10px; "
        "}"
    );

    QVBoxLayout *groupLayout = new QVBoxLayout(trendGroup);
    groupLayout->setSpacing(10);

    // 图表类型选择
    QHBoxLayout *chartTypeLayout = new QHBoxLayout();
    QLabel *chartTypeLabel = new QLabel("图表类型:");
    chartTypeLabel->setStyleSheet("font-weight: bold; color: #6c757d;");
    chartTypeCombo = new QComboBox();
    chartTypeCombo->addItem("折线图", "line");
    chartTypeCombo->addItem("柱状图", "bar");
    chartTypeCombo->addItem("饼图", "pie");
    chartTypeCombo->setStyleSheet(
        "QComboBox { "
        "    padding: 8px; "
        "    border: 1px solid #ced4da; "
        "    border-radius: 5px; "
        "    background-color: white; "
        "    min-width: 150px; "
        "}"
    );
    connect(chartTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ReportsTab::onChartTypeChanged);
    chartTypeLayout->addWidget(chartTypeLabel);
    chartTypeLayout->addWidget(chartTypeCombo);
    chartTypeLayout->addStretch();

    groupLayout->addLayout(chartTypeLayout);

    // ECharts 图表容器
    chartWidget = new QWidget();
    chartWidget->setMinimumHeight(400);
    chartWidget->setStyleSheet(
        "QWidget { "
        "    border: 1px solid #dee2e6; "
        "    border-radius: 5px; "
        "    background-color: white; "
        "}"
    );
    groupLayout->addWidget(chartWidget);

    scrollLayout->addWidget(trendGroup);
}

void ReportsTab::setupDataTable()
{
    tableGroup = new QGroupBox("数据明细");
    tableGroup->setStyleSheet(
        "QGroupBox { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #495057; "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 10px; "
        "    margin-top: 10px; "
        "    padding-top: 15px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 20px; "
        "    padding: 0 10px; "
        "}"
    );

    QVBoxLayout *groupLayout = new QVBoxLayout(tableGroup);
    groupLayout->setSpacing(10);

    // 搜索框
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("搜索:");
    searchLabel->setStyleSheet("font-weight: bold; color: #6c757d;");
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("输入关键词搜索...");
    searchEdit->setStyleSheet(
        "QLineEdit { "
        "    padding: 8px; "
        "    border: 1px solid #ced4da; "
        "    border-radius: 5px; "
        "    background-color: white; "
        "}"
    );
    connect(searchEdit, &QLineEdit::textChanged, this, &ReportsTab::onSearchTextChanged);
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchEdit);
    searchLayout->addStretch();

    groupLayout->addLayout(searchLayout);

    // 数据表格
    dataTable = new QTableWidget();
    dataTable->setColumnCount(6);
    dataTable->setHorizontalHeaderLabels({"日期", "访问次数", "活跃用户", "新增用户", "会话时长", "转化率"});
    dataTable->horizontalHeader()->setStretchLastSection(true);
    dataTable->verticalHeader()->setVisible(false);
    dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
    dataTable->setSortingEnabled(true);
    dataTable->setStyleSheet(
        "QTableWidget { "
        "    border: 1px solid #dee2e6; "
        "    border-radius: 5px; "
        "    background-color: white; "
        "} "
        "QTableWidget::item { "
        "    padding: 8px; "
        "} "
        "QHeaderView::section { "
        "    background-color: #f8f9fa; "
        "    color: #495057; "
        "    font-weight: bold; "
        "    border: none; "
        "    padding: 8px; "
        "}"
    );
    groupLayout->addWidget(dataTable);

    // 分页控件
    QHBoxLayout *paginationLayout = new QHBoxLayout();
    paginationLayout->setSpacing(10);

    currentPageLabel = new QLabel("第 1 页");
    currentPageLabel->setStyleSheet("color: #6c757d;");

    prevButton = new QPushButton("上一页");
    prevButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #6c757d; "
        "    color: white; "
        "    border: none; "
        "    padding: 5px 15px; "
        "    border-radius: 5px; "
        "} "
        "QPushButton:hover { "
        "    background-color: #5a6268; "
        "} "
        "QPushButton:disabled { "
        "    background-color: #e9ecef; "
        "    color: #6c757d; "
        "}"
    );
    prevButton->setEnabled(false);
    connect(prevButton, &QPushButton::clicked, this, &ReportsTab::onPrevPage);

    nextButton = new QPushButton("下一页");
    nextButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #667eea; "
        "    color: white; "
        "    border: none; "
        "    padding: 5px 15px; "
        "    border-radius: 5px; "
        "} "
        "QPushButton:hover { "
        "    background-color: #5568d3; "
        "} "
        "QPushButton:disabled { "
        "    background-color: #e9ecef; "
        "    color: #6c757d; "
        "}"
    );
    connect(nextButton, &QPushButton::clicked, this, &ReportsTab::onNextPage);

    paginationLayout->addWidget(currentPageLabel);
    paginationLayout->addWidget(prevButton);
    paginationLayout->addWidget(nextButton);
    paginationLayout->addStretch();

    groupLayout->addLayout(paginationLayout);

    scrollLayout->addWidget(tableGroup);
}

void ReportsTab::onGenerateReport()
{
    QString reportType = reportTypeCombo->currentData().toString();
    QString startDate = startDateEdit->date().toString("yyyy-MM-dd");
    QString endDate = endDateEdit->date().toString("yyyy-MM-dd");

    // TODO: 调用后端API生成报表
    // 这里先使用模拟数据
    generateMockReport();
}

void ReportsTab::onExportReport()
{
    // TODO: 实现报表导出功能
    QMessageBox::information(this, "提示", "报表导出功能开发中...");
}

void ReportsTab::onChartTypeChanged(int index)
{
    QString chartType = chartTypeCombo->itemData(index).toString();
    // TODO: 更新图表类型
    updateChart(chartType);
}

void ReportsTab::onSearchTextChanged(const QString &text)
{
    // TODO: 实现表格搜索过滤
    filterDataTable(text);
}

void ReportsTab::onPrevPage()
{
    if (currentPage > 1) {
        currentPage--;
        updateDataTable();
        updatePaginationControls();
    }
}

void ReportsTab::onNextPage()
{
    if (currentPage < totalPages) {
        currentPage++;
        updateDataTable();
        updatePaginationControls();
    }
}

void ReportsTab::generateMockReport()
{
    // 更新报表摘要
    totalVisitsLabel->setText("12,345");
    activeUsersLabel->setText("1,234");
    avgSessionLabel->setText("15.5 分钟");
    conversionLabel->setText("25.8%");

    // 更新数据表格
    dataTable->setRowCount(30);
    for (int i = 0; i < 30; i++) {
        QDate date = startDateEdit->date().addDays(i);
        dataTable->setItem(i, 0, new QTableWidgetItem(date.toString("yyyy-MM-dd")));
        dataTable->setItem(i, 1, new QTableWidgetItem(QString::number(300 + i * 10)));
        dataTable->setItem(i, 2, new QTableWidgetItem(QString::number(50 + i * 2)));
        dataTable->setItem(i, 3, new QTableWidgetItem(QString::number(10 + i)));
        dataTable->setItem(i, 4, new QTableWidgetItem(QString::number(10 + i * 0.5, 'f', 1) + " 分钟"));
        dataTable->setItem(i, 5, new QTableWidgetItem(QString::number(20 + i * 0.5, 'f', 1) + "%"));
    }

    // 更新分页
    currentPage = 1;
    totalPages = 3;
    updatePaginationControls();

    // 更新图表
    updateChart("line");
}

void ReportsTab::updateChart(const QString &chartType)
{
    // TODO: 使用ECharts渲染图表
    // 这里可以集成QtWebEngine来显示ECharts图表
}

void ReportsTab::updateDataTable()
{
    // TODO: 根据当前页码更新表格数据
}

void ReportsTab::updatePaginationControls()
{
    currentPageLabel->setText(QString("第 %1 页").arg(currentPage));
    prevButton->setEnabled(currentPage > 1);
    nextButton->setEnabled(currentPage < totalPages);
}

void ReportsTab::filterDataTable(const QString &text)
{
    // TODO: 实现表格数据过滤
}